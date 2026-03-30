/**
 * \file
 * NuttX interp-to-native trampoline runtime
 *
 * Provides per-signature C trampolines so the Mono interpreter can call
 * native functions (icalls and P/Invoke) on NuttX/ARM without JIT.
 *
 * Follows the same pattern as aot-runtime-wasm.c: encode the method
 * signature as a "cookie" string and look up a pre-compiled C invoke
 * wrapper that matches.
 */

#include "config.h"

#include <sys/types.h>
#include <string.h>

#include "mini.h"
#include "interp/interp.h"

#ifdef __NuttX__

static char
type_to_c (MonoType *t)
{
	if (m_type_is_byref (t))
		return 'I';

handle_enum:
	switch (t->type) {
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
	case MONO_TYPE_I:
	case MONO_TYPE_U:
	case MONO_TYPE_PTR:
	case MONO_TYPE_FNPTR:
	case MONO_TYPE_SZARRAY:
	case MONO_TYPE_CLASS:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_STRING:
		return 'I';
	case MONO_TYPE_R4:
		return 'F';
	case MONO_TYPE_R8:
		return 'D';
	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
		return 'L';
	case MONO_TYPE_VOID:
		return 'V';
	case MONO_TYPE_VALUETYPE:
		if (m_class_is_enumtype (m_type_data_get_klass_unchecked (t))) {
			t = mono_class_enum_basetype_internal (m_type_data_get_klass_unchecked (t));
			goto handle_enum;
		}
		/* ARM AAPCS: structs ≤4 bytes in one register, 5-8 bytes in two
		 * registers (by value), >8 bytes by pointer.  The interpreter's
		 * PINVOKE_ARG_VTYPE stores a pointer in iargs[]; 'S' tells the
		 * trampoline to dereference it and pass as two ints, matching
		 * the callee's by-value expectation for 8-byte structs like
		 * MonoQCallTypeHandle. */
		{
			int size = mono_class_native_size (m_type_data_get_klass_unchecked (t), NULL);
			if (size > 4 && size <= 8)
				return 'S';
		}
		return 'I';
	case MONO_TYPE_GENERICINST: {
		MonoClass *klass = mono_class_from_mono_type_internal (t);
		if (m_class_is_valuetype (klass)) {
			int size = mono_class_native_size (klass, NULL);
			if (size > 4 && size <= 8)
				return 'S';
			return 'I';
		}
		return 'I';
	}
	default:
		g_warning ("NuttX trampoline: can't translate type %s (0x%x)",
			mono_type_full_name (t), t->type);
		return 'X';
	}
}

typedef union {
	gint64 l;
	struct {
		gint32 lo;
		gint32 hi;
	} pair;
} interp_pair;

#include "nuttx_m2n_invoke.g.h"

gpointer
mono_nuttx_get_interp_to_native_trampoline (MonoMethodSignature *sig)
{
	char cookie [32];
	int offset = 1;

	memset (cookie, 0, sizeof (cookie));
	cookie [0] = type_to_c (sig->ret);

	int c_count = sig->param_count + sig->hasthis + 1;
	g_assert (c_count < (int)sizeof (cookie));

	if (sig->hasthis) {
		cookie [offset] = 'I';
		offset++;
	}
	for (int i = 0; i < sig->param_count; ++i) {
		cookie [offset + i] = type_to_c (sig->params [i]);
	}

	/* Look up the invoke wrapper in the table */
	for (int i = 0; i < nuttx_invoke_table_size; i++) {
		if (strcmp (cookie, nuttx_invoke_table [i].cookie) == 0)
			return (gpointer) nuttx_invoke_table [i].func;
	}

	g_error ("NuttX interp-to-native: no trampoline for signature cookie '%s' "
		"(method: %s)", cookie,
		sig->has_type_parameters ? "<generic>" : "unknown");
	return NULL;
}

/* ========================================================================
 * Native-to-interp trampolines (reverse direction)
 *
 * When native code calls into managed code (delegate callbacks, thread
 * entry points, etc.), we need a unique C function pointer per method.
 * Each thunk:
 *   1. Saves ARM R0-R3 and VFP D0-D7 into a CallContext on the stack
 *   2. Looks up its MonoFtnDesc from a static pool (via index baked into code)
 *   3. Calls interp_entry_from_trampoline(CallContext*, InterpMethod*)
 *   4. Restores return value registers from CallContext
 *   5. Returns to the native caller
 *
 * The thunk pool is pre-compiled at build time.  Methods are assigned
 * thunks on demand via mono_nuttx_get_native_to_interp_trampoline().
 * ======================================================================== */

