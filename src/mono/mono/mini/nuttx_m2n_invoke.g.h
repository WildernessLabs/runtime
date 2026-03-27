/*
 * NuttX interp-to-native invoke wrappers for ARM (32-bit AAPCS).
 *
 * Each function unpacks InterpMethodArguments and calls the target
 * native function with the correct C types.  The ARM compiler handles
 * register assignment (R0-R3, VFP D0-D7, stack) automatically.
 *
 * Naming: nuttx_invoke_<cookie>
 *   V = void, I = int/ptr, L = int64, F = float, D = double
 *   First char = return type, rest = arg types.
 */

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */

#define FIDX(x) (x)

static gint64
get_long_arg (InterpMethodArguments *margs, int idx)
{
	interp_pair p;
	p.pair.lo = (gint32)(gssize)margs->iargs [idx];
	p.pair.hi = (gint32)(gssize)margs->iargs [idx + 1];
	return p.l;
}

/* ------------------------------------------------------------------ */
/*  void return, integer-only args                                    */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_v (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(void);
	T func = (T)target_func;
	func ();
}

static void
nuttx_invoke_vi (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0]);
}

static void
nuttx_invoke_vii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1]);
}

static void
nuttx_invoke_viii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2]);
}

static void
nuttx_invoke_viiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3]);
}

static void
nuttx_invoke_viiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4]);
}

static void
nuttx_invoke_viiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4], (int)(gssize)margs->iargs [5]);
}

static void
nuttx_invoke_viiiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int, int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4], (int)(gssize)margs->iargs [5], (int)(gssize)margs->iargs [6]);
}

static void
nuttx_invoke_viiiiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int, int, int, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4], (int)(gssize)margs->iargs [5], (int)(gssize)margs->iargs [6], (int)(gssize)margs->iargs [7]);
}

/* ------------------------------------------------------------------ */
/*  int/ptr return, integer-only args                                 */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_i (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(void);
	T func = (T)target_func;
	int res = func ();
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_ii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iiiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int, int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4], (int)(gssize)margs->iargs [5]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iiiiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int, int, int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4], (int)(gssize)margs->iargs [5], (int)(gssize)margs->iargs [6]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iiiiiiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int, int, int, int, int);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2], (int)(gssize)margs->iargs [3], (int)(gssize)margs->iargs [4], (int)(gssize)margs->iargs [5], (int)(gssize)margs->iargs [6], (int)(gssize)margs->iargs [7]);
	*(int*)margs->retval = res;
}

/* ------------------------------------------------------------------ */
/*  int64 (long) return                                               */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_l (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(void);
	T func = (T)target_func;
	gint64 res = func ();
	*(gint64*)margs->retval = res;
}

static void
nuttx_invoke_li (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(int);
	T func = (T)target_func;
	gint64 res = func ((int)(gssize)margs->iargs [0]);
	*(gint64*)margs->retval = res;
}

static void
nuttx_invoke_lii (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(int, int);
	T func = (T)target_func;
	gint64 res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1]);
	*(gint64*)margs->retval = res;
}

static void
nuttx_invoke_liii (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(int, int, int);
	T func = (T)target_func;
	gint64 res = func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], (int)(gssize)margs->iargs [2]);
	*(gint64*)margs->retval = res;
}

/* ------------------------------------------------------------------ */
/*  void return, mixed int + int64 args                               */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_vil (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, gint64);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1));
}

static void
nuttx_invoke_vl (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(gint64);
	T func = (T)target_func;
	func (get_long_arg (margs, 0));
}

static void
nuttx_invoke_vli (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(gint64, int);
	T func = (T)target_func;
	func (get_long_arg (margs, 0), (int)(gssize)margs->iargs [2]);
}

static void
nuttx_invoke_vill (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, gint64, gint64);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1), get_long_arg (margs, 3));
}

static void
nuttx_invoke_villi (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, gint64, gint64, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1), get_long_arg (margs, 3), (int)(gssize)margs->iargs [5]);
}