#define NUTTX_N2I_MAX_THUNKS 128

/* Pool of ftndesc pointers — one per allocated thunk */
static MonoFtnDesc *n2i_ftndesc_pool [NUTTX_N2I_MAX_THUNKS] __attribute__((used));
static int n2i_next_thunk;

/*
 * Common native-to-interp trampoline.
 *
 * Called by each thunk with R12 = MonoFtnDesc*.
 * Saves all ARM argument registers into a CallContext on the stack,
 * invokes interp_entry_from_trampoline(ccontext, imethod), then
 * restores the return value and returns to the native caller.
 *
 * CallContext layout (88 bytes):
 *   [sp+0]  gregs[0..3]  = R0-R3       (16 bytes)
 *   [sp+16] fregs[0..15] = D0-D7       (64 bytes)
 *   [sp+80] stack_size                  (4 bytes)
 *   [sp+84] *stack                      (4 bytes)
 */
void nuttx_n2i_common_trampoline (void);

__attribute__((naked, used))
void
nuttx_n2i_common_trampoline (void)
{
	/*
	 * On entry: R12 = MonoFtnDesc*
	 *   ftndesc->addr (offset 0) = interp_entry_from_trampoline
	 *   ftndesc->arg  (offset 4) = InterpMethod*
	 * R0-R3, D0-D7 = native caller's arguments
	 * LR = return address
	 */
	__asm__ volatile (
		/* ---- prologue ---- */
		"push  {r4, r7, lr}\n"            /* save r4 (scratch), r7 (fp), lr */
		"sub   sp, sp, #4\n"              /* align stack to 8 bytes */
		"mov   r7, sp\n"
		"mov   r4, r12\n"                 /* r4 = ftndesc (preserved) */

		/* Allocate CallContext (88 bytes, 8-byte aligned) */
		"sub   sp, sp, #88\n"

		/* Save integer arg registers → gregs[0..3] */
		"str   r0, [sp, #0]\n"
		"str   r1, [sp, #4]\n"
		"str   r2, [sp, #8]\n"
		"str   r3, [sp, #12]\n"

		/* Save VFP double registers → fregs (offset 16, 64 bytes) */
		"add   r0, sp, #16\n"
		"vstmia r0, {d0-d7}\n"

		/* stack pointer for stack-passed args:
		 * above our saved {r4, r7, lr} + 4-byte align pad = +16 bytes */
		"add   r0, r7, #16\n"
		"str   r0, [sp, #84]\n"           /* ccontext->stack */
		"mov   r0, #0\n"
		"str   r0, [sp, #80]\n"           /* ccontext->stack_size = 0 */

		/* Call entry function indirectly through ftndesc:
		 * R0 = CallContext*  (sp)
		 * R1 = ftndesc->arg = InterpMethod*
		 * R12 = ftndesc->addr = interp_entry_from_trampoline */
		"mov   r0, sp\n"
		"ldr   r1, [r4, #4]\n"
		"ldr   r12, [r4, #0]\n"
		"blx   r12\n"

		/* ---- restore return value ---- */
		/* Integer return in R0 (and R1 for int64) */
		"ldr   r0, [sp, #0]\n"
		"ldr   r1, [sp, #4]\n"

		/* Float/double return in D0 */
		"add   r2, sp, #16\n"
		"vldmia r2, {d0}\n"

		/* ---- epilogue ---- */
		"mov   sp, r7\n"
		"add   sp, sp, #4\n"              /* undo align pad */
		"pop   {r4, r7, pc}\n"
	);
}

/*
 * Per-method thunk functions.
 *
 * Each thunk loads its MonoFtnDesc* from n2i_ftndesc_pool[N] into R12,
 * then branches to the common trampoline.
 *
 * Using GCC inline asm with "i" constraint to bake the pool offset.
 * The local label "1:" is unique per function (naked function scope).
 */
#define NUTTX_N2I_THUNK(N)                                            \
__attribute__((naked, used))                                          \
static void nuttx_n2i_thunk_##N (void)                                \
{                                                                     \
	__asm__ volatile (                                            \
		"ldr   r12, 1f\n"                                    \
		"ldr   r12, [r12]\n"                                 \
		"b.w   nuttx_n2i_common_trampoline\n"                \
		".align 2\n"                                         \
		"1: .word n2i_ftndesc_pool + %c[off]\n"              \
		: : [off] "i" ((N) * (int)sizeof(MonoFtnDesc*))      \
	);                                                            \
}