/* ------------------------------------------------------------------ */
/*  int return, mixed int + int64 args                                */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_il (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(gint64);
	T func = (T)target_func;
	int res = func (get_long_arg (margs, 0));
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iil (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, gint64);
	T func = (T)target_func;
	int res = func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1));
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_ill (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(gint64, gint64);
	T func = (T)target_func;
	int res = func (get_long_arg (margs, 0), get_long_arg (margs, 2));
	*(int*)margs->retval = res;
}

/* ------------------------------------------------------------------ */
/*  int64 return, mixed int + int64 args                              */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_ll (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(gint64);
	T func = (T)target_func;
	gint64 res = func (get_long_arg (margs, 0));
	*(gint64*)margs->retval = res;
}

static void
nuttx_invoke_lil (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(int, gint64);
	T func = (T)target_func;
	gint64 res = func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1));
	*(gint64*)margs->retval = res;
}

static void
nuttx_invoke_lill (void *target_func, InterpMethodArguments *margs)
{
	typedef gint64 (*T)(int, gint64, gint64);
	T func = (T)target_func;
	gint64 res = func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1), get_long_arg (margs, 3));
	*(gint64*)margs->retval = res;
}

static void
nuttx_invoke_lili (void *target_func, InterpMethodArguments *margs)
{
	/* e.g. lseek(int fd, off_t offset, int whence) -> off_t */
	typedef gint64 (*T)(int, gint64, int);
	T func = (T)target_func;
	/* ARM AAPCS: int in r0, int64 in r2:r3 (r1 padding), int in stack[0] */
	gint64 res = func ((int)(gssize)margs->iargs [0], get_long_arg (margs, 1), (int)(gssize)margs->iargs [3]);
	*(gint64*)margs->retval = res;
}

/* ------------------------------------------------------------------ */
/*  float return                                                      */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_f (void *target_func, InterpMethodArguments *margs)
{
	typedef float (*T)(void);
	T func = (T)target_func;
	float res = func ();
	*(float*)margs->retval = res;
}

static void
nuttx_invoke_fi (void *target_func, InterpMethodArguments *margs)
{
	typedef float (*T)(int);
	T func = (T)target_func;
	float res = func ((int)(gssize)margs->iargs [0]);
	*(float*)margs->retval = res;
}

static void
nuttx_invoke_fif (void *target_func, InterpMethodArguments *margs)
{
	typedef float (*T)(int, float);
	T func = (T)target_func;
	float res = func ((int)(gssize)margs->iargs [0], *(float*)&margs->fargs [FIDX (0)]);
	*(float*)margs->retval = res;
}

static void
nuttx_invoke_fiff (void *target_func, InterpMethodArguments *margs)
{
	typedef float (*T)(int, float, float);
	T func = (T)target_func;
	float res = func ((int)(gssize)margs->iargs [0], *(float*)&margs->fargs [FIDX (0)], *(float*)&margs->fargs [FIDX (1)]);
	*(float*)margs->retval = res;
}

/* ------------------------------------------------------------------ */
/*  double return                                                     */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_d (void *target_func, InterpMethodArguments *margs)
{
	typedef double (*T)(void);
	T func = (T)target_func;
	double res = func ();
	*(double*)margs->retval = res;
}

static void
nuttx_invoke_di (void *target_func, InterpMethodArguments *margs)
{
	typedef double (*T)(int);
	T func = (T)target_func;
	double res = func ((int)(gssize)margs->iargs [0]);
	*(double*)margs->retval = res;
}

static void
nuttx_invoke_did (void *target_func, InterpMethodArguments *margs)
{
	typedef double (*T)(int, double);
	T func = (T)target_func;
	double res = func ((int)(gssize)margs->iargs [0], margs->fargs [FIDX (0)]);
	*(double*)margs->retval = res;
}

static void
nuttx_invoke_didd (void *target_func, InterpMethodArguments *margs)
{
	typedef double (*T)(int, double, double);
	T func = (T)target_func;
	double res = func ((int)(gssize)margs->iargs [0], margs->fargs [FIDX (0)], margs->fargs [FIDX (1)]);
	*(double*)margs->retval = res;
}