/* Generate 128 thunks */
NUTTX_N2I_THUNK(0)   NUTTX_N2I_THUNK(1)   NUTTX_N2I_THUNK(2)   NUTTX_N2I_THUNK(3)
NUTTX_N2I_THUNK(4)   NUTTX_N2I_THUNK(5)   NUTTX_N2I_THUNK(6)   NUTTX_N2I_THUNK(7)
NUTTX_N2I_THUNK(8)   NUTTX_N2I_THUNK(9)   NUTTX_N2I_THUNK(10)  NUTTX_N2I_THUNK(11)
NUTTX_N2I_THUNK(12)  NUTTX_N2I_THUNK(13)  NUTTX_N2I_THUNK(14)  NUTTX_N2I_THUNK(15)
NUTTX_N2I_THUNK(16)  NUTTX_N2I_THUNK(17)  NUTTX_N2I_THUNK(18)  NUTTX_N2I_THUNK(19)
NUTTX_N2I_THUNK(20)  NUTTX_N2I_THUNK(21)  NUTTX_N2I_THUNK(22)  NUTTX_N2I_THUNK(23)
NUTTX_N2I_THUNK(24)  NUTTX_N2I_THUNK(25)  NUTTX_N2I_THUNK(26)  NUTTX_N2I_THUNK(27)
NUTTX_N2I_THUNK(28)  NUTTX_N2I_THUNK(29)  NUTTX_N2I_THUNK(30)  NUTTX_N2I_THUNK(31)
NUTTX_N2I_THUNK(32)  NUTTX_N2I_THUNK(33)  NUTTX_N2I_THUNK(34)  NUTTX_N2I_THUNK(35)
NUTTX_N2I_THUNK(36)  NUTTX_N2I_THUNK(37)  NUTTX_N2I_THUNK(38)  NUTTX_N2I_THUNK(39)
NUTTX_N2I_THUNK(40)  NUTTX_N2I_THUNK(41)  NUTTX_N2I_THUNK(42)  NUTTX_N2I_THUNK(43)
NUTTX_N2I_THUNK(44)  NUTTX_N2I_THUNK(45)  NUTTX_N2I_THUNK(46)  NUTTX_N2I_THUNK(47)
NUTTX_N2I_THUNK(48)  NUTTX_N2I_THUNK(49)  NUTTX_N2I_THUNK(50)  NUTTX_N2I_THUNK(51)
NUTTX_N2I_THUNK(52)  NUTTX_N2I_THUNK(53)  NUTTX_N2I_THUNK(54)  NUTTX_N2I_THUNK(55)
NUTTX_N2I_THUNK(56)  NUTTX_N2I_THUNK(57)  NUTTX_N2I_THUNK(58)  NUTTX_N2I_THUNK(59)
NUTTX_N2I_THUNK(60)  NUTTX_N2I_THUNK(61)  NUTTX_N2I_THUNK(62)  NUTTX_N2I_THUNK(63)
NUTTX_N2I_THUNK(64)  NUTTX_N2I_THUNK(65)  NUTTX_N2I_THUNK(66)  NUTTX_N2I_THUNK(67)
NUTTX_N2I_THUNK(68)  NUTTX_N2I_THUNK(69)  NUTTX_N2I_THUNK(70)  NUTTX_N2I_THUNK(71)
NUTTX_N2I_THUNK(72)  NUTTX_N2I_THUNK(73)  NUTTX_N2I_THUNK(74)  NUTTX_N2I_THUNK(75)
NUTTX_N2I_THUNK(76)  NUTTX_N2I_THUNK(77)  NUTTX_N2I_THUNK(78)  NUTTX_N2I_THUNK(79)
NUTTX_N2I_THUNK(80)  NUTTX_N2I_THUNK(81)  NUTTX_N2I_THUNK(82)  NUTTX_N2I_THUNK(83)
NUTTX_N2I_THUNK(84)  NUTTX_N2I_THUNK(85)  NUTTX_N2I_THUNK(86)  NUTTX_N2I_THUNK(87)
NUTTX_N2I_THUNK(88)  NUTTX_N2I_THUNK(89)  NUTTX_N2I_THUNK(90)  NUTTX_N2I_THUNK(91)
NUTTX_N2I_THUNK(92)  NUTTX_N2I_THUNK(93)  NUTTX_N2I_THUNK(94)  NUTTX_N2I_THUNK(95)
NUTTX_N2I_THUNK(96)  NUTTX_N2I_THUNK(97)  NUTTX_N2I_THUNK(98)  NUTTX_N2I_THUNK(99)
NUTTX_N2I_THUNK(100) NUTTX_N2I_THUNK(101) NUTTX_N2I_THUNK(102) NUTTX_N2I_THUNK(103)
NUTTX_N2I_THUNK(104) NUTTX_N2I_THUNK(105) NUTTX_N2I_THUNK(106) NUTTX_N2I_THUNK(107)
NUTTX_N2I_THUNK(108) NUTTX_N2I_THUNK(109) NUTTX_N2I_THUNK(110) NUTTX_N2I_THUNK(111)
NUTTX_N2I_THUNK(112) NUTTX_N2I_THUNK(113) NUTTX_N2I_THUNK(114) NUTTX_N2I_THUNK(115)
NUTTX_N2I_THUNK(116) NUTTX_N2I_THUNK(117) NUTTX_N2I_THUNK(118) NUTTX_N2I_THUNK(119)
NUTTX_N2I_THUNK(120) NUTTX_N2I_THUNK(121) NUTTX_N2I_THUNK(122) NUTTX_N2I_THUNK(123)
NUTTX_N2I_THUNK(124) NUTTX_N2I_THUNK(125) NUTTX_N2I_THUNK(126) NUTTX_N2I_THUNK(127)