/* ------------------------------------------------------------------ */
/*  void return, float/double args                                    */
/* ------------------------------------------------------------------ */

static void
nuttx_invoke_vif (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, float);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], *(float*)&margs->fargs [FIDX (0)]);
}

static void
nuttx_invoke_vid (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, double);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], margs->fargs [FIDX (0)]);
}

static void
nuttx_invoke_viif (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, float);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], *(float*)&margs->fargs [FIDX (0)]);
}

static void
nuttx_invoke_viid (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, double);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], margs->fargs [FIDX (0)]);
}

static void
nuttx_invoke_viiffi (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, float, float, int);
	T func = (T)target_func;
	func ((int)(gssize)margs->iargs [0], (int)(gssize)margs->iargs [1], *(float*)&margs->fargs [FIDX (0)], *(float*)&margs->fargs [FIDX (1)], (int)(gssize)margs->iargs [2]);
}

/* ------------------------------------------------------------------ */
/*  8-byte struct ('S') wrappers                                      */
/* ------------------------------------------------------------------ */
/* 'S' = 8-byte value type passed by pointer (PINVOKE_ARG_VTYPE).     */
/* The trampoline dereferences the pointer and passes the two 32-bit  */
/* halves as separate int args, matching ARM AAPCS struct-by-value.   */

#define S_LO(m, i) (((gint32*)(m)->iargs[i])[0])
#define S_HI(m, i) (((gint32*)(m)->iargs[i])[1])

/* void return, struct args */

static void
nuttx_invoke_vs (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int);
	((T)target_func)(S_LO(margs,0), S_HI(margs,0));
}

static void
nuttx_invoke_vsi (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int);
	((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1]);
}

static void
nuttx_invoke_vsii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int);
	((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1], (int)(gssize)margs->iargs[2]);
}

static void
nuttx_invoke_vsiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int);
	((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1], (int)(gssize)margs->iargs[2], (int)(gssize)margs->iargs[3]);
}

static void
nuttx_invoke_vsiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int, int);
	((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1], (int)(gssize)margs->iargs[2], (int)(gssize)margs->iargs[3], (int)(gssize)margs->iargs[4]);
}

static void
nuttx_invoke_visi (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int);
	((T)target_func)((int)(gssize)margs->iargs[0], S_LO(margs,1), S_HI(margs,1), (int)(gssize)margs->iargs[2]);
}

static void
nuttx_invoke_vssii (void *target_func, InterpMethodArguments *margs)
{
	typedef void (*T)(int, int, int, int, int, int);
	((T)target_func)(S_LO(margs,0), S_HI(margs,0), S_LO(margs,1), S_HI(margs,1), (int)(gssize)margs->iargs[2], (int)(gssize)margs->iargs[3]);
}

/* int/ptr return, struct args */