/* Lookup table: thunk index → function pointer */
static gpointer n2i_thunk_ptrs [NUTTX_N2I_MAX_THUNKS] = {
#define T(N) (gpointer)nuttx_n2i_thunk_##N
	T(0),   T(1),   T(2),   T(3),   T(4),   T(5),   T(6),   T(7),
	T(8),   T(9),   T(10),  T(11),  T(12),  T(13),  T(14),  T(15),
	T(16),  T(17),  T(18),  T(19),  T(20),  T(21),  T(22),  T(23),
	T(24),  T(25),  T(26),  T(27),  T(28),  T(29),  T(30),  T(31),
	T(32),  T(33),  T(34),  T(35),  T(36),  T(37),  T(38),  T(39),
	T(40),  T(41),  T(42),  T(43),  T(44),  T(45),  T(46),  T(47),
	T(48),  T(49),  T(50),  T(51),  T(52),  T(53),  T(54),  T(55),
	T(56),  T(57),  T(58),  T(59),  T(60),  T(61),  T(62),  T(63),
	T(64),  T(65),  T(66),  T(67),  T(68),  T(69),  T(70),  T(71),
	T(72),  T(73),  T(74),  T(75),  T(76),  T(77),  T(78),  T(79),
	T(80),  T(81),  T(82),  T(83),  T(84),  T(85),  T(86),  T(87),
	T(88),  T(89),  T(90),  T(91),  T(92),  T(93),  T(94),  T(95),
	T(96),  T(97),  T(98),  T(99),  T(100), T(101), T(102), T(103),
	T(104), T(105), T(106), T(107), T(108), T(109), T(110), T(111),
	T(112), T(113), T(114), T(115), T(116), T(117), T(118), T(119),
	T(120), T(121), T(122), T(123), T(124), T(125), T(126), T(127),
#undef T
};

gpointer
mono_nuttx_get_native_to_interp_trampoline (MonoMethod *method, MonoFtnDesc *ftndesc)
{
	if (n2i_next_thunk >= NUTTX_N2I_MAX_THUNKS) {
		char *name = mono_method_get_full_name (method);
		g_warning ("NuttX native-to-interp: thunk pool exhausted (%d/%d), "
			"cannot create trampoline for '%s'",
			n2i_next_thunk, NUTTX_N2I_MAX_THUNKS, name);
		g_free (name);
		return NULL;
	}

	int idx = n2i_next_thunk++;
	n2i_ftndesc_pool [idx] = ftndesc;

	return n2i_thunk_ptrs [idx];
}

#else /* __NuttX__ */

/* Empty source when not building for NuttX */
MONO_EMPTY_SOURCE_FILE (aot_runtime_nuttx);

#endif /* __NuttX__ */