static void
nuttx_invoke_is (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int);
	int res = ((T)target_func)(S_LO(margs,0), S_HI(margs,0));
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_isi (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int);
	int res = ((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_isii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int);
	int res = ((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1], (int)(gssize)margs->iargs[2]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_isiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int, int);
	int res = ((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1], (int)(gssize)margs->iargs[2], (int)(gssize)margs->iargs[3]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_isiiii (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int, int, int);
	int res = ((T)target_func)(S_LO(margs,0), S_HI(margs,0), (int)(gssize)margs->iargs[1], (int)(gssize)margs->iargs[2], (int)(gssize)margs->iargs[3], (int)(gssize)margs->iargs[4]);
	*(int*)margs->retval = res;
}

static void
nuttx_invoke_iss (void *target_func, InterpMethodArguments *margs)
{
	typedef int (*T)(int, int, int, int);
	int res = ((T)target_func)(S_LO(margs,0), S_HI(margs,0), S_LO(margs,1), S_HI(margs,1));
	*(int*)margs->retval = res;
}

#undef S_LO
#undef S_HI

/* ------------------------------------------------------------------ */
/*  Cookie-based dispatch table                                       */
/* ------------------------------------------------------------------ */

typedef void (*NuttxInvokeFunc)(void *target_func, InterpMethodArguments *margs);

typedef struct {
	const char *cookie;
	NuttxInvokeFunc func;
} NuttxInvokeEntry;

static const NuttxInvokeEntry nuttx_invoke_table [] = {
	/* void return */
	{ "V",         nuttx_invoke_v },
	{ "VI",        nuttx_invoke_vi },
	{ "VII",       nuttx_invoke_vii },
	{ "VIII",      nuttx_invoke_viii },
	{ "VIIII",     nuttx_invoke_viiii },
	{ "VIIIII",    nuttx_invoke_viiiii },
	{ "VIIIIII",   nuttx_invoke_viiiiii },
	{ "VIIIIIII",  nuttx_invoke_viiiiiii },
	{ "VIIIIIIII", nuttx_invoke_viiiiiiii },
	{ "VL",        nuttx_invoke_vl },
	{ "VIL",       nuttx_invoke_vil },
	{ "VLI",       nuttx_invoke_vli },
	{ "VILL",      nuttx_invoke_vill },
	{ "VILLI",     nuttx_invoke_villi },
	{ "VIF",       nuttx_invoke_vif },
	{ "VID",       nuttx_invoke_vid },
	{ "VIIF",      nuttx_invoke_viif },
	{ "VIID",      nuttx_invoke_viid },
	{ "VIIFFI",    nuttx_invoke_viiffi },
	/* void return, 8-byte struct args */
	{ "VS",        nuttx_invoke_vs },
	{ "VSI",       nuttx_invoke_vsi },
	{ "VSII",      nuttx_invoke_vsii },
	{ "VSIII",     nuttx_invoke_vsiii },
	{ "VSIIII",    nuttx_invoke_vsiiii },
	{ "VISI",      nuttx_invoke_visi },
	{ "VSSII",     nuttx_invoke_vssii },
	/* int/ptr return */
	{ "I",         nuttx_invoke_i },
	{ "II",        nuttx_invoke_ii },
	{ "III",       nuttx_invoke_iii },
	{ "IIII",      nuttx_invoke_iiii },
	{ "IIIII",     nuttx_invoke_iiiii },
	{ "IIIIII",    nuttx_invoke_iiiiii },
	{ "IIIIIII",   nuttx_invoke_iiiiiii },
	{ "IIIIIIII",  nuttx_invoke_iiiiiiii },
	{ "IIIIIIIII", nuttx_invoke_iiiiiiiii },
	{ "IL",        nuttx_invoke_il },
	{ "IIL",       nuttx_invoke_iil },
	{ "ILL",       nuttx_invoke_ill },
	/* int/ptr return, 8-byte struct args */
	{ "IS",        nuttx_invoke_is },
	{ "ISI",       nuttx_invoke_isi },
	{ "ISII",      nuttx_invoke_isii },
	{ "ISIII",     nuttx_invoke_isiii },
	{ "ISIIII",    nuttx_invoke_isiiii },
	{ "ISS",       nuttx_invoke_iss },
	/* int64 return */
	{ "L",         nuttx_invoke_l },
	{ "LI",        nuttx_invoke_li },
	{ "LII",       nuttx_invoke_lii },
	{ "LIII",      nuttx_invoke_liii },
	{ "LL",        nuttx_invoke_ll },
	{ "LIL",       nuttx_invoke_lil },
	{ "LILI",      nuttx_invoke_lili },
	{ "LILL",      nuttx_invoke_lill },
	/* float return */
	{ "F",         nuttx_invoke_f },
	{ "FI",        nuttx_invoke_fi },
	{ "FIF",       nuttx_invoke_fif },
	{ "FIFF",      nuttx_invoke_fiff },
	/* double return */
	{ "D",         nuttx_invoke_d },
	{ "DI",        nuttx_invoke_di },
	{ "DID",       nuttx_invoke_did },
	{ "DIDD",      nuttx_invoke_didd },
};

static const int nuttx_invoke_table_size = sizeof (nuttx_invoke_table) / sizeof (nuttx_invoke_table [0]);
