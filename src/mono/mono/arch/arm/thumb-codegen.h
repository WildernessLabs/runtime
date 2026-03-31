#ifndef __THUMB_CODEGEN_H
# define __THUMB_CODEGEN_H
# include <stdlib.h>
# include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __THUMB_INLINE__ __inline__ // __attribute__((always_inline))

typedef unsigned int arminstr_t;
typedef unsigned int armword_t;

typedef enum {
	ARMREG_R0 = 0,
	ARMREG_R1,
	ARMREG_R2,
	ARMREG_R3,
	ARMREG_R4,
	ARMREG_R5,
	ARMREG_R6,
	ARMREG_R7,
	ARMREG_R8,
	ARMREG_R9,
	ARMREG_R10,
	ARMREG_R11,
	ARMREG_R12,
	ARMREG_R13,
	ARMREG_R14,
	ARMREG_R15,


	/* aliases */
	/* args */
	ARMREG_A1 = ARMREG_R0,
	ARMREG_A2 = ARMREG_R1,
	ARMREG_A3 = ARMREG_R2,
	ARMREG_A4 = ARMREG_R3,

	/* local vars */
	ARMREG_V1 = ARMREG_R4,
	ARMREG_V2 = ARMREG_R5,
	ARMREG_V3 = ARMREG_R6,
	ARMREG_V4 = ARMREG_R7,
	ARMREG_V5 = ARMREG_R8,
	ARMREG_V6 = ARMREG_R9,
	ARMREG_V7 = ARMREG_R10,

	ARMREG_FP = ARMREG_R11,
	ARMREG_IP = ARMREG_R12,
	ARMREG_SP = ARMREG_R13,
	ARMREG_LR = ARMREG_R14,
	ARMREG_PC = ARMREG_R15,

	/* co-processor */
	ARMREG_CR0 = 0,
	ARMREG_CR1,
	ARMREG_CR2,
	ARMREG_CR3,
	ARMREG_CR4,
	ARMREG_CR5,
	ARMREG_CR6,
	ARMREG_CR7,
	ARMREG_CR8,
	ARMREG_CR9,
	ARMREG_CR10,
	ARMREG_CR11,
	ARMREG_CR12,
	ARMREG_CR13,
	ARMREG_CR14,
	ARMREG_CR15,

	/* XScale: acc0 on CP0 */
	ARMREG_ACC0 = ARMREG_CR0,

	ARMREG_MAX = ARMREG_R15
} ARMReg;

/* number of argument registers */
#define ARM_NUM_ARG_REGS 4

/* bitvector for all argument regs (A1-A4) */
#define ARM_ALL_ARG_REGS \
	(1 << ARMREG_A1) | (1 << ARMREG_A2) | (1 << ARMREG_A3) | (1 << ARMREG_A4)

/* 
 * Displacement to use for sequences like:
 *  ARM_LDR (code, reg, ARMREG_PC, displacement)
 *  ARM_B (code, n)
 */
#define ARMDISP_LDRPC	4

/* 
 * Displacement to use for OP_SWITCH:
 *  ARM_ADR (code ARMREG_IP, displacement)
 *  ARM_LDR (code, ARMREG_IP, ARMREG_IP, r, lsl, 2)
 *  ARM_BX (code, ARMREG_IP)
 *  ARM_NOPS (code)
 */
#define ARMDISP_SWITCH	16

/*
 * IMT offset use by mono_arch_find_imt_method
 */
#define ARMOFF_IMT	5

typedef enum {
	ARMCOND_EQ = 0x0,          /* Equal; Z = 1 */
	ARMCOND_NE = 0x1,          /* Not equal, or unordered; Z = 0 */
	ARMCOND_CS = 0x2,          /* Carry set; C = 1 */
	ARMCOND_HS = ARMCOND_CS,   /* Unsigned higher or same; */
	ARMCOND_CC = 0x3,          /* Carry clear; C = 0 */
	ARMCOND_LO = ARMCOND_CC,   /* Unsigned lower */
	ARMCOND_MI = 0x4,          /* Negative; N = 1 */
	ARMCOND_PL = 0x5,          /* Positive or zero; N = 0 */
	ARMCOND_VS = 0x6,          /* Overflow; V = 1 */
	ARMCOND_VC = 0x7,          /* No overflow; V = 0 */
	ARMCOND_HI = 0x8,          /* Unsigned higher; C = 1 && Z = 0 */
	ARMCOND_LS = 0x9,          /* Unsigned lower or same; C = 0 || Z = 1 */
	ARMCOND_GE = 0xA,          /* Signed greater than or equal; N = V */
	ARMCOND_LT = 0xB,          /* Signed less than; N != V */
	ARMCOND_GT = 0xC,          /* Signed greater than; Z = 0 && N = V */
	ARMCOND_LE = 0xD,          /* Signed less than or equal; Z = 1 && N != V */
	ARMCOND_AL = 0xE,          /* Always */
	ARMCOND_NV = 0xF,          /* Never */

	ARMCOND_SHIFT = 28
} ARMCond;

const static int reverseCC[16] = { ARMCOND_NE, ARMCOND_EQ, ARMCOND_CC, ARMCOND_CS, 
		 		   ARMCOND_PL, ARMCOND_MI, ARMCOND_VC, ARMCOND_VS, 
		 		   ARMCOND_LS, ARMCOND_HI, ARMCOND_LT, ARMCOND_GE, 
		 		   ARMCOND_LE, ARMCOND_GT, ARMCOND_NV, ARMCOND_AL };

#define ARM_IS_IMM12(v) ((v) > -4096 && (v) < 4096)
#define ARM_IS_IMM8(v) ((v) > -256 && (v) < 256)

#define ROL(val, rot) (val << (rot & 0x1f) | val >> ((32 - rot) & 0x1f))

enum {
	/* VFP registers */
	ARM_VFP_F0,
	ARM_VFP_F1,
	ARM_VFP_F2,
	ARM_VFP_F3,
	ARM_VFP_F4,
	ARM_VFP_F5,
	ARM_VFP_F6,
	ARM_VFP_F7,
	ARM_VFP_F8,
	ARM_VFP_F9,
	ARM_VFP_F10,
	ARM_VFP_F11,
	ARM_VFP_F12,
	ARM_VFP_F13,
	ARM_VFP_F14,
	ARM_VFP_F15,
	ARM_VFP_F16,
	ARM_VFP_F17,
	ARM_VFP_F18,
	ARM_VFP_F19,
	ARM_VFP_F20,
	ARM_VFP_F21,
	ARM_VFP_F22,
	ARM_VFP_F23,
	ARM_VFP_F24,
	ARM_VFP_F25,
	ARM_VFP_F26,
	ARM_VFP_F27,
	ARM_VFP_F28,
	ARM_VFP_F29,
	ARM_VFP_F30,
	ARM_VFP_F31,

	ARM_VFP_D0 = ARM_VFP_F0,
	ARM_VFP_D1 = ARM_VFP_F2,
	ARM_VFP_D2 = ARM_VFP_F4,
	ARM_VFP_D3 = ARM_VFP_F6,
	ARM_VFP_D4 = ARM_VFP_F8,
	ARM_VFP_D5 = ARM_VFP_F10,
	ARM_VFP_D6 = ARM_VFP_F12,
	ARM_VFP_D7 = ARM_VFP_F14,
	ARM_VFP_D8 = ARM_VFP_F16,
	ARM_VFP_D9 = ARM_VFP_F18,
	ARM_VFP_D10 = ARM_VFP_F20,
	ARM_VFP_D11 = ARM_VFP_F22,
	ARM_VFP_D12 = ARM_VFP_F24,
	ARM_VFP_D13 = ARM_VFP_F26,
	ARM_VFP_D14 = ARM_VFP_F28,
	ARM_VFP_D15 = ARM_VFP_F30,

	ARM_VFP_COPROC_SINGLE = 10,
	ARM_VFP_COPROC_DOUBLE = 11,
};
 
typedef enum {
	ARM_DMB_ISH = 0xb,
	ARM_DMB_SY = 0xf,
} ArmDmbFlags;

static __THUMB_INLINE__ int
arm_imm12(int imm)
{
	uint32_t m, lz, rol, ror;
	int val;
	uint8_t *byte = (uint8_t *) &imm;

	if (imm == -1)
		return(1023);

	if ((imm >= 0) && (imm < 256))
		return(imm);

	/*
	 * Thumb2 ThumbExpandImm byte-repeat patterns (bits[11:10] == 00):
	 *   bits[9:8] == 01 → 0x00XY00XY  (byte[1]==0, byte[3]==0, byte[0]==byte[2])
	 *   bits[9:8] == 10 → 0xXY00XY00  (byte[0]==0, byte[2]==0, byte[1]==byte[3])
	 *   bits[9:8] == 11 → 0xXYXYXYXY  (all bytes equal)
	 */
	if ((byte[1] == 0) && (byte[3] == 0) && (byte[0] == byte[2]))
		val = 0x100 | byte[0];
	else if ((byte[0] == 0) && (byte[2] == 0) && (byte[1] == byte[3]))
		val = 0x200 | byte[1];
	else if ((byte[0] == byte[1]) && (byte[0] == byte[2]) && (byte[0] == byte[3]))
		val = 0x300 | byte[0];
	else {
		lz = __builtin_clz(imm);
		rol = 24 - lz;
		ror = 32 - rol;
		m = (uint32_t)imm >> rol;
		val = ((ror << 7) & 0xf80) | (m & 0x7f);
	}
	return(val);
}

/**
 * 16-bit Thumb Instructions
 */

/**
 * Shift by immediate, move register
 */
typedef struct {
	uint16_t rd:3;		/* Destination register */
	uint16_t rm:3;		/* Source regiser */
	uint16_t imm:5;		/* Shift amount */
	uint16_t op:2;		/* Operation code */
	uint16_t type:3;	/* '000' */
} simr16_t;

#define OP_SIMR16	0x0000	/* Initial value for simr_t */

static __THUMB_INLINE__ void
arm_simr16(void **code, int rd, int rm, int imm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	simr16_t *op = *(simr16_t **) code;
	*p = OP_SIMR16;
	op->rd = rd;
	op->rm = rm;
	op->imm = imm;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/**
 * Add/Subtract register
 */
typedef struct {
	uint16_t rd:3;		/* Destination register */
	uint16_t rn:3;		/* Register n */
	uint16_t rm:3;		/* Register m */
	uint16_t op:1;		/* Operation code */
	uint16_t type:6;	/* '000110' */
} asr16_t;

#define OP_ASR16	0x1800	/* Initial value for asr_t */

static __THUMB_INLINE__ void
arm_asr16(void **code, int rd, int rn, int rm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	asr16_t *op = *(asr16_t **) code;
	*p = OP_ASR16;
	op->rd = rd;
	op->rn = rn;
	op->rm = rm;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/**
 * Add/Subtract immediate
 */
typedef struct {
	uint16_t rd:3;		/* Destination register */
	uint16_t rn:3;		/* Register n */
	uint16_t imm:3;		/* Immediate value */
	uint16_t op:1;		/* Operation code */
	uint16_t type:6;	/* '000111' */
} asi16_t;

#define OP_ASI16	0x1c00	/* Initial value for asi16_t */

static __THUMB_INLINE__ void
arm_asi16(void **code, int rd, int rn, int imm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	asi16_t *op = *(asi16_t **) code;
	*p = OP_ASI16;
	op->rd = rd;
	op->rn = rn;
	op->imm = imm;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/**
 * Add/Subtract/Compare/Move immediate
 */
typedef struct {
	uint16_t imm:8;		/* Immediate value */
	uint16_t rd:3;		/* Destination register */
	uint16_t op:5;		/* Operation code */
} ascmi16_t;

#define OP_ASCMI16	0x0000	/* Initial value for ascmi16_t */

static __THUMB_INLINE__ void
arm_ascmi16(void **code, int rd, int imm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	ascmi16_t *op = *(ascmi16_t **) code;
	*p = OP_ASCMI16;
	op->imm = imm;
	op->rd = rd;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/**
 * Data processing register
 */
typedef struct {
	uint16_t rd:3;		/* Destination register */
	uint16_t rn:3;		/* Register n */
	uint16_t op:4;		/* Operation code */
	uint16_t type:6;	/* '010000' */
} dpr16_t;

#define OP_DPR16	0x4000	/* Initial value for dpr16_t */

static __THUMB_INLINE__ void
arm_dpri16(void **code, int rd, int rn, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	dpr16_t *op = *(dpr16_t **) code;
	*p = OP_DPR16;
	op->rd = rd;
	op->rn = rn;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/**
 * Special Data Processing
 */
typedef struct {
	uint16_t rd:3;		/* Register d */
	uint16_t rm:4;		/* Register n */
	uint16_t dn:1;		/* Modifier */
	uint16_t op:2;		/* Operation code */
	uint16_t type:6;	/* '010001' */
} sdp16_t;

#define OP_SDP16	0x4400	/* Initial value for sdp16_t */

static __THUMB_INLINE__ void
arm_sdp16(void **code, int rd, int rm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	sdp16_t *op = *(sdp16_t **) code;
	*p = OP_SDP16;
	op->rd = rd;
	op->rm = rm;
	op->dn = (rd >> 3);
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Branch/Exchange Instruction Set
 */
typedef struct {
	uint16_t typea:3;	/* '000'b */
	uint16_t rm:4;		/* Register m */
	uint16_t link:1;	/* Link selector */
	uint16_t type:8;	/* '01000111' */
} bxis16_t;

#define OP_BXIS16	0x4780	/* Initial value for bxis16_t */

static __THUMB_INLINE__ void
arm_bxis16(void **code, int rm, int link)
{
	uint16_t *p = *(uint16_t **) code;
	bxis16_t *op = *(bxis16_t **) code;
	*p = OP_BXIS16;
	op->rm = rm;
	op->link = link;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load from literal pool
 */
typedef struct {
	uint16_t imm:8;		/* PC relative */
	uint16_t rd:3;		/* Register n */
	uint16_t type:5;	/* '01001' */
} llp16_t;

#define OP_LLP16	0x4800	/* Initial value for llp16_t */

static __THUMB_INLINE__ void
arm_llp16(void **code, int imm, int rd)
{
	uint16_t *p = *(uint16_t **) code;
	llp16_t *op = *(llp16_t **) code;
	*p = OP_LLP16;
	op->imm = imm;
	op->rd = rd;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store register offset
 */
typedef struct {
	uint16_t rd:3;		/* Register d */
	uint16_t rn:3;		/* Register n */
	uint16_t rm:3;		/* Register m */
	uint16_t op:3;		/* Operation code */
	uint16_t type:4;	/* '0101' */
} lsro16_t;

#define OP_LSRO16	0x5000	/* Initial value for lsro16_t */

static __THUMB_INLINE__ void
arm_lsro16(void **code, int rd, int rn, int rm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	lsro16_t *op = *(lsro16_t **) code;
	*p = OP_LSRO16;
	op->rd = rd;
	op->rn = rn;
	op->rm = rm;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/store word/byte immediate offset
 */
typedef struct {
	uint16_t rd:3;		/* Register d */
	uint16_t rn:3;		/* Register n */
	uint16_t imm:5;		/* Immediate offset */
	uint16_t l:1;		/* Load/store selector */
	uint16_t b:1;		/* Byte/word selector */
	uint16_t type:3;	/* '011' */
} lswbi16_t;

#define OP_LSWBI16	0x6000	/* Initial value for lswbi16_t */

static __THUMB_INLINE__ void
arm_lswbi16(void **code, int rd, int rn, int imm, int l, int b)
{
	uint16_t *p = *(uint16_t **) code;
	lswbi16_t *op = *(lswbi16_t **) code;
	*p = OP_LSWBI16;
	op->rd = rd;
	op->rn = rn;
	op->imm = imm;
	op->l = l;
	op->b = b;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/store halfword immediate offset
 */
typedef struct {
	uint16_t rd:3;		/* Register d */
	uint16_t rn:3;		/* Register n */
	uint16_t imm:5;		/* Immediate offset */
	uint16_t l:1;		/* Load/store selector */
	uint16_t type:4;	/* '1000' */
} lshi16_t;

#define OP_LSHI16	0x8000	/* Initial value for lshi16_t */

static __THUMB_INLINE__ void
arm_lshi16(void **code, int rd, int rn, int imm, int l)
{
	uint16_t *p = *(uint16_t **) code;
	lshi16_t *op = *(lshi16_t **) code;
	*p = OP_LSHI16;
	op->rd = rd;
	op->rn = rn;
	op->imm = imm;
	op->l = l;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/store to/from stack 
 */
typedef struct {
	uint16_t imm:8;		/* SP-relative immediate */
	uint16_t rd:3;		/* Register d */
	uint16_t l:1;		/* Load/store selector */
	uint16_t type:4;	/* '1001' */
} lss16_t;

#define OP_LSS16	0x9000	/* Initial value for lss16_t */

static __THUMB_INLINE__ void
arm_lss16(void **code, int imm, int rd, int l)
{
	uint16_t *p = *(uint16_t **) code;
	lss16_t *op = *(lss16_t **) code;
	*p = OP_LSS16;
	op->imm = imm;
	op->rd = rd;
	op->l = l;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Add to SP or PC
 */
typedef struct {
	uint16_t imm:8;		/* SP-relative immediate */
	uint16_t rd:3;		/* Register d */
	uint16_t sp:1;		/* SP/PC selector */
	uint16_t type:4;	/* '1010' */
} asp16_t;

#define OP_ASP16	0xa000	/* Initial value for asp16_t */

static __THUMB_INLINE__ void
arm_asp16(void **code, int rd, int imm, int sp)
{
	uint16_t *p = *(uint16_t **) code;
	asp16_t *op = *(asp16_t **) code;
	*p = OP_ASP16;
	op->imm = imm;
	op->rd = rd;
	op->sp = sp;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store multiple
 */
typedef struct {
	uint16_t rl:8;		/* Register list */
	uint16_t rn:3;		/* Register n */
	uint16_t l:1;		/* Load/store selector */
	uint16_t type:4;	/* '1100' */
} lsm16_t;

#define OP_LSM16	0xc000	/* Initial value for lsm16_t */

static __THUMB_INLINE__ void
arm_lsm16(void **code, int rn, int rl, int l)
{
	uint16_t *p = *(uint16_t **) code;
	lsm16_t *op = *(lsm16_t **) code;
	*p = OP_LSM16;
	op->rl = rl;
	op->rn = rn;
	op->l = l;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Conditional branch
 */
typedef struct {
	uint16_t imm:8;		/* PC-relative immediate */
	uint16_t cond:4;	/* Condition code*/
	uint16_t type:4;	/* '1101' */
} cbr16_t;

#define OP_CBR16	0xd000	/* Initial value for cbr16_t */

static __THUMB_INLINE__ void
arm_cbr16(void **code, int cond, int offset)
{
	uint16_t *p = *(uint16_t **) code;
	cbr16_t *op = *(cbr16_t **) code;
	*p = OP_CBR16;
	op->imm = (offset >> 1) - 2;
	op->cond = cond;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * System Call
 */
typedef struct {
	uint16_t imm:8;		/* Call code */
	uint16_t type:8;	/* '11011111' */
} svc16_t;

#define OP_SVC16	0xdf00	/* Initial value for svc16_t */

static __THUMB_INLINE__ void
arm_svc16(void **code, int imm)
{
	uint16_t *p = *(uint16_t **) code;
	svc16_t *op = *(svc16_t **) code;
	*p = OP_SVC16;
	op->imm = imm;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Unconditional branch
 */
typedef struct {
	uint16_t imm:11;	/* PC-relative immediate */
	uint16_t type:5;	/* '11100' */
} ub16_t;

#define OP_UB16		0xe000	/* Initial value for ub16_t */

static __THUMB_INLINE__ void
arm_ub16(void **code, int imm)
{
	uint16_t *p = *(uint16_t **) code;
	ub16_t *op = *(ub16_t **) code;
	*p = OP_UB16;
	op->imm = imm;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaneous - Adjust stack pointer
 */
typedef struct {
	uint16_t imm:7;		/* Immediate value */
	uint16_t op:1;		/* Opcode */
	uint16_t type:8;	/* '10110000'b */
} ajsp16_t;

#define OP_AJSP16	0xb000

static __THUMB_INLINE__ void
arm_ajsp16(void **code, int imm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	ajsp16_t *op = *(ajsp16_t **) code;
	*p = OP_AJSP16;
	op->imm = imm;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaneous - Sign/zero extend
 */
typedef struct {
	uint16_t rd:3;		/* Register d */
	uint16_t rm:3;		/* Register m */
	uint16_t op:2;		/* Opcode */
	uint16_t type:8;	/* '10110010'b */
} szx16_t;

#define OP_SZX16	0xb200

static __THUMB_INLINE__ void
arm_szx16(void **code, int rd, int rm, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	szx16_t *op = *(szx16_t **) code;
	*p = OP_SZX16;
	op->rd = rd;
	op->rm = rm;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - Compare and Branch on (Non-)Zero
 */
typedef struct {
	uint16_t rn:3;		/* Register n */
	uint16_t imm:5;		/* Bits 0-4 of immediate value */
	uint16_t typea:1;	/* '1'b */
	uint16_t imm1:1;	/* Bit 5 of immediate value */
	uint16_t typeb:1;	/* '0'b */
	uint16_t n:1;		/* Zero/non-zero */
	uint16_t type:4;	/* '1011'b */
} cbnz16_t;

#define OP_CBNZ16	0xb100

static __THUMB_INLINE__ void
arm_cbnz16(void **code, int rn, int imm, int n)
{
	uint16_t *p = *(uint16_t **) code;
	cbnz16_t *op = *(cbnz16_t **) code;
	*p = OP_CBNZ16;
	op->rn = rn;
	op->imm = imm;
	op->imm1 = (imm >> 4);
	op->n = n;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - Push/pop register list
 */
typedef struct {
	uint16_t rl:8;		/* Register list */
	uint16_t p:1;		/* Register */
	uint16_t typea:2;	/* '01'b */
	uint16_t op:1;		/* Push/pop indicator */
	uint16_t type:4;	/* '1011'b */
} pp16_t;

#define OP_PP16		0xb400

static __THUMB_INLINE__ void
arm_pp16(void **code, int rl, int p, int opcode)
{
	uint16_t *x = *(uint16_t **) code;
	pp16_t *op = *(pp16_t **) code;
	*x = OP_PP16;
	op->rl = rl;
	op->p = p;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - Set endianness
 */
typedef struct {
	uint16_t typea:3;	/* '000' */
	uint16_t e:1;		/* Endianess selector */
	uint16_t type:12;	/* '101101100101'b */
} send16_t;

#define OP_SEND16	0xb650

static __THUMB_INLINE__ void
arm_send16(void **code, int endianess)
{
	uint16_t *p = *(uint16_t **) code;
	send16_t *op = *(send16_t **) code;
	*p = OP_SEND16;
	op->e = endianess;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - Change Processor State
 */
typedef struct {
	uint16_t f:1;		/* Affect F interrupt bits */
	uint16_t i:1;		/* Affect I interrupt bits */
	uint16_t a:1;		/* Affect A interrupt bits */
	uint16_t typea:1;	/* '0'b */
	uint16_t op:1;		/* Enable/disable */
	uint16_t type:11;	/* '10110110011'b */
} cps16_t;

#define OP_CPS16	0xb660

static __THUMB_INLINE__ void
arm_cps16(void **code, int a, int i, int f, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	cps16_t *op = *(cps16_t **) code;
	*p = OP_CPS16;
	op->a = a;
	op->i = i;
	op->f = f;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - Reverse bytes
 */
typedef struct {
	uint16_t rd:3;		/* Register d */
	uint16_t rn:3;		/* Register n */
	uint16_t op:2;		/* Op-code */
	uint16_t type:8;	/* '10111010'b */
} rev16_t;

#define OP_REV16	0xba00

static __THUMB_INLINE__ void
arm_rev16(void **code, int rd, int rn, int opcode)
{
	uint16_t *p = *(uint16_t **) code;
	rev16_t *op = *(rev16_t **) code;
	*p = OP_REV16;
	op->rd = rd;
	op->rn = rn;
	op->op = opcode;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - Software Breakpoint
 */
typedef struct {
	uint16_t imm:8;		/* Immediate value */
	uint16_t type:8;	/* '10111110'b */
} sbp16_t;

#define OP_SBP16	0xbe00

static __THUMB_INLINE__ void
arm_sbp16(void **code, int imm)
{
	uint16_t *p = *(uint16_t **) code;
	sbp16_t *op = *(sbp16_t **) code;
	*p = OP_SBP16;
	op->imm = imm;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - If-then
 */
typedef struct {
	uint16_t mask:4;	/* Mask */
	uint16_t cond:4;	/* Condition */
	uint16_t type:8;	/* '10111111'b */
} ift16_t;

#define OP_IFT16	0xbf00

static __THUMB_INLINE__ void
arm_ift16(void **code, int cond, int mask)
{
	uint16_t *p = *(uint16_t **) code;
	ift16_t *op = *(ift16_t **) code;
	*p = OP_IFT16;
	op->mask = mask;
	op->cond = cond;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Miscellaenous - NOP-compatible hints
 */
typedef struct {
	uint16_t typea:4;	/* '0000'b */
	uint16_t hint:4;	/* Hint */
	uint16_t type:8;	/* '10111111'b */
} nop16_t;

#define OP_NOP16	0xbf00

static __THUMB_INLINE__ void
arm_nop16(void **code, int hint)
{
	uint16_t *p = *(uint16_t **) code;
	nop16_t *op = *(nop16_t **) code;
	*p = OP_NOP16;
	op->hint = hint;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * 32-bit instructions
 */

/**
 * Data processing, modified 12-bit immediate
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t cond:1;	/* Set condition code */
	uint32_t op:4;		/* op code */
	uint32_t typeb:1;	/* '0'b' */
	uint32_t imm1:1;	/* Bit 11 of immediate */
	uint32_t type:5;	/* '11110'b */
	uint32_t imm8:8;	/* Bits 0-7 of immediate */
	uint32_t rd:4;		/* Register d */
	uint32_t imm3:3;	/* Bits 8-10 of immediate */
	uint32_t typec:1;	/* '0'b */
} dpm32_t;

#define OP_DPM32	0x0000f000

static __THUMB_INLINE__ void
arm_dpm32(void **code, int rd, int rn, int imm, int cond, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	dpm32_t *op = *(dpm32_t **) code;
	*p = OP_DPM32;
	imm = arm_imm12(imm);
	op->rn = rn;
	op->rd = rd;
	op->cond = cond;
	op->op = opcode;
	op->imm1 = (imm >> 11);
	op->imm8 = imm;
	op->imm3 = (imm >> 8);
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
			
static __THUMB_INLINE__ void
arm_dpm32_rot(void **code, int rd, int rn, int imm, int rot, int cond, int opcode)
{
	/*
	 * The caller passes ARM-format rotated immediate (imm8, rot) where
	 * value = imm8 ROR rot.  Reconstruct the actual value and delegate
	 * to arm_dpm32 which uses arm_imm12() for correct Thumb2 encoding.
	 *
	 * The old code did `imm = (rot << 7) | imm` which placed the ARM
	 * rotation and mantissa directly into the Thumb2 imm12 field.  This
	 * is wrong because Thumb2 ThumbExpandImm adds an implicit leading 1
	 * bit to the mantissa in the rotation case (bits[11:10] != 00),
	 * producing a different value when imm8 bit 7 is clear.
	 */
	uint32_t actual = (rot == 0) ? (uint32_t)imm
		: (((uint32_t)imm >> rot) | ((uint32_t)imm << (32 - rot)));
	arm_dpm32(code, rd, rn, (int)actual, cond, opcode);
}
			
/**
 * Add, Subtract, plain 12-bit immediate
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t op2:2;  	/* Bottom 2 bits of op-code */
	uint32_t typea:1;	/* '0'b */
	uint32_t op:1;		/* Top bit of op-code */
	uint32_t typeb:2;	/* '10'b' */
	uint32_t imm1:1;	/* Bit 11 of immediate */
	uint32_t type:5;	/* '11110'b */
	uint32_t imm8:8;	/* Bits 0-7 of immediate */
	uint32_t rd:4;		/* Register d */
	uint32_t imm3:3;	/* Bits 8-10 of immediate */
	uint32_t typec:1;	/* '0'b */
} asp32_t;

#define OP_ASP32	0x0000f200

static __THUMB_INLINE__ void
arm_asp32(void **code, int rd, int rn, int imm, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	asp32_t *op = *(asp32_t **) code;
	*p = OP_ASP32;
	imm = arm_imm12(imm);
	op->rn = rn;
	op->rd = rd;
	op->op2 = opcode;
	op->op = (opcode >> 1);
	op->imm1 = (imm >> 11);
	op->imm8 = imm;
	op->imm3 = (imm >> 8);
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
			
/**
 * Move, plain 16-bit immediate
 */
typedef struct {
	uint32_t imm4:4;	/* Bits 11-14 */
	uint32_t op2:2;  	/* Bottom 2 bits of op-code */
	uint32_t typea:1;	/* '1'b */
	uint32_t op:1;		/* Top bit of op-code */
	uint32_t typeb:2;	/* '10'b' */
	uint32_t imm1:1;	/* Bit 15 of immediate */
	uint32_t type:5;	/* '11110'b */
	uint32_t imm8:8;	/* Bottom 0-7 of immdiate */
	uint32_t rd:4;		/* Register d */
	uint32_t imm3:3;	/* Bits 8-10 of immediate */
	uint32_t typec:1;	/* '0'b */
} mvp32_t;

#define OP_MVP32	0x0000f240

static __THUMB_INLINE__ void
arm_mvp32(void **code, int rd, int imm, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	mvp32_t *op = *(mvp32_t **) code;
	*p = OP_MVP32;
	op->rd = rd;
	op->op2 = opcode;
	op->op = (opcode >> 2);
	op->imm1 = (imm >> 11);
	op->imm4 = (imm >> 12);
	op->imm8 = imm;
	op->imm3 = (imm >> 8);
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
			
/**
 * Bit field operations. Saturation with shift
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t typea:1;	/* '0'b */
	uint32_t op:3;		/* Op-code */
	uint32_t typeb:2;	/* '110'b' */
	uint32_t type:5;	/* '11110'b */
	uint32_t imm5:5;	/* Bits 0-4 of immediate */
	uint32_t sbz:1;		/* '0'b */
	uint32_t imm2:2;	/* Bits 5-6 of immediate */
	uint32_t rd:4;		/* Register d */
	uint32_t imm3:3;	/* Bits 7-10 of immediate */
	uint32_t typec:1;	/* '0'b */
} bfo32_t;

#define OP_BFO32	0x0000f300

static __THUMB_INLINE__ void
arm_bfo32(void **code, int rd, int imm, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	bfo32_t *op = *(bfo32_t **) code;
	*p = OP_BFO32;
	op->rd = rd;
	op->op = opcode;
	op->imm5 = imm;
	op->imm2 = (imm >> 5);
	op->imm3 = (imm >> 7);
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
			
/**
 * Data processing: constant shift
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t cond:1; 	/* Set condition code */
	uint32_t op:4;		/* Op-code */
	uint32_t type:7;	/* '1110101'b */
	uint32_t rm:4;		/* Register m */
	uint32_t optype:2;	/* Operation type */
	uint32_t imm2:2;	/* Bits 0-1 of immediate */
	uint32_t rd:4;		/* Register d */
	uint32_t imm3:3;	/* Bits 2-4 of immediate */
	uint32_t sbz:1;		/* '0'b */
} dpc32_t;

#define OP_DPC32	0x0000ea00

static __THUMB_INLINE__ void
arm_dpc32(void **code, int rd, int rn, int rm, int imm, int cond, int opcode, int optype)
{
	uint32_t *p = *(uint32_t **) code;
	dpc32_t *op = *(dpc32_t **) code;
	*p = OP_DPC32;
	op->rn = rn;
	op->rm = rm;
	op->rd = rd;
	op->cond = cond;
	op->optype = optype;
	op->op = opcode;
	op->imm2 = imm;
	op->imm3 = (imm >> 2);
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
			
/**
 * Register-controlled shift
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t op:4;		/* Op-code */
	uint32_t type:8;	/* '11111010'b */
	uint32_t rm:4;		/* Regiser m */
	uint32_t op2:4;		/* Op-code */
	uint32_t rd:4;		/* Register d */
	uint32_t typea:4;	/* '1111'b */
} rcs32_t;

#define OP_RCS32	0xf000fa00

static __THUMB_INLINE__ void
arm_rcs32(void **code, int rd, int rn, int rm, int opcode, int op2)
{
	uint32_t *p = *(uint32_t **) code;
	rcs32_t *op = *(rcs32_t **) code;
	*p = OP_RCS32;
	op->rn = rn;
	op->rm = rm;
	op->op = opcode;
	op->op2 = 0;
	op->rd = rd;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Sign or zero extension with optional addition
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t op:2;		/* Op-code */
	uint32_t type:9;	/* '111110100'b */
	uint32_t rm:4;		/* Register m */
	uint32_t rot:2;		/* Rot */
	uint32_t typea:2;	/* '01'b */
	uint32_t rd:4;		/* Register d */
	uint32_t typeb:4;	/* '1111'b */
} szx32_t;

#define OP_SZX32	0xf080fa00
			
static __THUMB_INLINE__ void
arm_szx32(void **code, int rd, int rn, int rm, int rot, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	szx32_t *op = *(szx32_t **) code;
	*p = OP_SZX32;
	op->rn = rn;
	op->rot = rot;
	op->op = opcode;
	op->rm = rm;
	op->rd = rd;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * SIMD add or subtract 
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t op:2;		/* Op-code */
	uint32_t type:9;	/* '111110101'b */
	uint32_t rm:4;		/* Register m */
	uint32_t pfx:3;		/* Prefix */
	uint32_t typea:1;	/* '0'b */
	uint32_t rd:4;		/* Register d */
	uint32_t typeb:4;	/* '1111'b */
} sim32_t;

#define OP_SIM32	0xf000fa80
			
static __THUMB_INLINE__ void
arm_sim32(void **code, int rd, int rn, int rm, int pfx, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	sim32_t *op = *(sim32_t **) code;
	*p = OP_SIM32;
	op->rn = rn;
	op->pfx = pfx;
	op->op = opcode;
	op->rm = rm;
	op->rd = rd;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Other data processing instructions
 */
typedef struct {
	uint32_t rm:4;		/* Register m */
	uint32_t op2:3;		/* Op-code part 2 */
	uint32_t typea:1;	/* '1'b */
	uint32_t rd:4;		/* Register d */
	uint32_t typeb:4;	/* '1111'b */
	uint32_t rn:4;		/* Register n */
	uint32_t op:2;		/* Op-code */
	uint32_t type:9;	/* '111110101'b */
} odp32_t;

#define OP_ODP32	0xf080fa80
			
static __THUMB_INLINE__ void
arm_odp32(void **code, int rd, int rn, int rm, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	odp32_t *op = *(odp32_t **) code;
	*p = OP_ODP32;
	op->rn = rn;
	op->op2 = (opcode >> 2);
	op->op = opcode;
	op->rm = rm;
	op->rd = rd;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * 32-bit mutiplies and sum of absolute differences with(out) accumulate
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t op1:3;		/* Op-code */
	uint32_t type:9;	/* '111110110'b */
	uint32_t rm:4;		/* Register m */
	uint32_t op2:2;		/* Op-code part 2 */
	uint32_t typea:2;	/* '00'b */
	uint32_t rd:4;		/* Register d */
	uint32_t ra:4; 	 	/* Accumulator register */
} msa32_t;

#define OP_MSA32	0x0000fb00

static __THUMB_INLINE__ void
arm_msa32(void **code, int rd, int rn, int rm, int ra, int op1, int op2)
{
	uint32_t *p = *(uint32_t **) code;
	msa32_t *op = *(msa32_t **) code;
	*p = OP_MSA32;
	op->rn = rn;
	op->op2 = op2;
	op->op1 = op1;
	op->ra = ra;
	op->rm = rm;
	op->rd = rd;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * 64-bit mutiplies and multiply accumulates. Divides.
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t op1:3;		/* Op-code */
	uint32_t type:9;	/* '111110111'b */
	uint32_t rm:4;		/* Register m */
	uint32_t op2:4;		/* Op-code part 2 */
	uint32_t rdhi:4;	/* Register d high */
	uint32_t rdlo:4;	/* Register d low */
} mad32_t;

#define OP_MAD32	0x0000fbc0
		
static __THUMB_INLINE__ void
arm_mad32(void **code, int rdhi, int rdlo, int rn, int rm, int op1, int op2)
{
	uint32_t *p = *(uint32_t **) code;
	mad32_t *op = *(mad32_t **) code;
	*p = OP_MAD32;
	op->rn = rn;
	op->op2 = op2;
	op->op1 = op1;
	op->rm = rm;
	op->rdhi = rdhi;
	op->rdlo = rdlo;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Coprocessor - MRRC and MCRR coprocessor register transfer
 */
typedef struct {
	uint32_t rt2:4;		/* Register t2 */
	uint32_t l:1;		/* Selector */
	uint32_t typea:7;	/* '11000010'b */
	uint32_t c:1;		/* C */
	uint32_t type:3;	/* '111'b */
	uint32_t crm:4;		/* Coprocessor Register m */
	uint32_t op:4;		/* Op-code */
	uint32_t coproc:4;	/* Coprocessor */
	uint32_t rt:4;		/* Register t */
} crr32_t;

#define OP_CRR32	0x0000ec20

static __THUMB_INLINE__ void
arm_crr32(void **code, int c, int l, int rt, int rt2, int coproc, int crm, int opcode)
{
	uint32_t *p = *(uint32_t **) code;
	crr32_t *op = *(crr32_t **) code;
	*p = OP_CRR32;
	op->crm = crm;
	op->op = opcode;
	op->rt = rt;
	op->rt2 = rt2;
	op->coproc = coproc;
	op->c = c;
	op->l = l;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Coprocessor - Load and Store Co-processor
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t l:1;		/* Selector */
	uint32_t w:1;		/* Selector */
	uint32_t n:1;		/* Selector */
	uint32_t u:1;		/* Selector */
	uint32_t p:1;		/* Selector */
	uint32_t typea:3;	/* '110'b */
	uint32_t c:1;		/* C */
	uint32_t type:3;	/* '111'b */
	uint32_t imm:8;		/* Immediate value */
	uint32_t coproc:4;	/* Coprocessor */
	uint32_t crd:4;		/* Coprocessor Register d */
} lsc32_t;

#define OP_LSC32	0x0000ec20

static __THUMB_INLINE__ void
arm_lsc32(void **code, int c, int p, int u, int n, int w, int l, 
	  int rn, int crd, int coproc, int imm)
{
	uint32_t *x = *(uint32_t **) code;
	lsc32_t *op = *(lsc32_t **) code;
	*x = OP_LSC32;
	op->imm = imm;
	op->crd = crd;
	op->coproc = coproc;
	op->c = c;
	op->p = p;
	op->u = u;
	op->l = l;
	op->n = n;
	op->w = w;
	op->rn = rn;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Coprocessor - Data Processing
 */
typedef struct {
	uint32_t crn:4;		/* Coprocessor Register n */
	uint32_t opc1:4;	/* Op-code 1 */
	uint32_t typeb:4;	/* '1110'b */
	uint32_t c:1;		/* C */
	uint32_t type:3;	/* '111'b */
	uint32_t crm:4;		/* Coprocessor Register m */
	uint32_t typea:1;	/* '0'b */
	uint32_t opc2:3;	/* Op-code 2 */
	uint32_t coproc:4;	/* Coprocessor */
	uint32_t crd:4;		/* Coprocessor Register d */
} cdp32_t;

#define OP_CDP32	0x0000ee00

static __THUMB_INLINE__ void
arm_cdp32(void **code, int crd, int crn, int crm, int c, int coproc, int opc2, int opc1)
{
	uint32_t *p = *(uint32_t **) code;
	cdp32_t *op = *(cdp32_t **) code;
	*p = OP_CDP32;
	op->crm = crm;
	op->crn = crn;
	op->crd = crd;
	op->coproc = coproc;
	op->c = c;
	op->opc1 = opc1;
	op->opc2 = opc2;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Coprocessor - MRC and MCR coprocessor register transfers
 */
typedef struct {
	uint32_t crn:4;		/* Coprocessor Register n */
	uint32_t l:1;		/* L */
	uint32_t opc1:3;	/* Op-code 1 */
	uint32_t typeb:4;	/* '1110'b */
	uint32_t c:1;		/* C */
	uint32_t type:3;	/* '111'b */
	uint32_t crm:4;		/* Coprocessor Register m */
	uint32_t typea:1;	/* '1'b */
	uint32_t opc2:3;	/* Op-code 2 */
	uint32_t coproc:4;	/* Coprocessor */
	uint32_t rxf:4;		/* RXF */
} mrt32_t;

#define OP_MRT32	0x0010ee00

static __THUMB_INLINE__ void
arm_mrt32(void **code, int c, int l, int crn, int rxf, int crm, int coproc, int opc1, int opc2)
{
	uint32_t *p = *(uint32_t **) code;
	mrt32_t *op = *(mrt32_t **) code;
	*p = OP_MRT32;
	op->crm = crm;
	op->opc1 = opc1;
	op->opc2 = opc2;
	op->rxf = rxf;
	op->crn = crn;
	op->coproc = coproc;
	op->c = c;
	op->l = l;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store - single data item, and memory hints: pc/rn 
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t ls:1;		/* Load/store selector */
	uint32_t size:2;	/* Size */
	uint32_t up:1;		/* Upwards indexing selector */
	uint32_t sign:1;	/* Sign extending selector */
	uint32_t type:7;	/* '1111100'b */
	uint32_t imm:12;	/* +/- immediate */
	uint32_t rt:4;		/* Register t */
} lspr32_t;

#define OP_LSPR32	0x0000f800
#define LDRPC_TEMPLATE	0xc000f8df

static __THUMB_INLINE__ void
arm_lspr32(void **code, int rt, int rn, int ls, int size, int up, int imm, int sign)
{
	uint32_t *p = *(uint32_t **) code;
	lspr32_t *op = *(lspr32_t **) code;
	*p = OP_LSPR32;
	op->rt = rt;
	op->rn = rn;
	op->ls = ls;
	op->size = size;
	op->up = up;
	op->imm = imm;
	op->sign = sign;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store - single data item, and memory hints: rn imm8
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t ls:1;		/* Load/store selector */
	uint32_t size:2;	/* Size */
	uint32_t up:1;		/* Upwards indexing selector */
	uint32_t sign:1;	/* Sign extending selector */
	uint32_t type:7;	/* '1111100'b */
	uint32_t imm:8;		/* +/- immediate */
	uint32_t idx:1;		/* Pre/post index */
	uint32_t priv:1;	/* User privilege selector */
	uint32_t prep:1;	/* Pre/post selector */
	uint32_t shift:1;	/* Shift selector */
	uint32_t rt:4;		/* Register t */
} lsrn32_t;

#define OP_LSRN32	0x0000f800

static __THUMB_INLINE__ void
arm_lsrn32(void **code, int rt, int rn, int ls, int size,
	   int imm, int sign, int idx, int priv)
{
	uint32_t *p = *(uint32_t **) code;
	lsrn32_t *op = *(lsrn32_t **) code;
	*p = OP_LSRN32;
	op->rt = rt;
	op->rn = rn;
	op->ls = ls;
	op->size = size;
	if (imm >= 0) {
		op->up = 1;
		op->imm = imm;
	} else {
		op->up = 0;
		op->imm = -imm;
		op->shift = 1;
		op->prep = 1;
	}
	op->sign = sign;
	op->idx = idx;
	op->priv = priv;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store - single data item, and memory hints: Rn + shifted r
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t ls:1;		/* Load/store selector */
	uint32_t size:2;	/* Size */
	uint32_t typea:1;	/* '0'b */
	uint32_t sign:1;	/* Sign extending selector */
	uint32_t type:7;	/* '1111100'b */
	uint32_t rm:4;		/* Register m */
	uint32_t shift:2;	/* Pre/post index */
	uint32_t typeb:6;	/* '000000'b */
	uint32_t rt:4;		/* Register t */
} lsro32_t;

#define OP_LSRO32	0x0000f800

static __THUMB_INLINE__ void
arm_lsro32(void **code, int rt, int rn, int rm, int ls, int size, int sign, int shift)
{
	uint32_t *p = *(uint32_t **) code;
	lsro32_t *op = *(lsro32_t **) code;
	*p = OP_LSRO32;
	op->rt = rt;
	op->rn = rn;
	op->ls = ls;
	op->rm = rm;
	op->size = size;
	op->sign = sign;
	op->shift = shift;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store - double
 */
typedef struct {
	uint32_t rn :4;		/* Register n */
	uint32_t ls:1;		/* Load/store selector */
	uint32_t wb:1;		/* Write back selector */
	uint32_t typea:1;	/* '1'b */
	uint32_t up:1;		/* Upwards indexing selector */
	uint32_t prep:1;	/* Pre/post index selector */
	uint32_t type:7;	/* '1110100'b */
	uint32_t imm:8;		/* Immediate */
	uint32_t rt2d:4;	/* Register t2 or d */
	uint32_t rt:4;		/* Register t */
} lsd32_t;

#define OP_LSD32	0x0000e840

static __THUMB_INLINE__ void
arm_lsd32(void **code, int rt, int rt2d, int rn, int ls, int up, int prep)
{
	uint32_t *p = *(uint32_t **) code;
	lsd32_t *op = *(lsd32_t **) code;
	*p = OP_LSD32;
	op->rt = rt;
	op->rt2d = rt2d;
	op->rn = rn;
	op->ls = ls;
	op->up = up;
	op->prep = prep;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store - exclusive and table branch
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t ls:1;		/* Load/store selector */
	uint32_t type:11;	/* '111010000010'b */
	uint32_t imm:8;		/* Immediate value */
	uint32_t rd:4;		/* Register d */
	uint32_t rt:4;		/* Register t */
} lsxt32_t;

#define OP_LSXT32	0x0000e840

static __THUMB_INLINE__ void
arm_lsxt32(void **code, int rd, int rt, int rn, int ls, int imm)
{
	uint32_t *p = *(uint32_t **) code;
	lsxt32_t *op = *(lsxt32_t **) code;
	*p = OP_LSXT32;
	op->rd = rd;
	op->rt = rt;
	op->rn = rn;
	op->ls = ls;
	op->imm = imm;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Load/Store Multiple
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t ls:1;		/* Load/store selector */
	uint32_t wb:1;		/* Write back selector */
	uint32_t typea:1;	/* '0'b */
	uint32_t u:1;		/* Upwards selector */
	uint32_t v:1;		/* Not U */
	uint32_t type:7;	/* '1110100'b */
	uint32_t mask:13;	/* Register mask */
	uint32_t sbz:1;		/* '0'b */
	uint32_t m:1;		/* Load/Store Register 14 */
	uint32_t p:1;		/* Load PC */
} lsm32_t;

#define OP_LSM32	0x0000e800

static __THUMB_INLINE__ void
arm_lsm32(void **code, int rn, int mask, int ls, int up, int wb)
{
	uint32_t *x = *(uint32_t **) code;
	lsm32_t *op = *(lsm32_t **) code;
	*x = OP_LSM32;
	op->rn = rn;
	op->mask = mask & 0x1fff;
	op->ls = ls;
	op->u = up;
	op->v = ~op->u;
	op->wb = wb;
	op->p = mask >> ARMREG_PC;
	op->m = mask >> ARMREG_LR;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Branch/Branch with link
 */
typedef struct {
	uint32_t offhi:10;	/* Offset bits 11-20 */
	uint32_t sign:1;	/* Sign of offset */
	uint32_t type:5;	/* '11110'b */
	uint32_t offlo:11;	/* Offset bits 0-10 */
	uint32_t i2:1;		/* Offset bit 18 */
	uint32_t unc:1;		/* '1'b - unconditional */
	uint32_t i1:1;		/* Offset bit 19 */
	uint32_t link:1;	/* With link selector */
	uint32_t typea:1;	/* '1'b */
} brl32_t;

#define OP_BRL32	0x9000f000
#define BL_TEMPLATE	(OP_BRL32 | 0x40000000)
#define BR_TEMPLATE	OP_BRL32

static __THUMB_INLINE__ void
arm_brl32(void **code, int offset, int link)
{
	uint32_t *p = *(uint32_t **) code;
	brl32_t *op = *(brl32_t **) code;
	offset -= 4;
	int imm = offset >> 1;
	*p = OP_BRL32;
	op->offlo = imm;
	op->offhi = (imm >> 11);
	if (offset < 0)
		op->sign = 1;
	op->i2 = ~((imm >> 21) ^ op->sign);
	op->i1 = ~((imm >> 22) ^ op->sign);
	op->link = link;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Conditional Branch
 */
typedef struct {
	uint32_t offhi:6;	/* Offset bits 11-16 */
	uint32_t cond:4;	/* Condition */
	uint32_t sign:1;	/* Sign of offset */
	uint32_t type:5;	/* '11110'b */
	uint32_t offlo:11;	/* Offset bits 0-10 */
	uint32_t i2:1;		/* Offset bit 18 */
	uint32_t typea:1;	/* '0'b */
	uint32_t i1:1;		/* Offset bit 19 */
	uint32_t link:1;	/* With link selector */
	uint32_t typeb:1;	/* '1'b */
} cbr32_t;

#define OP_CBR32	0x8000f000

static __THUMB_INLINE__ void
arm_cbr32(void **code, int cond, int offset)
{
	uint32_t *p = *(uint32_t **) code;
	cbr32_t *op = *(cbr32_t **) code;
	int imm = (offset - 4) >> 1;
	*p = OP_CBR32;
	op->offlo = imm;
	op->offhi = (imm >> 11);
	if (imm < 0)
		op->sign = 1;
	op->i2 = (imm >> 17);
	op->i1 = (imm >> 18);
	op->cond = cond;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Floating Point - Data Processing
 */
typedef struct {
	uint32_t opc2:4;	/* Opcode 2 */
	uint32_t opc1:4;	/* Opcode 1 */
	uint32_t typea:4;	/* '1110'b */
	uint32_t t:1;		/* T field */
	uint32_t typed:3;	/* '111'b */
	uint32_t opc4:4;	/* Opcode 4 */
	uint32_t typec:1;	/* '0'b */
	uint32_t opr2:1;	/* Operand 2 */
	uint32_t opc3:2;	/* Opcode 3 */
	uint32_t size:1;	/* Size */
	uint32_t typeb:3;	/* '101'b */
	uint32_t opr1:4;	/* Operand 1 */
} fdp_t;

#define OP_FDP	0x0a00ee00

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = x; aaaa = opc1; bbbb = opc2; CCCC=opr1; S=size; dd=opc3 E=opr2; ffff=opc4
 */

static __THUMB_INLINE__ void
arm_fdp(void **code, int t, int opc1, int opc2, int opc3, int opc4, int opr1, int opr2, int size)
{
	uint32_t *p = *(uint32_t **) code;
	fdp_t *op = *(fdp_t **) code;
	*p = OP_FDP;
	op->t = t;
	op->opc1 = opc1;
	op->opc2 = opc2;
	op->opc3 = opc3;
	op->opc4 = opc4;
	op->opr1 = opr1;
	op->opr2 = opr2;
	op->size = size;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/*
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f 
 * T = 1; aaaa = 0||Vd[1]||cc; bbbb = Vn; CCCC = Vd S = size dd = Vn[0]||0
 * E = Vm[0]; Vm[0]; ffff = Vm
 */

static __THUMB_INLINE__ void
arm_vsel(void **code, int vd, int vn, int vm, int size, int cc)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vn >>= 1; vd >>= 1; vm >>= 1;
		opc1 = (((vd >> 4) << 2) | cc);
		opc2 = vn & 0xf;
		opr1 = vd & 0xf;
		opc3 = ((vn >> 4) << 1);
		opr2 = vm >> 4;
		opc4 = vm & 0xf; 
	} else {
		opc1 = (((vd & 0x1) << 2) | cc);
		opc2 = vn >> 1;
		opr1 = vd >> 1;
		opc3 = ((vn & 0x1) << 1);
		opr2 = vm & 0x1;
		opc4 = vm >> 1;
	}
	arm_fdp(code, 1, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_VSEL(p, vd, vn, vm, sz, cc)	arm_vsel((void **) &p, vd, vn, vm, sz, cc)

/**
 * Floating Point - VABS
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 0||Vd[1]||00; opc2 = 0000; opr1=Vd; S=size; opc3=11 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fabs(void **code, int vrd, int vrm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	opc2 = 0;
	opc3 = 3;
	if (size == 1) {
		vrd >>= 1; vrm >>= 1;
		opc1 = ((vrd >> 4) << 2);
		opr1 = vrd & 0xf;
		opr2 = ((vrm >> 4) << 1);
		opc4 = vrm & 0xf; 
	} else {
		opc1 = ((vrd & 0x1) << 2);
		opr1 = vrd >> 1;
		opr2 = vrm & 0x1;
		opc4 = vrm >> 1;
	}
	opc1 |= 0xb;
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_ABSS(p, vrd, vrn)		arm_fabs((void **)&p, vrd, vrn, 0)
#define ARM_ABSD(p, vrd, vrn)		arm_fabs((void **)&p, vrd, vrn, 1)

/**
 * Floating Point - VADD
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 0||Vd[1]||11; opc2 = Vn; opr1=Vd; S=size; opc3=Vn[0]||0 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fadd(void **code, int vd, int vn, int vm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vd >>= 1; vm >>= 1; vn >>= 1;
		opc1 = (((vd >> 4) << 2) | 0x3);
		opc2 = vn & 0xf;
		opr1 = vd & 0xf;
		opc3 = ((vn >> 4) << 1);
		opr2 = vm >> 4;
		opc4 = vm & 0xf; 
	} else {
		opc1 = (((vd & 0x1) << 2) | 0x3);
		opc2 = vn >> 1;
		opr1 = vd >> 1;
		opc3 = ((vn & 0x1) << 1);
		opr2 = vm & 0x1;
		opc4 = vm >> 1;
	}
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_VFP_ADDD(p, vrd, vrn, vrm)	arm_fadd((void **)&p, vrd, vrn, vrm, 1)
#define ARM_VFP_ADDS(p, vrd, vrn, vrm)	arm_fadd((void **)&p, vrd, vrn, vrm, 0)

/**
 * Floating Point - VSUB
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 0||Vd[1]||11; opc2 = Vn; opr1=Vd; S=size; opc3=Vn[0]||1 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fsub(void **code, int vd, int vn, int vm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vd >>= 1; vm >>= 1; vn >>= 1;
		opc1 = (((vd >> 4) << 2) | 0x3);
		opc2 = vn & 0xf;
		opr1 = vd & 0xf;
		opc3 = (((vn >> 4) << 1) | 0x1);
		opr2 = vm >> 4;
		opc4 = vm & 0xf; 
	} else {
		opc1 = (((vd & 0x1) << 2) | 0x3);
		opc2 = vn >> 1;
		opr1 = vd >> 1;
		opc3 = (((vn & 0x1) << 1) | 0x1);
		opr2 = vm & 0x1;
		opc4 = vm >> 1;
	}
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_VFP_SUBD(p, vrd, vrn, vrm)	arm_fsub((void **)&p, vrd, vrn, vrm, 1)
#define ARM_VFP_SUBS(p, vrd, vrn, vrm)	arm_fsub((void **)&p, vrd, vrn, vrm, 0)

/**
 * Floating Point - VDIV
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 0||Vd[1]||11; opc2 = Vn; opr1=Vd; S=size; opc3=Vn[0]||0 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fdiv(void **code, int vd, int vn, int vm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vd >>= 1; vm >>= 1; vn >>= 1;
		opc1 = (((vd >> 4) << 2) | 0x8);
		opc2 = vn & 0xf;
		opr1 = vd & 0xf;
		opc3 = ((vn >> 4) << 1);
		opr2 = vm >> 4;
		opc4 = vm & 0xf; 
	} else {
		opc1 = (((vd & 0x1) << 2) | 0x8);
		opc2 = vn >> 1;
		opr1 = vd >> 1;
		opc3 = ((vn & 0x1) << 1);
		opr2 = vm & 0x1;
		opc4 = vm >> 1;
	}
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_VFP_DIVD(p, vrd, vrn, vrm)	arm_fdiv((void **)&p, vrd, vrn, vrm, 1)
#define ARM_VFP_DIVS(p, vrd, vrn, vrm)	arm_fdiv((void **)&p, vrd, vrn, vrm, 0)

/**
 * Floating Point - VMUL
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 0||Vd[1]||11; opc2 = Vn; opr1=Vd; S=size; opc3=Vn[0]||0 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fmul(void **code, int vd, int vn, int vm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vd >>= 1; vm >>= 1; vn >>= 1;
		opc1 = (((vd >> 4) << 2) | 0x2);
		opc2 = vn & 0xf;
		opr1 = vd & 0xf;
		opc3 = ((vn >> 4) << 1);
		opr2 = vm >> 4;
		opc4 = vm & 0xf; 
	} else {
		opc1 = (((vd & 0x1) << 2) | 0x2);
		opc2 = vn >> 1;
		opr1 = vd >> 1;
		opc3 = ((vn & 0x1) << 1);
		opr2 = vm & 0x1;
		opc4 = vm >> 1;
	}
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_VFP_MULD(p, vrd, vrn, vrm)	arm_fmul((void **)&p, vrd, vrn, vrm, 1)
#define ARM_VFP_MULS(p, vrd, vrn, vrm)	arm_fmul((void **)&p, vrd, vrn, vrm, 0)

/**
 * Floating Point - VCMP
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 1||Vd[1]||11; opc2 = 1111; opr1=Vd; S=size; opc3=01 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fcmp(void **code, int vd, int vm, int size, int imm)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vd >>= 1; vm >>= 1;
		opr2 = (vm & 0x10) >> 3;
		opc4 = vm & 0xf;
		opc1 = (vd >> 2) & 0x4; 
		opr1 = vd & 0xf;
	} else {
		opr2 = (vm & 1) << 1;
		opc4 = vm >> 1;
		opc1 = (vd & 1) << 2;
		opr1 = vd >> 1;
	}
	opc1 |= 0xb;
	opc3 = 1;
	opc2 = 4 | imm;
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

/* Compare - register-to-register */
#define ARM_CMPD(p, vrd, vrm) 		arm_fcmp((void **)&p, vrd, vrm, 1, 0)
#define ARM_CMPS(p, vrd, vrm) 		arm_fcmp((void **)&p, vrd, vrm, 0, 0)

/* Compare - register-to-zero */
#define ARM_CMPZD(p, vrd) 		arm_fcmp((void **)&p, vrd, 0, 1, 1)
#define ARM_CMPZS(p, vrd) 		arm_fcmp((void **)&p, vrd, 0, 0, 1)

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 1||Vd[1]||11; opc2 = 1101; opr1=Vd; S=size; opc3=[op]1 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fcvt(void **code, int vd, int vm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	if (size == 1) {
		vm >>= 1;
		opr2 = (vm & 0x10) >> 2;
		opc4 = vm & 0xf;
		opc1 = (vd & 1) << 2;
		opr1 = (vd >> 1) & 0xf;
	} else {
		vd >>= 1;
		opr2 = (vm & 0x1) >> 2;
		opc4 = (vm >> 1) & 0xf;
		opc1 = (vd & 0x10) >> 1;
		opr1 = vd & 0xf;
	}
	opc1 |= 0xb;
	opc3 = 3;
	opc2 = 7;
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_CVTD(p, vrd, vrm) 		arm_fcvt((void **)&p, vrd, vrm, 1)
#define ARM_CVTS(p, vrd, vrm) 		arm_fcvt((void **)&p, vrd, vrm, 0)

/**
 * Floating Point - VCVT - Fixed/Float
 */
typedef struct {
	uint32_t rm:2;		/* Rounding mode */
	uint32_t typea:4;	/* '1111'b */
	uint32_t vdhi:1;	/* FP register d high bit */
	uint32_t type:5;	/* '11101'b */
	uint32_t t:1;		/* t field */
	uint32_t typeb:3;	/* '111'b */
	uint32_t vm:4;		/* FP register d */
	uint32_t typec:1;	/* '0'B */
	uint32_t vmhi:1;	/* FP register m high bit */
	uint32_t typed:1;	/* '1'b */
	uint32_t op:1;		/* Op */
	uint32_t size:1;	/* Single/Double selector */
	uint32_t typee:3;	/* '101'B */
	uint32_t vd:4;  	/* FP register m */
} fcvt_t;

#define OP_VFCVT 	0x0a40eebc

static __THUMB_INLINE__ void
arm_fcvtr(void **code, int vrd, int vrm, int sign, int size, int toInt)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2, t;

	t = 0;
	if (toInt == 1) {
		opc3 = 3;
		opc2 = 0xc | sign;
		opr1 = vrd >> 1;
		opc1 = (vrd & 1) << 2;
		if (size == 1) {
			vrm >>= 1;
			opr2 = (vrm & 0x10) >> 2;
			opc4 = vrm & 0xf;
		} else {
			opr2 = (vrm & 1);
			opc4 = vrm >> 1;
		}
	} else {
		opc2 = 0x8;
		if (sign)
			opc3 = 3;
		else
			opc3 = 1;
		opr2 = vrm & 1;
		opc4 = vrm >> 1;
		if (size == 1) {
			vrd >>= 1;
			opc1 = (vrd & 0x10) >> 1;
			opr1 = vrd & 0xf;
		} else {
			opc1 = (vrd & 1) << 2;
			opr1 = (vrd >> 1) & 0xf;
		}
	}
	opc1 |= 0xb;
	arm_fdp(code, t, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_FSITOS(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 1, 0, 0)
#define ARM_FSITOD(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 1, 1, 0)
#define ARM_FUITOS(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 0, 0, 0)
#define ARM_FUITOD(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 0, 1, 0)

#define ARM_TOSIZD(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 1, 1, 1)
#define ARM_TOSIZS(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 1, 0, 1)
#define ARM_TOUIZD(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 0, 1, 1)
#define ARM_TOUIZS(p, rd, rm)	arm_fcvtr((void **) &p, rd, rm, 0, 0, 1)

/**
 * Floating Point - VMOV - register-to-register
 */

static __THUMB_INLINE__ void
arm_fmovr(void **code, int vrd, int vrm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	opc2 = 0;
	opc3 = 1;
	if (size == 1) {
		vrm >>= 1; vrd >>= 1;
		opc1 = ((vrd >> 4) << 2);
		opr1 = vrd & 0xf;
		opr2 = ((vrm >> 4) << 1);
		opc4 = vrm & 0xf; 
	} else {
		opc1 = ((vrd & 0x1) << 2);
		opr1 = vrd >> 1;
		opr2 = vrm & 0x1;
		opc4 = vrm >> 1;
	}
	opc1 |= 0xb;
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_CPYS(p, rd, vm)	arm_fmovr((void **) &p, rd, vm, 0)
#define ARM_CPYD(p, rd, vm)	arm_fmovr((void **) &p, rd, vm, 1)

/**
 * Floating Point - VNEG
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 0||Vd[1]||00; opc2 = 0001; opr1=Vd; S=size; opc3=01 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fneg(void **code, int vrd, int vrm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	opc2 = 1;
	opc3 = 1;
	if (size == 1) {
		vrd >>= 1; vrm >>= 1;
		opc1 = ((vrd >> 4) << 2);
		opr1 = vrd & 0xf;
		opr2 = ((vrm >> 4) << 1);
		opc4 = vrm & 0xf; 
	} else {
		opc1 = ((vrd & 0x1) << 2);
		opr1 = vrd >> 1;
		opr2 = vrm & 0x1;
		opc4 = vrm >> 1;
	}
	opc1 |= 0xb;
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_NEGS(p, rd, rm)	arm_fneg((void **) &p, rd, rm, 0)
#define ARM_NEGD(p, rd, rm)	arm_fneg((void **) &p, rd, rm, 1)

/**
 * Floating Point - VSQRT
 */

/* 
 * 1 1 1 T 1 1 1 0 a a a a b b b b C C C C 1 0 1 S d d E 0 f f f f
 * T = 0; opc1 = 1||Vd[1]||11; opc2 = 0001; opr1=Vd; S=size; opc3=11 opr2=Vm[0] opc4=Vm
 * 
 */

static __THUMB_INLINE__ void
arm_fsqrt(void **code, int vrd, int vrm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	opc2 = 1;
	opc3 = 3;
	if (size == 1) {
		vrd >>= 1; vrm >>= 1;
		opc1 = ((vrd >> 4) << 2);
		opr1 = vrd & 0xf;
		opr2 = ((vrm >> 4) << 1);
		opc4 = vrm & 0xf; 
	} else {
		opc1 = ((vrd & 0x1) << 2);
		opr1 = vrd >> 1;
		opr2 = vrm & 0x1;
		opc4 = vrm >> 1;
	}
	opc1 |= 0xb;
	arm_fdp(code, 0, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_SQRTS(p, vrd, vrn)		arm_fsqrt((void **)&p, vrd, vrn, 0)
#define ARM_SQRTD(p, vrd, vrn)		arm_fsqrt((void **)&p, vrd, vrn, 1)

/**
 * Floating Point - VMOV - between two ARM core registers and a doubleword register
 */
typedef struct {
	uint32_t vrt2:4;   	/* FP register t2 */
	uint32_t op:1;		/* Op type */
	uint32_t type:11;	/* '11101100010'b */
	uint32_t vrm:4;		/* FP register m */
	uint32_t typea:1;	/* '1'b */
	uint32_t vrmhi:1;	/* Hi-bit of FP register m */
	uint32_t typed:6;	/* '101100'b */
	uint32_t vrt:4;  	/* FP register t */
} fmovd_t;

#define OP_FVMOVD 	0x0b10ec40

static __THUMB_INLINE__ void
arm_fmovd(void **code, int vrt, int vrt2, int vrm, int opr)
{
	uint32_t *x = *(uint32_t **) code;
	fmovd_t *op = *(fmovd_t **) code;
	*x = OP_FVMOVD;
	vrm >>= 1;
	op->op = opr;
	op->vrt = vrt;
	op->vrt2 = vrt2;
	op->vrm = vrm;
	op->vrmhi = (vrm >> 4);
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FMDRR(p, r1, r2, rd)	arm_fmovd((void **) &p, r1, r2, rd, 1)
#define ARM_FMRRD(p, r1, r2, rd)	arm_fmovd((void **) &p, r1, r2, rd, 0)

/**
 * Floating Point - VMOV - between ARM core register and a single-precision register
 */
typedef struct {
	uint32_t rn:4;		/* [FP|core] register n */
	uint32_t op:1;		/* Operation - 0: from 1: to */
	uint32_t type:11;  	/* '11101110000'b */
	uint32_t typea:7;	/* '0010000'b */
	uint32_t rnlo:1;	/* Lo-bit of [FP|core] register n */
	uint32_t typeb:4;	/* '1010'b */
	uint32_t rt:4;		/* [FP|core] Register t */
} fmovs_t;

#define OP_FVMOVS 	0x0a10ee00

static __THUMB_INLINE__ void
arm_fmovs(void **code, int rt, int rn, int opr)
{
	uint32_t *x = *(uint32_t **) code;
	fmovs_t *op = *(fmovs_t **) code;
	*x = OP_FVMOVS;
	op->op = opr;
	op->rt = rt;
	op->rn = (rn >> 1);
	op->rnlo = rn;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FMRS(p, rd, vn)	arm_fmovs((void **) &p, rd, vn, 1)
#define ARM_FMSR(p, vd, rn)	arm_fmovs((void **) &p, rn, vd, 0)

/**
 * Floating Point - Load/store [multiple] single/double from register offset
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t ls:1;		/* Load/Store selector */
	uint32_t w:1;		/* W */
	uint32_t vrdhi:1;	/* FP register d high bit */
	uint32_t add:1;		/* Add indicator */
	uint32_t p:1;		/* P */
	uint32_t type:7;	/* '11101101'b */
	uint32_t imm:8;		/* Offset */
	uint32_t size:1;	/* Double/single selector */
	uint32_t typeb:3;	/* '101'b */
	uint32_t vrd:4;  	/* FP register d */
} fpls_t;

#define OP_VFPLD 	0x0a00ed00

static __THUMB_INLINE__ void
arm_fpls(void **code, int vrd, int rn, int count, int size, int ls, int p, int w)
{
	uint32_t *x = *(uint32_t **) code;
	fpls_t *op = *(fpls_t **) code;
	*x = OP_VFPLD;
	op->size = size;
	op->rn = rn;
	op->ls = ls;
	op->w = w;
	op->p = p;
	op->add = 1;
	if (size == 1) {
		vrd >>= 1;
		op->vrd = vrd;
		op->vrdhi = (vrd >> 4);
		op->imm = count * 2;
	} else {
		op->vrd = (vrd >> 1);
		op->vrdhi = vrd;
		op->imm = count;
	}
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FLDMD(p, rd, count, rn)	arm_fpls((void **) &p, rd, rn, count, 1, 1, 0, 0)
#define ARM_FSTMD(p, rd, count, rn)	arm_fpls((void **) &p, rd, rn, count, 1, 0, 0, 0)
#define ARM_FLDMS(p, rd, count, rn)	arm_fpls((void **) &p, rd, rn, count, 0, 1, 0, 0)
#define ARM_FSTMS(p, rd, count, rn)	arm_fpls((void **) &p, rd, rn, count, 0, 0, 0, 0)

/**
 * Floating Point - VMRS - Moves the value of the FPSCR to a general-purpose register, 
 * 			   or the values of the FCSR flags to the APSR
 */
typedef struct {
	uint32_t type:16;  	/* '1110111011110001'b */
	uint32_t typea:12;	/* '101000010000'b */
	uint32_t rt:4;		/* Register t */
} fvmrs_t;

#define OP_FVMRS 	0x0a10eef1

static __THUMB_INLINE__ void
arm_fvmrs(void **code, int rt)
{
	uint32_t *x = *(uint32_t **) code;
	fvmrs_t *op = *(fvmrs_t **) code;
	*x = OP_FVMRS;
	op->rt = rt;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FMSTAT(p)		arm_fvmrs((void **) &p, 15)

/**
 * Floating Point - VMSR - Moves the value of a general-purpose register to the FPSCR
 */
typedef struct {
	uint32_t type:16;  	/* '1110111011100001'b */
	uint32_t typea:12;	/* '101000010000'b */
	uint32_t rt:4;		/* Register t */
} fvmsr_t;

#define OP_FVMSR 	0x0a10eee1

static __THUMB_INLINE__ void
arm_fvmsr(void **code, int rt)
{
	uint32_t *x = *(uint32_t **) code;
	fvmsr_t *op = *(fvmsr_t **) code;
	*x = OP_FVMSR;
	op->rt = rt;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

/**
 * Floating Point - VLDR
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t typea:2;	/* '01'b */
	uint32_t vrdhi:1;	/* FP register d high bit */
	uint32_t add:1; 	/* Add indicator */
	uint32_t type:8;	/* '11101101'b */
	uint32_t imm:8;		/* Offset */
	uint32_t size:1;	/* Single/Double selector */
	uint32_t typeb:3;	/* '101'b */
	uint32_t vrd:4;  	/* FP register d */
} fldr_t;

#define OP_FLDR		0x0a00ed10

static __THUMB_INLINE__ void
arm_fldr(void **code, int vrd, int rn, int size, int offset)
{
	uintptr_t loc = ((uintptr_t) *code % 4);
	uint32_t *p = *(uint32_t **) code;
	fldr_t *op = *(fldr_t **) code;
	*p = OP_FLDR;
	
	if ((rn == ARMREG_PC) && (loc != 0))
		offset += 2;

	if (size == 1) {
		vrd >>= 1;
		op->vrd = vrd;
		op->vrdhi = (vrd >> 4);
	} else {
		op->vrd = (vrd >> 1);
		op->vrdhi = vrd;
	}
	op->size = size;
	if (offset >= 0) {
		op->add = 1;
		op->imm = offset >> 2;
	} else 
		op->imm = -offset >> 2;
	op->rn = rn;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FLDS(p, vd, rm, offset)	arm_fldr((void **) &p, vd, rm, 0, offset)
#define ARM_FLDD(p, vd, rm, offset)	arm_fldr((void **) &p, vd, rm, 1, offset)

/**
 * Floating Point - VSTR
 */
typedef struct {
	uint32_t rn:4;		/* Register n */
	uint32_t typea:2;	/* '00'b */
	uint32_t vrdhi:1;	/* FP register d high bit */
	uint32_t add:1; 	/* Add indicator */
	uint32_t type:8;	/* '11101101'b */
	uint32_t imm:8;		/* Offset */
	uint32_t size:1;	/* Single/Double selector */
	uint32_t typeb:3;	/* '101'b */
	uint32_t vrd:4;  	/* FP register d */
} fstr_t;

#define OP_FSTR		0x0a00ed00

static __THUMB_INLINE__ void
arm_fstr(void **code, int vrd, int rn, int size, int offset)
{
	uintptr_t loc = ((uintptr_t) *code % 4);
	uint32_t *p = *(uint32_t **) code;
	fstr_t *op = *(fstr_t **) code;
	*p = OP_FSTR;

	if ((rn == ARMREG_PC) && (loc != 0))
		offset += 2;

	if (size == 1) {
		vrd >>= 1;
		op->vrd = vrd;
		op->vrdhi = (vrd >> 4);
	} else {
		op->vrd = (vrd >> 1);
		op->vrdhi = vrd;
	}
	op->size = size;
	if (offset >= 0) {
		op->add = 1;
		op->imm = offset >> 2;
	} else 
		op->imm = -offset >> 2;
	op->rn = rn;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FSTS(p, vd, rm, offset)	arm_fstr((void **) &p, vd, rm, 0, offset)
#define ARM_FSTD(p, vd, rm, offset)	arm_fstr((void **) &p, vd, rm, 1, offset)

/**
 * Floating Point - VRINT (round to integral)
 */

/* 
 * 1 1 1 1 1 1 1 0 1 D 1 1 1 0 R R d d d d 1 0 1 S 0 1 M 0 m m m m 
 * T=1; opc1=1|Vd[1]|11; opc2=10RR; opr1=V[d]; size=S opc3=01 opr2=Vm[0] opc4=V[m]
 */

static __THUMB_INLINE__ void
arm_vrint(void **code, int vrd, int vrm, int rm, int size)
{
	int opc1, opc2, opc3, opc4;
	int opr1, opr2;

	opc2 = 8 | rm;
	opc3 = 1;
	if (size == 1) {
		vrd >>= 1; vrm >>= 1;
		opc1 = ((vrd >> 4) << 2);
		opr1 = vrd & 0xf;
		opr2 = ((vrm >> 4) << 1);
		opc4 = vrm & 0xf; 
	} else {
		opc1 = ((vrd & 0x1) << 2);
		opr1 = vrd >> 1;
		opr2 = vrm & 0x1;
		opc4 = vrm >> 1;
	}
	opc1 |= 0xb;
	arm_fdp(code, 1, opc1, opc2, opc3, opc4, opr1, opr2, size);
}

#define ARM_VFP_RNDAS(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 0, 0)
#define ARM_VFP_RNDNS(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 1, 0)
#define ARM_VFP_RNDPS(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 2, 0)
#define ARM_VFP_RNDMS(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 3, 0)
#define ARM_VFP_RNDAD(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 0, 1)
#define ARM_VFP_RNDND(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 1, 1)
#define ARM_VFP_RNDPD(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 2, 1)
#define ARM_VFP_RNDMD(p, vrd, vrm)	arm_vrint((void **)&p, vrd, vrm, 3, 1)

/**
 * Floating Point - VPUSH (push consecutive list of registers to stack)
 */

typedef struct {
	uint32_t typec:6;	/* '101101'b */
	uint32_t vrdhi:1;	/* FP register d high bit */
	uint32_t typeb:9;	/* '111011010'b */
	uint32_t count:8;	/* List of registers */
	uint32_t size:1;	/* Size */
	uint32_t typea:3;	/* '101'b */
	uint32_t vrd:4;		/* Register d */
} fpush_t;

#define OP_FPUSH	0x0a00ed2d

static __THUMB_INLINE__ void
arm_vpush(void **code, int vrd, int count, int size)
{
	uint32_t *p = *(uint32_t **) code;
	fpush_t *op = *(fpush_t **) code;
	*p = OP_FPUSH;

	if (size == 1) {
		vrd >>= 1;
		op->vrd = vrd;
		op->vrdhi = (vrd >> 4);
		op->count = count << 1;
	} else {
		op->vrd = (vrd >> 1);
		op->vrdhi = vrd;
		op->count = count;
	}
	op->size = size;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FPUSHD(p, vrd, count)	arm_vpush ((void **)&p, vrd, count, 1)
#define ARM_FPUSHS(p, vrd, count)	arm_vpush ((void **)&p, vrd, count, 0)

/**
 * Floating Point - VPOP (push consecutive list of registers to stack)
 */

typedef struct {
	uint32_t typec:6;	/* '101101'b */
	uint32_t vrdhi:1;	/* FP register d high bit */
	uint32_t typeb:9;	/* '111011010'b */
	uint32_t count:8;	/* Count of registers */
	uint32_t size:1;	/* Size */
	uint32_t typea:3;	/* '101'b */
	uint32_t vrd:4;		/* Register d */
} fpop_t;

#define OP_FPOP		0x0a00ecbd

static __THUMB_INLINE__ void
arm_vpop(void **code, int vrd, int count, int size)
{
	uint32_t *p = *(uint32_t **) code;
	fpop_t *op = *(fpop_t **) code;
	*p = OP_FPOP;

	if (size == 1) {
		vrd >>= 1;
		op->vrd = vrd;
		op->vrdhi = (vrd >> 4);
		op->count = count << 1;
	} else {
		op->vrd = (vrd >> 1);
		op->vrdhi = vrd;
		op->count = count;
	}
	op->size = size;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}

#define ARM_FPOPD(p, vrd, count)	arm_vpop ((void **)&p, vrd, count, 1)
#define ARM_FPOPS(p, vrd, count)	arm_vpop ((void **)&p, vrd, count, 0)

/*
 * Hint Instructions - NOP, DBG...
 */

typedef struct {
	uint32_t type:16;	/* Operation specific */
	uint32_t op1:3;		/* Operation part 1 */
	uint32_t op2:5;		/* Operation part 2 */
	uint32_t typea:4;	/* Operations specific */
} hint32_t;

#define OP_NOP32 0x8000f3af

static __THUMB_INLINE__ void
arm_hint32(void **code, int opr)
{
	uint32_t *x = *(uint32_t **) code;
	hint32_t *op = *(hint32_t **) code;
	*x = opr;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/*
 * Miscellaneous Control Instructions - DMB
 */

typedef struct {
	uint32_t type:16;	/* Operation specific */
	uint32_t option:4;	/* Option */
	uint32_t op:4;		/* OP - e.g. 5 = DMB */
	uint32_t typea:8;	/* Operations specific */
} mci32_t;

#define OP_DMB	0x8f50f3bf

static __THUMB_INLINE__ void
arm_mci(void **code, int opr, int option)
{
	uint32_t *x = *(uint32_t **) code;
	*x = opr;
	mci32_t *op = *(mci32_t **) code;
	op->option = option;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/*
 * Miscellaneous Control Instructions - Move from status to register
 */

typedef struct {
	uint16_t type;		/* '1111001111101111'b */
	uint16_t sr:8;		/* Special register */
	uint16_t rd:4;		/* Register d */
	uint16_t typea:4;	/* '1000'b */
} mrs32_t;

#define OP_MRS	0x8000f3ef

static __THUMB_INLINE__ void
arm_mrs(void **code, int rd, int sr)
{
        uint32_t *x = *(uint32_t **) code;
        mrs32_t *op = *(mrs32_t **) code;
        *x = OP_MRS;
        op->sr = sr;
        op->rd = rd;
        *code = (void *)((uintptr_t) *code + sizeof(*op));
}

/*
 * Miscellaneous Control Instructions - Move to status from register
 */

typedef struct {
	uint16_t rn:4;		/* Register n */
	uint16_t type:12;	/* '111100111110'b */
	uint16_t sr:8;		/* Special register */
	uint16_t fm:4;		/* Field mask */
	uint16_t typea:4;	/* '1000'b */
} msr32_t;

#define OP_MSR	0x8000f380

static __THUMB_INLINE__ void
arm_msr(void **code, int rn, int sr, int fm)
{
        uint32_t *x = *(uint32_t **) code;
        msr32_t *op = *(msr32_t **) code;
        *x = OP_MSR;
        op->sr = sr;
        op->rn = rn;
        op->fm = fm;
        *code = (void *)((uintptr_t) *code + sizeof(*op));
}

static __THUMB_INLINE__ void
arm_msr32(void **code, int sr, int rn)
{
	uint32_t *x = *(uint32_t **) code;
	msr32_t *op = *(msr32_t **) code;
	*x = OP_MSR;
	op->sr = sr;
	op->rn = rn;
	*code = (void *)((uintptr_t) *code + sizeof(*op));
}
	
/*
 * Instruction definition and encoding - Prototypes
 */
static __THUMB_INLINE__ void arm_adc_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_adc_register(void **code, int rd, int rn, int rm, int rot, int cond);
static __THUMB_INLINE__ void arm_add_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_add_register(void **code, int rd, int rn, int rm, int rot, int cond);
static __THUMB_INLINE__ void arm_add_sp_imm(void **code, int rd, int imm, int cond);
static __THUMB_INLINE__ void arm_add_sp_reg(void **code, int rm, int cond);
static __THUMB_INLINE__ void arm_add_pc_imm(void **code, int rd, int imm, int cond);
static __THUMB_INLINE__ void arm_asr_imm(void **code, int rd, int rn, int imm);
static __THUMB_INLINE__ void arm_asr_register(void **code, int rd, int rn, int rm);
static __THUMB_INLINE__ void arm_bkpt(void **code, int imm);
static __THUMB_INLINE__ void arm_branch(void **code, int offset);
static __THUMB_INLINE__ void arm_branch_cond(void **code, int cond, int offset);
static __THUMB_INLINE__ void arm_bic_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_branch_link(void **code, int offset, int cond);
static __THUMB_INLINE__ void arm_branch_link_x(void **code, int rm, int cond);
static __THUMB_INLINE__ void arm_branch_link_x_align(void **code, int rm);
static __THUMB_INLINE__ void arm_branch_long(void **code, int offset);
static __THUMB_INLINE__ void arm_branch_x(void **code, int rm, int cond);
static __THUMB_INLINE__ void arm_branch_x_align(void **code, int rm);
static __THUMB_INLINE__ void arm_cmn_imm(void **code, int rn, int imm);
static __THUMB_INLINE__ void arm_cmp_imm(void **code, int rn, int imm);
static __THUMB_INLINE__ void arm_cmp_register(void **code, int rd, int rm);
static __THUMB_INLINE__ void arm_dmb(void **code, int op);
static __THUMB_INLINE__ void arm_eor_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_eor_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_eor_register(void **code, int rd, int rn, int rm);
static __THUMB_INLINE__ void arm_get_cc(void **code, int rd, int cond);
static __THUMB_INLINE__ void arm_it(void **code, int cond);
static __THUMB_INLINE__ void arm_ite(void **code, int cond);
static __THUMB_INLINE__ void arm_itt(void **code, int cond);
static __THUMB_INLINE__ void arm_itee(void **code, int cond);
static __THUMB_INLINE__ void arm_itte(void **code, int cond);
static __THUMB_INLINE__ void arm_ldr_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_ldr_imm_cond(void **code, int rn, int rt, int imm, int cond);
static __THUMB_INLINE__ void arm_ldr_lit(void **code, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrb_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrb_lit(void **code, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrex_reg(void **code, int rt, int rn, int offset);
static __THUMB_INLINE__ void arm_ldrh_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrh_lit(void **code, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrsb_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrsb_lit(void **code, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrsh_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_ldrsh_lit(void **code, int rt, int imm);
static __THUMB_INLINE__ void arm_ldr_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_ldr_reg_shift(void **code, int rt, int rn, int rm, int shift);
static __THUMB_INLINE__ void arm_ldrb_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_ldrh_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_ldrsb_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_ldrsh_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_lsl_imm(void **code, int rd, int rn, int imm);
static __THUMB_INLINE__ void arm_lsr_imm(void **code, int rd, int rn, int imm);
static __THUMB_INLINE__ void arm_lsl_reg(void **code, int rd, int rn, int rs);
static __THUMB_INLINE__ void arm_lsr_reg(void **code, int rd, int rn, int rs);
static __THUMB_INLINE__ void arm_mla(void **code, int rd, int rn, int rm, int ra);
static __THUMB_INLINE__ void arm_mlas(void **code, int rd, int rn, int rm, int ra);
static __THUMB_INLINE__ void arm_mls(void **code, int rd, int rn, int rm, int ra);
static __THUMB_INLINE__ void arm_mlss(void **code, int rd, int rn, int rm, int ra);
static __THUMB_INLINE__ void arm_mov_imm(void **code, int rd, int imm, int cond);
static __THUMB_INLINE__ void arm_mov_imm_cond(void **code, int rd, int imm, int cond);
static __THUMB_INLINE__ void arm_mov_imm_rot(void **code, int rd, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_mov_reg(void **code, int rd, int rn, int cond);
static __THUMB_INLINE__ void arm_movt_imm(void **code, int rd, int imm);
static __THUMB_INLINE__ void arm_movw_imm(void **code, int rd, int imm);
static __THUMB_INLINE__ void arm_mrc(void **code, int coproc, int opc1, int rt, int crn, int crm, int opc2, int cond);
static __THUMB_INLINE__ void arm_mul_register(void **code, int rd, int rn, int rm);
static __THUMB_INLINE__ void arm_mvn_imm(void **code, int rd, int imm, int cond);
static __THUMB_INLINE__ void arm_mvn_reg(void **code, int rd, int rn, int cond);
static __THUMB_INLINE__ void arm_or_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_or_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_or_register(void **code, int rd, int rn, int rm);

static __THUMB_INLINE__ void arm_pop(void **code, int rl, int wb);
static __THUMB_INLINE__ void arm_push(void **code, int rl, int wb);
static __THUMB_INLINE__ void arm_ror_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_ror_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_ror_register(void **code, int rd, int rn, int rm);
static __THUMB_INLINE__ void arm_rsb_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_rsb_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_rsb_register(void **code, int rd, int rn, int rm, int cond);
static __THUMB_INLINE__ void arm_rsc_imm(void **code, int rd, int rn, int imm, int rot);
static __THUMB_INLINE__ void arm_sbc_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_sbc_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_sbc_register(void **code, int rd, int rn, int rm, int rot, int cond);
static __THUMB_INLINE__ void arm_sdiv(void **code, int rd, int rn, int rm);
static __THUMB_INLINE__ void arm_smull(void **code, int rdhi, int rdlo, int rn, int rm);
static __THUMB_INLINE__ void arm_stm(void **code, int rn, int rl);
static __THUMB_INLINE__ void arm_str_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_strb_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_strex_reg(void **code, int rd, int rt, int rn, int offset);
static __THUMB_INLINE__ void arm_strh_imm(void **code, int rd, int rt, int imm);
static __THUMB_INLINE__ void arm_str_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_strb_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_strh_reg(void **code, int rt, int rn, int rm);
static __THUMB_INLINE__ void arm_sub_imm(void **code, int rd, int rn, int imm, int cond);
static __THUMB_INLINE__ void arm_sub_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond);
static __THUMB_INLINE__ void arm_sub_register(void **code, int rd, int rn, int rm, int rot, int cond);
static __THUMB_INLINE__ void arm_sub_sp_reg(void **code, int rm, int cond);
static __THUMB_INLINE__ void arm_sub_sp_imm(void **code, int rd, int imm, int cond);
static __THUMB_INLINE__ void arm_tst_imm(void **code, int rn, int imm);
static __THUMB_INLINE__ void arm_tst_reg(void **code, int rn, int rm, int imm);
static __THUMB_INLINE__ void arm_udiv(void **code, int rd, int rn, int rm);
static __THUMB_INLINE__ void arm_umull(void **code, int rdhi, int rdlo, int rn, int rm);

/*
 * Instruction definition and encoding
 */

static __THUMB_INLINE__ void
arm_adc_imm(void **code, int rd, int rn, int imm, int cond)
{
	if (imm >= 0)
		arm_dpm32(code, rd, rn, imm, cond, 10);
	else
		arm_dpm32(code, rd, rn, -imm, cond, 13);
}

#define ARM_ADC_REG_IMM8(p, rd, rn, imm)	arm_adc_imm((void **) &p, rd, rn, imm, 0)
#define ARM_ADCS_REG_IMM8(p, rd, rn, imm)	arm_adc_imm((void **) &p, rd, rn, imm, 1)

static __THUMB_INLINE__ void
arm_adc_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 10);
}

#define ARM_ADCS_REG_IMM(p, rd, rn, imm, rot)	arm_adc_imm_rot((void **) &p, rd, rn, imm, rot, 1)

static __THUMB_INLINE__ void
arm_adc_register(void **code, int rd, int rn, int rm, int rot, int cond)
{
	if ((rd < 8) && (rd == rn) && (rm < 8) && (rot == 0) && (cond != 0)) 
		arm_dpri16(code, rd, rm, 5);
	else 
		arm_dpc32(code, rd, rn, rm, rot, cond, 10, 0);
}

#define ARM_ADC_REG_REG(p, rd, rn, rm)	arm_adc_register((void **) &p, rd, rn, rm, 0, 0)
#define ARM_ADCS_REG_REG(p, rd, rn, rm)	arm_adc_register((void **) &p, rd, rn, rm, 0, 1)

static __THUMB_INLINE__ void
arm_add_imm(void **code, int rd, int rn, int imm, int cond)
{
	if (imm < 0)
		arm_sub_imm(code, rd, rn, imm, cond);
	else {
		if (rn == ARMREG_SP) 
			arm_add_sp_imm(code, rd, imm, cond);
		else if (rd == ARMREG_SP) {
			arm_mov_reg(code, rd, rn, cond);
			arm_add_sp_imm(code, rd, imm, cond);
		} else {
			if ((cond != 0) && (rd < 8) && (rn < 8)) {
				if (rd == rn) {
					if ((imm >= 0) && (imm < 256)) 
						arm_ascmi16(code, rd, imm, 6);
					else
						arm_dpm32(code, rd, rn, imm, cond, 8);
				} else {
					if ((imm >= 0) && (imm < 8)) 
						arm_asi16(code, rd, rn, imm, 14);
					else
						arm_dpm32(code, rd, rn, imm, cond, 8);
				}
			} else
				arm_dpm32(code, rd, rn, imm, 0, 8);
		}
	}
}

#define ARM_ADD_REG_IMM8(p, rd, rn, imm)	arm_add_imm((void **) &p, rd, rn, imm, 1)

static __THUMB_INLINE__ void
arm_add_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	if (rot == 0) 
		arm_add_imm(code, rd, rn, imm, cond);
	else {
		/* 
		 * Allow for ARM_ADD_REG_IMM (p, ARMREG_SP, x, i, rot) which is
		 * valid for ARM but not for thumb 
		 */
		if (rd == ARMREG_SP) {
			arm_dpm32_rot(code, ARMREG_IP, rn, imm, rot, 0, 8);
			arm_mov_reg (code, rd, ARMREG_IP, cond);
		} else 
			arm_dpm32_rot(code, rd, rn, imm, rot, cond, 8);
	}
}

#define ARM_ADD_REG_IMM(p, rd, rn, imm, rot)	arm_add_imm_rot((void **) &p, rd, rn, imm, rot, 0)
#define ARM_ADDS_REG_IMM(p, rd, rn, imm, rot)	arm_add_imm_rot((void **) &p, rd, rn, imm, rot, 1)

static __THUMB_INLINE__ void
arm_add_register(void **code, int rd, int rn, int rm, int rot, int cond)
{
	if ((rd < 8) && (rn < 8) && (rm < 8) && (rot == 0) && (cond == 0)) 
		arm_asr16(code, rd, rn, rm, 0);
	else if ((rd == rn) && (rot == 0) && (cond == 0))
		arm_sdp16 (code, rd, rm, 0);
	else if (rd != ARMREG_SP) 
		arm_dpc32(code, rd, rn, rm, rot, cond, 8, 0);
	else {
		if (rn == ARMREG_SP)
			arm_dpc32(code, rd, rn, rm, rot, cond, 8, 0);
		else {
			if (rm != ARMREG_IP)
				arm_mov_reg(code, ARMREG_IP, rm, 0);
			arm_sdp16(code, ARMREG_IP, rn, 0);
			arm_mov_reg(code, ARMREG_SP, ARMREG_IP, cond);
		}
	}
}

#define ARM_ADD_REG_REG(p, rd, rn, rm)		arm_add_register((void **) &p, rd, rn, rm, 0, 0)
#define ARM_ADDS_REG_REG(p, rd, rn, rm)		arm_add_register((void **) &p, rd, rn, rm, 0, 1)

static __THUMB_INLINE__ void
arm_add_sp_imm(void **code, int rd, int imm, int cond)
{
	if (rd == ARMREG_SP) {
		if (imm % 4 == 0) {
			if ((imm >= 0) && (imm < 511))
				arm_ajsp16(code, (imm / 4), 0);
			else
				arm_dpm32(code, rd, ARMREG_SP, imm, cond, 8);
		} else { 
			arm_dpm32(code, rd, ARMREG_SP, imm, cond, 8);
		}
	} else if (cond != 0) {
		if ((rd < 8) && (imm % 4 == 0) && (imm >= 0) && (imm < 1024)) {
			arm_asp16(code, rd, (imm / 4), 1);
		} else {
			arm_dpm32(code, rd, ARMREG_SP, imm, cond, 8);
		}
	} else {
		arm_dpm32(code, rd, ARMREG_SP, imm, 0, 8);
	}
}

static __THUMB_INLINE__ void
arm_add_sp_reg(void **code, int rm, int cond)
{
	if (cond != 0)
		arm_sdp16(code, ARMREG_SP, rm, 0);
	else
		arm_dpc32(code, ARMREG_SP, ARMREG_SP, rm, 0, 0, 8, 0);
}			

static __THUMB_INLINE__ void
arm_adr_imm(void **code, int rd, int imm)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		imm += 2;
 
	if ((rd < 8) && (imm >= 0) && (imm < 256))
		arm_asp16(code, rd, imm, 0);
	else
		arm_asp32(code, rd, 15, imm, 0);
}			

#define ARM_ADR_IMM(p, r, imm)		arm_adr_imm((void **) &p, r, imm)

static __THUMB_INLINE__ void
arm_add_pc_imm(void **code, int rd, int imm, int cond)
{
	if (cond != 0) {
		if (imm % 4 == 0) {
			if ((imm >=0) && (imm < 512))
				arm_ajsp16(code, (imm / 4), 1);
			else {
				if (imm >= 0)
					arm_dpm32(code, rd, ARMREG_PC, imm, cond, 8);
				else
					arm_dpm32(code, rd, ARMREG_PC, -imm, cond, 13);
			}
		} else 
			arm_dpm32(code, rd, ARMREG_PC, imm, cond, 8);
	} else {
		if (imm >= 0)
			arm_dpm32(code, rd, ARMREG_PC, imm, 0, 8);
		else
			arm_dpm32(code, rd, ARMREG_PC, -imm, 0, 13);
	}
}

static __THUMB_INLINE__ void
arm_and_imm(void **code, int rd, int rn, int imm, int cond)
{
	arm_dpm32(code, rd, rn, imm, cond, 0);
}

#define ARM_AND_REG_IMM8(p, rd, rn, imm)        arm_and_imm((void **) &p, rd, rn, imm, 1)

static __THUMB_INLINE__ void
arm_and_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 0);
}

#define ARM_AND_REG_IMM(p, rd, rn, imm, rot)	arm_and_imm_rot((void **) &p, rd, rn, imm, rot, 1)

static __THUMB_INLINE__ void
arm_and_register(void **code, int rd, int rn, int rm)
{
	if ((rd < 8) && (rn < 8) && (rm < 8)) {
		if (rd == rm) 
			arm_dpri16(code, rd, rn, 0);
		else if (rd == rn)
			arm_dpri16(code, rd, rm, 0);
		else
			arm_dpc32(code, rd, rn, rm, 0, 0, 0, 0);
	} else
		arm_dpc32(code, rd, rn, rm, 0, 0, 0, 0);
}

#define ARM_AND_REG_REG(p, rd, rn, rm)	arm_and_register((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_asr_imm(void **code, int rd, int rm, int imm)
{
	if ((rd < 8) && (rm < 8))
		arm_simr16(code, rd, rm, imm, 2);
	else if (rm == ARMREG_SP) {
		arm_mov_reg(code, rd, rm, 1);
		if (rd < 8)
			arm_simr16(code, rd, rd, imm, 2);
		else
			arm_dpc32(code, rd, 15, rd, imm, 0, 2, 2);
	} else
		arm_dpc32(code, rd, 15, rm, imm, 0, 2, 2);
}

#define ARM_SAR_IMM(p, rd, rm, imm)	arm_asr_imm((void **)&p, rd, rm, imm)

static __THUMB_INLINE__ void
arm_asr_register(void **code, int rd, int rn, int rm)
{
	if ((rd < 8) && (rn < 8) && (rm < 8)) {
		if (rd == rm) 
			arm_dpri16(code, rd, rn, 4);
		else if (rd == rn)
			arm_dpri16(code, rd, rm, 4);
		else
			arm_dpc32(code, rd, rn, rm, 0, 0, 4, 0);
	} else
		arm_dpc32(code, rd, rn, rm, 0, 0, 4, 0);
}

#define ARM_SAR_REG(p, rd, rn, rs)	arm_asr_register((void **)&p, rd, rn, rs)

static __THUMB_INLINE__ void
arm_bic_imm(void **code, int rd, int rn, int imm, int cond)
{
	arm_dpm32(code, rd, rn, imm, cond, 1);
}

#define ARM_BIC_REG_IMM8(p, rd, rn, imm)	arm_bic_imm((void **) &p, rd, rn, imm, 0)

static __THUMB_INLINE__ void 
arm_bkpt(void **code, int imm)
{
	arm_sbp16(code, imm);
}

#define ARM_DBRK(p)		arm_bkpt((void **) &p, 0)

static __THUMB_INLINE__ void
arm_branch(void **code, int offset)
{
	if (offset == 0) 
		arm_brl32(code, 4, 0);
	else {
		if (ARM_IS_IMM8(offset))
			arm_ub16(code, offset);
		else {
			uintptr_t loc = ((uintptr_t) *code % 4);

			if (loc != 0)
				offset += 2;

			arm_brl32(code, offset, 0);
		}
	}
}

#define ARM_B(p, offset)		arm_branch((void **) &p, offset)

static __THUMB_INLINE__ void
arm_branch_cond(void **code, int cond, int offset)
{
	if (cond == ARMCOND_AL)
		arm_branch(code, offset);

	if (offset == 0) 
		arm_cbr32(code, cond, 4);
	else {
		if (ARM_IS_IMM8(offset))
			arm_cbr16(code, cond, offset);
		else {
			uintptr_t loc = ((uintptr_t) *code % 4);

			if (loc != 0)
				offset += 2;

			arm_cbr32(code, cond, offset);
		}
	}
}

#define ARM_B_COND(p, cond, offset) 	arm_branch_cond((void **) &p, cond, offset)

static __THUMB_INLINE__ void
arm_branch_cond_long(void **code, int cond, int offset)
{
	if (cond == ARMCOND_AL)
		arm_branch_long(code, offset);

	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		offset += 2;

	arm_cbr32(code, cond, offset);
}

#define ARM_B_COND_LONG(p, cond, offset)	arm_branch_cond_long((void **) &p, cond, offset)

static __THUMB_INLINE__ void
arm_branch_link(void **code, int offset, int cond)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		offset += 2;

	if (cond != ARMCOND_AL)
		arm_it(code, cond);

	arm_brl32(code, offset, 1);
}

#define ARM_BL(p, offset)		arm_branch_link((void **) &p, offset, ARMCOND_AL)
#define ARM_BL_COND(p, cond, offset)	arm_branch_link((void **) &p, offset, cond)

static __THUMB_INLINE__ void
arm_branch_link_x(void **code, int rm, int cond)
{
	if (cond != ARMCOND_AL)
		arm_itt(code, cond);

	arm_or_imm(code, rm, rm, 1, 0);
	arm_bxis16(code, rm, 1);
}

#define ARM_BLX_REG(p, rm)		arm_branch_link_x((void **) &p, rm, ARMCOND_AL)
#define ARM_BLX_REG_COND(p, cond, rm)	arm_branch_link_x((void **) &p, rm, cond)

static __THUMB_INLINE__ void
arm_branch_link_x_align(void **code, int rm)
{
	arm_or_imm(code, rm, rm, 1, 0);
	if (((uintptr_t) *code % 4) == 0)
		arm_nop16(code, 0);
	arm_bxis16(code, rm, 1);
}

#define ARM_BLX_REG_ALIGN(p, rm)	arm_branch_link_x_align((void **) &p, rm)

static __THUMB_INLINE__ void
arm_branch_long(void **code, int offset)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		offset += 2;

	arm_brl32(code, offset, 0);
}

#define ARM_B_LONG(p, offset)		arm_branch_long((void **) &p, offset)

static __THUMB_INLINE__ void
arm_branch_x(void **code, int rm, int cond)
{
	if (cond != ARMCOND_NV) {
		arm_or_imm(code, rm, rm, 1, 0);
		if (cond != ARMCOND_AL) 
			arm_it(code, cond);
		arm_bxis16(code, rm, 0);
	}
}

#define ARM_BX(p, rm) 			arm_branch_x((void **) &p, rm, ARMCOND_AL)
#define ARM_BX_COND(p, rm, cond)	arm_branch_x((void **) &p, rm, cond)

static __THUMB_INLINE__ void
arm_branch_x_align(void **code, int rm)
{
	arm_or_imm(code, rm, rm, 1, 0);
	if (((uintptr_t) *code % 4) == 0)
		arm_nop16(code, 0);
	arm_bxis16(code, rm, 0);
}

#define ARM_BX_ALIGN(p, rm) 		arm_branch_x_align((void **) &p, rm)

static __THUMB_INLINE__ void
arm_cmn_imm(void **code, int rn, int imm)
{
	arm_dpm32(code, 15, rn, imm, 1, 8);
}

#define ARM_CMN_REG_IMM8(p, rn, imm)	arm_cmn_imm((void **) &p, rn, imm)

static __THUMB_INLINE__ void 
arm_cmp_imm(void **code, int rn, int imm)
{
	if (imm < 0)
		arm_cmn_imm(code, rn, -imm);

	else if ((rn < 8) && (imm >= 0) && (imm < 256))
		arm_ascmi16(code, rn, imm, 5);
	else
		arm_dpm32(code, 15, rn, imm, 1, 13);
}

#define ARM_CMP_REG_IMM8(p, rn, imm)		arm_cmp_imm((void **) &p, rn, imm)

static __THUMB_INLINE__ void 
arm_cmp_imm_rot(void **code, int rn, int imm, int rot)
{
	arm_dpm32_rot(code, 15, rn, imm, rot, 1, 13);
}

#define ARM_CMP_REG_IMM(p, rn, imm, rot)	arm_cmp_imm_rot((void **) &p, rn, imm, rot)

static __THUMB_INLINE__ void
arm_cmp_register(void **code, int rd, int rm)
{
	if ((rd < 8) && (rm < 8)) 
		arm_dpri16(code, rd, rm, 10);
	else
		arm_sdp16(code, rd, rm, 1);
}

#define ARM_CMP_REG_REG(p, rd, rm)		arm_cmp_register((void **) &p, rd, rm)

static __THUMB_INLINE__ void
arm_dmb(void **code, int op)
{
	arm_mci(code, OP_DMB, op);
}

#define ARM_DMB(p, op)				arm_dmb((void **) &p, op)

static __THUMB_INLINE__ void
arm_eor_imm(void **code, int rd, int rn, int imm, int cond)
{
	arm_dpm32(code, rd, rn, imm, cond, 4);
}

#define ARM_EOR_REG_IMM8(p, rd, rn, imm) 	arm_eor_imm((void **) &p, rd, rn, imm, 0)

static __THUMB_INLINE__ void
arm_eor_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 4);
}

#define ARM_EOR_REG_IMM(p, rd, rn, imm, rot) 	arm_eor_imm_rot((void **) &p, rd, rn, imm, rot, 0)

static __THUMB_INLINE__ void
arm_eor_register(void **code, int rd, int rn, int rm)
{
	if ((rd < 8) && (rn < 8) && (rm < 8)) {
		if (rd == rm) 
			arm_dpri16(code, rd, rn, 1);
		else if (rd == rn)
			arm_dpri16(code, rd, rm, 1);
		else
			arm_dpc32(code, rd, rn, rm, 0, 0, 4, 0);
	} else
		arm_dpc32(code, rd, rn, rm, 0, 0, 4, 0);
}

#define ARM_EOR_REG_REG(p, rd, rn, rm)	arm_eor_register((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_get_cc(void **code, int rd, int cond)
{
	arm_ite(code, cond);
	arm_mov_imm(code, rd, 1, 0);
	arm_mov_imm(code, rd, 0, 0);
}

#define ARM_GET_CC(p, rd, cond)	arm_get_cc((void **) &p, rd, cond)

static __THUMB_INLINE__ void 
arm_it(void **code, int cond)
{
	arm_ift16(code, cond, 0x8);
}
	
static __THUMB_INLINE__ void 
arm_ite(void **code, int cond)
{
	if (cond != ARMCOND_AL) {
		int mask = ((~cond & 0x1) << 3) | 0x4;
		arm_ift16(code, cond, mask);
	}
}

static __THUMB_INLINE__ void 
arm_itt(void **code, int cond)
{
	if (cond != ARMCOND_AL) {
		int mask = ((cond & 0x1) << 3) | 0x4;
		arm_ift16(code, cond, mask);
	}
}
	
static __THUMB_INLINE__ void 
arm_itee(void **code, int cond)
{
	if (cond != ARMCOND_AL) {
		int mask = ((~cond & 0x1) << 3) |
			   ((~cond & 0x1) << 2) |
			   0x2;
		arm_ift16(code, cond, mask);
	}
}
	
static __THUMB_INLINE__ void 
arm_itte(void **code, int cond)
{
	if (cond != ARMCOND_AL) {
		int mask = ((cond & 0x1) << 3) |
			   ((~cond & 0x1) << 2) |
			   0x2;
		arm_ift16(code, cond, mask);
	}
}
	
static __THUMB_INLINE__ void
arm_ldm(void **code, int rn, int rl)
{
	if ((rn < 8) && (rl < 256))
		arm_lsm16(code, rn, rl, 1);
	else
		arm_lsm32(code, rn, rl, 1, 1, 0);
}

#define ARM_LDM(p, rn, rl) arm_ldm((void **)&p, rn, rl)

static __THUMB_INLINE__ void
arm_ldr_imm(void **code, int rn, int rt, int imm)
{
	if (rt == ARMREG_PC)
		arm_ldr_lit(code, rn, imm);
	else {
		if ((rn < 8) && (rt < 8) && (imm < 128) && ((imm % 4) == 0)) {
			imm >>= 2;
			arm_lswbi16(code, rn, rt, imm, 1, 0);
		} else {
			if ((imm > -256) && (imm < 256))
				arm_lsrn32(code, rn, rt, 1, 2, imm, 0, 0, 0);
			else if ((imm >= 0) && (imm < 4096))
				arm_lspr32(code, rn, rt, 1, 2, 1, imm, 0);
			else
				assert((imm > -256) && (imm < 4096));
		}
	}
}
	
#define ARM_LDR_IMM(p, rn, rt, offset) arm_ldr_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_ldr_imm_cond(void **code, int rn, int rt, int imm, int cond)
{
	if (cond != ARMCOND_NV) {
		if (cond != ARMCOND_AL)
			arm_it(code, cond);
		arm_ldr_imm(code, rn, rt, imm);
	}
}

#define ARM_LDR_IMM_COND(p, rn, rt, offset, cond)	arm_ldr_imm_cond((void **) &p, rn, rt, offset, cond)

static __THUMB_INLINE__ void
arm_ldrb_imm(void **code, int rn, int rt, int imm)
{
	if (rt == ARMREG_PC)
		arm_ldrb_lit(code, rn, imm);
	else {
		if ((rn < 8) && (rt < 8) && (imm >= 0) && (imm < 32)) {
			arm_lswbi16(code, rn, rt, imm, 1, 1);
		} else {
			if ((imm > -256) && (imm < 256))  
				arm_lsrn32(code, rn, rt, 1, 0, imm, 0, 0, 0);
			else if ((imm >= 0) && (imm < 4096))
				arm_lspr32(code, rn, rt, 1, 0, 1, imm, 0);
			else
				assert((imm > -256) && (imm < 4096));
		}
	}
}
	
#define ARM_LDRB_IMM(p, rn, rt, offset) arm_ldrb_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_ldrex_reg(void **code, int rt, int rn, int offset)
{
	arm_lsxt32(code, 15, rt, rn, 1, offset);
}

#define ARM_LDREX_REG(p, rt, rn)	arm_ldrex_reg((void **) &p, rt, rn, 0)

static __THUMB_INLINE__ void
arm_ldrh_imm(void **code, int rn, int rt, int imm)
{
	if (rt == ARMREG_PC)
		arm_ldrh_lit(code, rn, imm);
	else {
		if ((rn < 8) && (rt < 8) && (imm >= 0) && (imm < 64) && ((imm % 2) == 0)) {
			imm >>= 1;
			arm_lshi16(code, rn, rt, imm, 1);
		} else {
			if ((imm > -256) && (imm < 256))
				arm_lsrn32(code, rn, rt, 1, 1, imm, 0, 0, 0);
			else if ((imm >= 0) && (imm < 4096))
				arm_lspr32(code, rn, rt, 1, 1, 1, imm, 0);
			else
				assert((imm > -256) && (imm < 4096));
		}
	}
}
	
#define ARM_LDRH_IMM(p, rn, rt, offset) arm_ldrh_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_ldrsb_imm(void **code, int rn, int rt, int imm)
{
	if (rt == ARMREG_PC)
		arm_ldrsb_lit(code, rn, imm);
	else {
		if ((imm > -256) && (imm < 256))
			arm_lsrn32(code, rn, rt, 1, 0, imm, 1, 0, 0);
		else if ((imm >= 0) && (imm < 4096))
			arm_lspr32(code, rn, rt, 1, 0, 1, imm, 1);
		else
			assert((imm > -256) && (imm < 4096));
	}
}
	
#define ARM_LDRSB_IMM(p, rn, rt, offset) arm_ldrsb_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_ldrsh_imm(void **code, int rn, int rt, int imm)
{
	if (rt == ARMREG_PC)
		arm_ldrsh_lit(code, rn, imm);
	else {
		if ((imm > -256) && (imm < 256))
			arm_lsrn32(code, rn, rt, 1, 1, imm, 1, 0, 0);
		else if ((imm >= 0) && (imm < 4096))
			arm_lspr32(code, rn, rt, 1, 1, 1, imm, 1);
		else
			assert((imm > -256) && (imm < 4096));
	}
}
	
#define ARM_LDRSH_IMM(p, rn, rt, offset) arm_ldrsh_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_ldr_lit(void **code, int rn, int imm)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		imm += 2;
	if ((imm > -4096) && (imm < 0))
		arm_lspr32(code, rn, ARMREG_PC, 1, 2, 0, -imm, 0);
	else if ((imm > -4096) && (imm < 4096))
		arm_lspr32(code, rn, ARMREG_PC, 1, 2, 1, imm, 0);
	else
		assert((imm > -256) && (imm < 4096));
}

#define ARM_LDR_LIT(p, rt, offset)	arm_ldr_lit((void **) &p, rt, offset)
	
static __THUMB_INLINE__ void
arm_ldrb_lit(void **code, int rn, int imm)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		imm += 2;
	if ((imm > -4096) && (imm < 0))
		arm_lspr32(code, rn, ARMREG_PC, 1, 0, 0, -imm, 0);
	else if ((imm > -4096) && (imm < 4096))
		arm_lspr32(code, rn, ARMREG_PC, 1, 0, 1, imm, 0);
	else
		assert((imm > -256) && (imm < 4096));
}

#define ARM_LDRB_LIT(p, rt, offset)	arm_ldrb_lit((void **) &p, rt, offset)
	
static __THUMB_INLINE__ void
arm_ldrh_lit(void **code, int rn, int imm)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		imm += 2;
	if ((imm > -4096) && (imm < 0))
		arm_lspr32(code, rn, ARMREG_PC, 1, 1, 0, -imm, 0);
	else if ((imm > -4096) && (imm < 4096))
		arm_lspr32(code, rn, ARMREG_PC, 1, 1, 1, imm, 0);
	else
		assert((imm > -256) && (imm < 4096));
}

#define ARM_LDRH_LIT(p, rt, offset)	arm_ldrh_lit((void **) &p, rt, offset)
	
static __THUMB_INLINE__ void
arm_ldrsb_lit(void **code, int rn, int imm)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		imm += 2;
	if ((imm > -4096) && (imm < 0))
		arm_lspr32(code, rn, ARMREG_PC, 1, 0, 0, -imm, 1);
	else if ((imm > -4096) && (imm < 4096))
		arm_lspr32(code, rn, ARMREG_PC, 1, 0, 1, imm, 1);
	else
		assert((imm > -256) && (imm < 4096));
}

#define ARM_LDRSB_LIT(p, rt, offset)	arm_ldrsb_lit((void **) &p, rt, offset)
	
static __THUMB_INLINE__ void
arm_ldrsh_lit(void **code, int rn, int imm)
{
	uintptr_t loc = ((uintptr_t) *code % 4);

	if (loc != 0)
		imm += 2;
	if ((imm > -4096) && (imm < 0))
		arm_lspr32(code, rn, ARMREG_PC, 1, 1, 0, -imm, 1);
	else if ((imm > -4096) && (imm < 4096))
		arm_lspr32(code, rn, ARMREG_PC, 1, 1, 1, imm, 1);
	else
		assert((imm > -256) && (imm < 4096));
}

#define ARM_LDRSH_LIT(p, rt, offset)	arm_ldrsh_lit((void **) &p, rt, offset)
	
static __THUMB_INLINE__ void
arm_ldr_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 4);
	else
		arm_lsro32(code, rt, rn, rm, 1, 2, 0, 0);
}

#define ARM_LDR_REG_REG(p, rt, rn, rm)	 arm_ldr_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_ldr_reg_shift(void **code, int rt, int rn, int rm, int shift)
{
	arm_lsro32(code, rt, rn, rm, 1, 2, 0, shift);
}

#define ARM_LDR_REG_REG_SHIFT(p, rt, rn, rm, st, sa)	 arm_ldr_reg_shift((void **) &p, rt, rn, rm, sa)

static __THUMB_INLINE__ void
arm_ldrb_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 6);
	else
		arm_lsro32(code, rt, rn, rm, 1, 0, 0, 0);
}

#define ARM_LDRB_REG_REG(p, rt, rn, rm)	 arm_ldrb_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_ldrh_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 5);
	else
		arm_lsro32(code, rt, rn, rm, 1, 1, 0, 0);
}

#define ARM_LDRH_REG_REG(p, rt, rn, rm)	 arm_ldrh_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_ldrsb_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 3);
	else
		arm_lsro32(code, rt, rn, rm, 1, 0, 1, 0);
}

#define ARM_LDRSB_REG_REG(p, rt, rn, rm)	 arm_ldrsb_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_ldrsh_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 7);
	else
		arm_lsro32(code, rt, rn, rm, 1, 1, 1, 0);
}

#define ARM_LDRSH_REG_REG(p, rt, rn, rm)	 arm_ldrsh_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_lsl_imm(void **code, int rd, int rm, int imm)
{
	if ((rd < 8) && (rm < 8))
		arm_simr16(code, rd, rm, imm, 0);
	else if (rm == ARMREG_SP) {
		arm_mov_reg(code, rd, rm, 1);
		if (rd < 8)
			arm_simr16(code, rd, rd, imm, 0);
		else
			arm_dpc32(code, rd, 15, rd, imm, 0, 2, 0);
	} else 
		arm_dpc32(code, rd, 15, rm, imm, 0, 2, 0);
}

#define ARM_SHL_IMM(p, rd, rm, imm) 	arm_lsl_imm((void **) &p, rd, rm, imm)

static __THUMB_INLINE__ void
arm_lsl_reg(void **code, int rd, int rn, int rs)
{
	if ((rd < 8) && (rn < 8) && (rs < 8)) {
		if (rd == rn) 
			arm_dpri16(code, rd, rs, 2);
		else
			arm_rcs32(code, rd, rn, rs, 0, 0);
	} else {
		arm_rcs32(code, rd, rn, rs, 0, 0);
	}
}

#define ARM_SHL_REG(p, rd, rn, rs)	arm_lsl_reg((void **)&p, rd, rn, rs)

static __THUMB_INLINE__ void
arm_lsr_imm(void **code, int rd, int rm, int imm)
{
	if ((rd < 8) && (rm < 8))
		arm_simr16(code, rd, rm, imm, 1);
	else if (rm == ARMREG_SP) {
		arm_mov_reg(code, rd, rm, 1);
		if (rd < 8)
			arm_simr16(code, rd, rm, imm, 1);
		else
			arm_dpc32(code, rd, 15, rd, imm, 0, 2, 1);
	} else
		arm_dpc32(code, rd, 15, rm, imm, 0, 2, 1);
}

#define ARM_SHR_IMM(p, rd, rm, imm) 	arm_lsr_imm((void **) &p, rd, rm, imm)

static __THUMB_INLINE__ void
arm_lsr_reg(void **code, int rd, int rn, int rs)
{
	if ((rd < 8) && (rn < 8) && (rs < 8)) {
		if (rd == rn) 
			arm_dpri16(code, rd, rs, 3);
		else
			arm_rcs32(code, rd, rn, rs, 2, 0);
	} else {
		arm_rcs32(code, rd, rn, rs, 2, 0);
	}
}

#define ARM_SHR_REG(p, rd, rn, rs)	arm_lsr_reg((void **)&p, rd, rn, rs)

static __THUMB_INLINE__ void
arm_mcr(void **code, int coproc, int opc1, int rt, int crn, int crm, int opc2, int cond)
{
	if (cond != ARMCOND_AL)
		arm_it(code, cond);
	arm_mrt32(code, coproc, 0, 0, crn, rt, coproc, opc1, opc2);
}

#define ARM_MCR_COND(p, coproc, opc1, rt, crn, crm, opc2, cond) arm_mcr((void **) &p, coproc, opc1, rt, crn, crm, opc2, cond)
#define ARM_MCR(p, coproc, opc1, rt, crn, crm, opc2)		arm_mcr((void **) &p, coproc, opc1, rt, crn, crm, opc2, ARMCOND_AL)

static __THUMB_INLINE__ void
arm_mla(void **code, int rd, int rn, int rm, int ra)
{
	arm_msa32(code, rd, rn, rm, ra, 0, 0);
}

#define ARM_MLA(p, rd, rn, rm, ra)	arm_mla((void **) &p, rd, rn, rm, ra)

static __THUMB_INLINE__ void
arm_mlas(void **code, int rd, int rn, int rm, int ra)
{
	arm_mla(code, rd, rn, rm, ra);
	arm_tst_imm(code, rd, 0);
}

#define ARM_MLAS(p, rd, rn, rm, ra)	arm_mlas((void **) &p, rd, rn, rm, ra)

static __THUMB_INLINE__ void
arm_mls(void **code, int rd, int rn, int rm, int ra)
{
	arm_msa32(code, rd, rn, rm, ra, 0, 1);
}

#define ARM_MLS(p, rd, rn, rm, ra)	arm_mls((void **) &p, rd, rn, rm, ra)

static __THUMB_INLINE__ void
arm_mlss(void **code, int rd, int rn, int rm, int ra)
{
	arm_mla(code, rd, rn, rm, ra);
	arm_tst_imm(code, rd, 1);
}

#define ARM_MLAS(p, rd, rn, rm, ra)	arm_mlas((void **) &p, rd, rn, rm, ra)

static __THUMB_INLINE__ void
arm_mov_imm(void **code, int rd, int imm, int cond)
{
	if ((rd < 8) && (imm < 256)) 
		arm_ascmi16(code, rd, imm, 4);
	else {
		arm_dpm32(code, rd, 15, imm, cond, 2);
	}
}

#define ARM_MOV_REG_IMM8(p, rd, imm)		arm_mov_imm((void **) &p, rd, imm, 0)

static __THUMB_INLINE__ void
arm_mov_imm_cond(void **code, int rd, int imm, int cond)
{
	if (cond != ARMCOND_NV) {
		arm_it(code, cond);

		if ((rd < 8) && (imm < 256)) 
			arm_ascmi16(code, rd, imm, 4);
		else 
			arm_dpm32(code, rd, 15, imm, 0, 2);
	}
}

#define ARM_MOV_REG_IMM8_COND(p, rd, imm, cond)	arm_mov_imm_cond((void **) &p, rd, imm, cond)

static __THUMB_INLINE__ void
arm_mov_imm_rot(void **code, int rd, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, 15, imm, rot, 0, 2);
}

#define ARM_MOV_REG_IMM(p, rd, imm, rot)	arm_mov_imm_rot((void **) &p, rd, imm, rot, 0)

static __THUMB_INLINE__ void
arm_mov_reg(void **code, int rd, int rn, int cond)
{
	if ((rd < 8) && (rn < 8))
		arm_simr16(code, rd, rn, 0, 0);
	else
		arm_sdp16(code, rd, rn, 2);
}

#define ARM_MOV_REG_REG(p, rd, rn)		arm_mov_reg((void **) &p, rd, rn, 1)

static __THUMB_INLINE__ void
arm_movt_imm(void **code, int rd, int imm)
{
	arm_mvp32(code, rd, imm, 4);
}

#define ARM_MOVT_REG_IMM(p, rd, imm)	arm_movt_imm((void **) &p, rd, imm)

static __THUMB_INLINE__ void
arm_movw_imm(void **code, int rd, int imm)
{
	arm_mvp32(code, rd, imm, 0);
}

#define ARM_MOVW_REG_IMM(p, rd, imm)	arm_movw_imm((void **) &p, rd, imm)

static __THUMB_INLINE__ void
arm_mrc(void **code, int coproc, int opc1, int rt, int crn, int crm, int opc2, int cond)
{
	if (cond != ARMCOND_AL)
		arm_it(code, cond);
	arm_mrt32(code, coproc, 0, 1, crn, rt, coproc, opc1, opc2);
}

#define ARM_MRC_COND(p, coproc, opc1, rt, crn, crm, opc2, cond) arm_mrc((void **) &p, coproc, opc1, rt, crn, crm, opc2, cond)
#define ARM_MRC(p, coproc, opc1, rt, crn, crm, opc2)		arm_mrc((void **) &p, coproc, opc1, rt, crn, crm, opc2, ARMCOND_AL)

static __THUMB_INLINE__ void
arm_mul_register(void **code, int rd, int rn, int rm)
{
	if ((rd < 8) && (rn < 8) && (rm == rd))
		arm_dpri16(code, rd, rn, 13);
	else 
		arm_msa32(code, rd, rn, rm, 15, 0, 0);
}

#define ARM_MUL_REG_REG(p, rd, rn, rm)	arm_mul_register((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_mvn_imm(void **code, int rd, int imm, int cond)
{
	arm_dpm32(code, rd, 15, imm, 0, 3);
}

#define ARM_MVN_REG_IMM8(p, rd, imm)	arm_mvn_imm((void **) &p, rd, imm, 0)

static __THUMB_INLINE__ void
arm_mvn_imm_rot(void **code, int rd, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, 15, imm, rot, cond, 3);
}

#define ARM_MVN_REG_IMM(p, rd, imm, rot)	arm_mvn_imm_rot((void **) &p, rd, imm, rot, 0)

static __THUMB_INLINE__ void
arm_mvn_reg(void **code, int rd, int rn, int cond)
{
	if ((rd < 8) && (rn < 8))
		arm_dpri16(code, rd, rn, 15);
	else
		arm_dpc32(code, rd, 15, rn, 0, cond, 3, 0);
}

#define ARM_MVN_REG_REG(p, rd, rn)	arm_mvn_reg((void **) &p, rd, rn, 1)

#define ARM_NOP(p)	arm_hint32((void **) &p, OP_NOP32)

static __THUMB_INLINE__ void
arm_or_imm(void **code, int rd, int rn, int imm, int cond)
{
	arm_dpm32(code, rd, rn, imm, cond, 2);
}

#define ARM_ORR_REG_IMM8(p, rd, rn, imm)        arm_or_imm((void **) &p, rd, rn, imm, 0)

static __THUMB_INLINE__ void
arm_or_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 2);
}

#define ARM_ORR_REG_IMM(p, rd, rn, imm, rot)	arm_or_imm_rot((void **) &p, rd, rn, imm, rot, 0)

static __THUMB_INLINE__ void
arm_or_register(void **code, int rd, int rn, int rm)
{
	if ((rd < 8) && (rn < 8) && (rm < 8)) {
		if (rd == rm) 
			arm_dpri16(code, rd, rn, 12);
		else if (rd == rn)
			arm_dpri16(code, rd, rm, 12);
		else
			arm_dpc32(code, rd, rn, rm, 0, 0, 2, 0);
	} else
		arm_dpc32(code, rd, rn, rm, 0, 0, 2, 0);
}

#define ARM_ORR_REG_REG(p, rd, rn, rm)	arm_or_register((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_pop(void **code, int rl, int wb)
{
	if ((rl < 256) && (wb == 1))
		arm_pp16(code, rl, 0, 1);
	else
		arm_lsm32(code, 13, rl, 1, 1, wb);
}

#define ARM_POP(p, rl)			arm_pop((void **) &p, rl, 1)
#define ARM_POP2(p, r1, r2)		arm_pop((void **) &p, ((1 << r1) | (1 << r2)), 1)
#define ARM_POP4(p, r1, r2, r3, r4)	arm_pop((void **) &p, ((1 << r1) | (1 << r2) | (1 << r3) | (1 << r4)), 1)
#define ARM_POP_NWB(p, rl) 		arm_pop((void **) &p, rl, 0)

static __THUMB_INLINE__ void
arm_push(void **code, int rl, int wb)
{
	if ((rl < 256) && (wb == 1))
		arm_pp16(code, rl, 0, 0);
	else
		arm_lsm32(code, 13, rl, 0, 0, wb);
}

#define ARM_PUSH2(p, r1, r2)		arm_push((void **) &p, ((1 << r1) | (1 << r2)), 1)
#define ARM_PUSH4(p, r1, r2, r3, r4)	arm_push((void **) &p, ((1 << r1) | (1 << r2) | (1 << r3) | (1 << r4)), 1)
#define ARM_PUSH(p, rl)			arm_push((void **) &p, rl, 1)

static __THUMB_INLINE__ void
arm_ror_imm(void **code, int rd, int rn, int imm, int cond)
{
	arm_dpm32(code, rd, rn, imm, cond, 6);
}

#define ARM_ROR_REG_IMM8(p, rd, rn, imm)        arm_ror_imm((void **) &p, rd, rn, imm, 0)

static __THUMB_INLINE__ void
arm_ror_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 6);
}

#define ARM_ROR_REG_IMM(p, rd, rn, imm, rot)	arm_ror_imm_rot((void **) &p, rd, rm, imm, rot, 0)

static __THUMB_INLINE__ void
arm_ror_register(void **code, int rd, int rn, int rm)
{
	if ((rd < 8) && (rn < 8) && (rm < 8)) {
		if (rd == rm) 
			arm_dpri16(code, rd, rn, 7);
		else if (rd == rn)
			arm_dpri16(code, rd, rm, 7);
		else
			arm_dpc32(code, rd, rn, rm, 0, 0, 6, 0);
	} else
		arm_dpc32(code, rd, rn, rm, 0, 0, 6, 0);
}

#define ARM_ROR_REG_REG(p, rd, rn, rm)	arm_ror_register((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_rsb_imm(void **code, int rd, int rn, int imm, int cond)
{
	if ((rd < 8) && (rn < 8) && (imm == 0) && (cond != 0))
		arm_dpri16(code, rd, rn, 9);
	else
		arm_dpm32(code, rd, rn, imm, cond, 14);
}

#define ARM_RSB_REG_IMM8(p, rd, rn, imm)        arm_rsb_imm((void **) &p, rd, rn, imm, 0)
#define ARM_RSBS_REG_IMM8(p, rd, rn, imm)       arm_rsb_imm((void **) &p, rd, rn, imm, 1)

static __THUMB_INLINE__ void
arm_rsb_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 14);
}

#define ARM_RSB_REG_IMM(p, rd, rn, imm, rot)	arm_rsb_imm_rot((void **) &p, rd, rn, imm, rot, 0)
#define ARM_RSBS_REG_IMM(p, rd, rn, imm, rot)	arm_rsb_imm_rot((void **) &p, rd, rn, imm, rot, 1)

/* Emulate the ARM RSC instruction */
static __THUMB_INLINE__ void
arm_rsc_imm(void **code, int rd, int rn, int imm, int rot)
{
	/* Use the if then then else */
	arm_itte(code, ARMCOND_CC);
	/* If carry is set we adjust */
	/* First then */
	if ((rd < 8) && (rn < 8)) {
		if (rd == rn) 
			arm_ascmi16(code, rd, 1, 6);
		else
			arm_asi16(code, rd, rn, 1, 14);
	} else
		arm_add_imm(code, rd, rn, 1, 1);	
	/* Next then */
	if ((rot == 0) && (imm == 0) && (rd < 8))
		arm_dpri16(code, rd, rd, 9);
	else
		arm_rsb_imm_rot(code, rd, rd, imm, rot, 0);

	/* Else we just do the rsb */
	if ((rot == 0) && (imm == 0) && (rd < 8) && (rn < 8))
		arm_dpri16(code, rd, rn, 9);
	else
		arm_rsb_imm_rot(code, rd, rn, imm, rot, 0);
}

#define ARM_RSC_REG_IMM(p, rd, rn, imm, rot)	arm_rsc_imm((void **) &p, rd, rn, imm, rot)

static __THUMB_INLINE__ void
arm_rsb_register(void **code, int rd, int rn, int rm, int cond)
{
	arm_dpc32(code, rd, rn, rm, 0, cond, 14, 0);
}

#define ARM_RSB_REG_REG(p, rd, rn, rm)	arm_rsb_register((void **) &p, rd, rn, rm, 0)
#define ARM_RSBS_REG_REG(p, rd, rn, rm)	arm_rsb_register((void **) &p, rd, rn, rm, 1)

static __THUMB_INLINE__ void
arm_sbc_imm(void **code, int rd, int rn, int imm, int cond)
{
	arm_dpm32(code, rd, rn, imm, cond, 11);
}

#define ARM_SBC_REG_IMM8(p, rd, rn, imm, rot)	arm_sbc_imm((void **) &p, rd, rn, imm, 0)
#define ARM_SBCS_REG_IMM8(p, rd, rn, imm, rot)	arm_sbc_imm((void **) &p, rd, rn, imm, 1)

static __THUMB_INLINE__ void
arm_sbc_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	arm_dpm32_rot(code, rd, rn, imm, rot, cond, 11);
}

#define ARM_SBC_REG_IMM(p, rd, rn, imm, rot)	arm_sbc_imm_rot((void **) &p, rd, rn, imm, rot, 0)
#define ARM_SBCS_REG_IMM(p, rd, rn, imm, rot)	arm_sbc_imm_rot((void **) &p, rd, rn, imm, rot, 1)

static __THUMB_INLINE__ void
arm_sbc_register(void **code, int rd, int rn, int rm, int rot, int cond)
{
	if ((rd < 8) && (rd == rn) && (rm < 8) && (rot == 0) && (cond != 0)) {
		arm_dpri16(code, rd, rm, 6);
	} else 
		arm_dpc32(code, rd, rn, rm, rot, cond, 11, 0);
}

#define ARM_SBC_REG_REG(p, rd, rn, rm)	arm_sbc_register((void **) &p, rd, rn, rm, 0, 0)
#define ARM_SBCS_REG_REG(p, rd, rn, rm)	arm_sbc_register((void **) &p, rd, rn, rm, 0, 1)

static __THUMB_INLINE__ void
arm_sdiv(void **code, int rd, int rn, int rm)
{
	arm_mad32(code, rd, 15, rn, rm, 1, 15);
}

#define ARM_SDIV(p, rd, rn, rm)		arm_sdiv((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_smull(void **code, int rdhi, int rdlo, int rn, int rm)
{
	arm_mad32(code, rdhi, rdlo, rn, rm, 0, 0);
}

#define ARM_SMULL_REG_REG(p, rdhi, rdlo, rm, rs)	arm_smull((void **) &p, rdhi, rdlo, rm, rs)

static __THUMB_INLINE__ void
arm_stm(void **code, int rn, int rl)
{
	if (rn < 8)
		arm_lsm16(code, rn, rl, 0);
	else
		arm_lsm32(code, rn, rl, 0, 1, 0);
}

#define ARM_STM(p, rn, rl) arm_stm((void **)&p, rn, rl)

static __THUMB_INLINE__ void
arm_str_imm(void **code, int rn, int rt, int imm)
{
	if ((rn < 8) && (rt < 8) && (imm < 128) && ((imm % 4) == 0)) {
		imm >>= 2;
		arm_lswbi16(code, rn, rt, imm, 0, 2);
	} else {
		if ((imm > -256) && (imm < 256))
			arm_lsrn32(code, rn, rt, 0, 2, imm, 0, 0, 0);
		else if ((imm >= 0) && (imm < 4096))
			arm_lspr32(code, rn, rt, 0, 2, 1, imm, 0);
		else
			assert((imm > -256) && (imm < 4096));
	}
}
	
#define ARM_STR_IMM(p, rn, rt, offset) arm_str_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_strb_imm(void **code, int rn, int rt, int imm)
{
	if ((rn < 8) && (rt < 8) && (imm >= 0) && (imm < 32)) {
		arm_lswbi16(code, rn, rt, imm, 0, 1);
	} else {
		if ((imm > -256) && (imm < 256))  
			arm_lsrn32(code, rn, rt, 0, 0, imm, 0, 0, 0);
		else if ((imm >= 0) && (imm < 4096))
			arm_lspr32(code, rn, rt, 0, 0, 1, imm, 0);
		else
			assert((imm > -256) && (imm < 4096));
	}
}
	
#define ARM_STRB_IMM(p, rn, rt, offset) arm_strb_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_strex_reg(void **code, int rd, int rt, int rn, int offset)
{
	arm_lsxt32(code, rd, rt, rn, 0, offset);
}

#define ARM_STREX_REG(p, rd, rt, rn)	arm_strex_reg((void **) &p, rd, rt, rn, 0)

static __THUMB_INLINE__ void
arm_strh_imm(void **code, int rn, int rt, int imm)
{
	if ((rn < 8) && (rt < 8) && (imm >= 0) && (imm < 64) && ((imm % 2) == 0)) {
		imm >>= 1;
		arm_lshi16(code, rn, rt, imm, 0);
	} else {
		if ((imm > -256) && (imm < 256))
			arm_lsrn32(code, rn, rt, 0, 1, imm, 0, 0, 0);
		else if ((imm >= 0) && (imm < 4096))
			arm_lspr32(code, rn, rt, 0, 1, 1, imm, 0);
		else
			assert((imm > -256) && (imm < 4096));
	}
}
	
#define ARM_STRH_IMM(p, rn, rt, offset) arm_strh_imm((void **) &p, rn, rt, offset)

static __THUMB_INLINE__ void
arm_str_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 0);
	else
		arm_lsro32(code, rt, rn, rm, 0, 2, 0, 0);
}

#define ARM_STR_REG_REG(p, rt, rn, rm)	 arm_str_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_strb_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 2);
	else
		arm_lsro32(code, rt, rn, rm, 0, 0, 0, 0);
}

#define ARM_STRB_REG_REG(p, rt, rn, rm)	 arm_strb_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_strh_reg(void **code, int rt, int rn, int rm)
{
	if ((rt < 8) && (rn < 8) && (rm < 8))
		arm_lsro16(code, rt, rn, rm, 1);
	else
		arm_lsro32(code, rt, rn, rm, 0, 1, 0, 0);
}

#define ARM_STRH_REG_REG(p, rt, rn, rm)	 arm_strh_reg((void **)&p, rt, rn, rm)

static __THUMB_INLINE__ void
arm_sub_imm(void **code, int rd, int rn, int imm, int cond)
{
	if (rn == ARMREG_SP) 
		arm_sub_sp_imm(code, rd, imm, cond);
	else if (rd == ARMREG_SP) {
		arm_mov_reg(code, rd, rn, cond);
		arm_sub_sp_imm(code, rd, imm, cond);
	} else {
		if ((cond != 0) && (rd < 8) && (rn < 8)) {
			if (rd == rn) {
				if ((imm >= 0) && (imm < 256)) 
					arm_ascmi16(code, rd, imm, 7);
				else
					arm_dpm32(code, rd, rn, imm, cond, 13);
			} else {
				if ((imm >= 0) && (imm < 8)) 
					arm_asi16(code, rd, rn, imm, 13);
				else
					arm_dpm32(code, rd, rn, imm, cond, 13);
			}
		} else
			arm_dpm32(code, rd, rn, imm, cond, 13);
	}
}

#define ARM_SUB_REG_IMM8(p, rd, rn, imm)				\
	do {								\
		if (imm < 0) 						\
			arm_add_imm((void **) &p, rd, rn, -imm, 0);	\
		else							\
			arm_sub_imm((void **) &p, rd, rn, imm, 0);	\
	} while (0)
		
#define ARM_SUBS_REG_IMM8(p, rd, rn, imm)				\
	do {								\
		if (imm < 0) 						\
			arm_add_imm((void **) &p, rd, rn, -imm, 1);	\
		else							\
			arm_sub_imm((void **) &p, rd, rn, imm, 1);	\
	} while (0)

static __THUMB_INLINE__ void
arm_sub_imm_rot(void **code, int rd, int rn, int imm, int rot, int cond)
{
	if (rot == 0)
		arm_sub_imm(code, rd, rn, imm, cond);
	else {
		/* 
		 * Allow for ARM_SUB_REG_IMM (p, ARMREG_SP, x, i, rot) which is
		 * valid for ARM but not for thumb 
		 */
		if (rd == ARMREG_SP) {
			arm_dpm32_rot(code, ARMREG_IP, rn, imm, rot, 0, 13);
			arm_mov_reg (code, ARMREG_SP, ARMREG_IP, cond);
		} else
			arm_dpm32_rot(code, rd, rn, imm, rot, cond, 13);
	}
}

#define ARM_SUB_REG_IMM(p, rd, rn, imm, rot)	arm_sub_imm_rot((void **) &p, rd, rn, imm, rot, 0)
#define ARM_SUBS_REG_IMM(p, rd, rn, imm, rot)	arm_sub_imm_rot((void **) &p, rd, rn, imm, rot, 1)

static __THUMB_INLINE__ void
arm_sub_register(void **code, int rd, int rn, int rm, int rot, int cond)
{
	if ((rd < 8) && (rn < 8) && (rm < 8) && (rot == 0) && (cond != 0))
		arm_asr16(code, rd, rn, rm, 1);
	else if ((rd != ARMREG_SP) && (rm != ARMREG_SP))
		arm_dpc32(code, rd, rn, rm, rot, cond, 13, 0);
	else {
                if (rm == ARMREG_SP) {
                	arm_mov_reg(code, ARMREG_IP, ARMREG_SP, 0);
			if (rd == ARMREG_SP) 
				arm_dpc32(code, ARMREG_IP, rn, ARMREG_IP, rot, cond, 13, 0);
                        else
				arm_dpc32(code, rd, rn, ARMREG_IP, rot, cond, 13, 0);
		} else {
			arm_dpc32(code, ARMREG_IP, rn, rm, rot, cond, 13, 0);
                	arm_mov_reg(code, ARMREG_SP, ARMREG_IP, 0);
                }
	}
}

#define ARM_SUB_REG_REG(p, rd, rn, rm)	arm_sub_register((void **) &p, rd, rn, rm, 0, 0)
#define ARM_SUBS_REG_REG(p, rd, rn, rm)	arm_sub_register((void **) &p, rd, rn, rm, 0, 1)

static __THUMB_INLINE__ void
arm_sub_sp_reg(void **code, int rm, int cond)
{
	arm_dpc32(code, ARMREG_SP, ARMREG_SP, rm, 0, cond, 13, 0);
}			

static __THUMB_INLINE__ void
arm_sub_sp_imm(void **code, int rd, int imm, int cond)
{
	if (rd == ARMREG_SP) {
		if (imm % 4 == 0) {
			if ((imm >= 0) && (imm < 511))
				arm_ajsp16(code, imm / 4, 1);
			else
				arm_dpm32(code, rd, ARMREG_SP, imm, cond, 13);
		} else {
			arm_dpm32(code, rd, ARMREG_SP, imm, cond, 13);
		}
	} else if (cond != 0) {
		if ((rd < 8) && (imm % 4 == 0) && (imm >= 0) && (imm < 1024)) 
			arm_asp16(code, rd, (imm / 4), 1);
		else {
			arm_dpm32(code, rd, ARMREG_SP, imm, cond, 13);
		}
	} else {
		arm_dpm32(code, rd, ARMREG_SP, imm, 0, 13);
	}
}			

static __THUMB_INLINE__ void
arm_tst_imm(void **code, int rn, int imm)
{
	arm_dpm32(code, 15, rn, imm, 1, 0);
}

#define ARM_TST_REG_IMM(p, rn, imm)	arm_tst_imm((void **) &p, rn, imm)

static __THUMB_INLINE__ void
arm_tst_reg(void **code, int rn, int rm, int imm)
{
	if ((rn < 8) && (rm < 8) && (imm == 0))
		arm_dpri16(code, rn, rm, 8);
	else
		arm_dpc32(code, 15, rn, rm, imm, 1, 0, 0);
}

#define ARM_TST_REG_REG(p, rn, rm, rot)	arm_tst_reg((void **) &p, rn, rm , rot)

static __THUMB_INLINE__ void
arm_udiv(void **code, int rd, int rn, int rm)
{
	arm_mad32(code, rd, 15, rn, rm, 3, 15);
}

#define ARM_UDIV(p, rd, rn, rm)		arm_udiv((void **) &p, rd, rn, rm)

static __THUMB_INLINE__ void
arm_umull(void **code, int rdhi, int rdlo, int rm, int rs)
{
	arm_mad32(code, rdhi, rdlo, rm, rs, 2, 0);
}

#define ARM_UMULL_REG_REG(p, rdhi, rdlo, rm, rs)	arm_umull((void **) &p, rdhi, rdlo, rm, rs)

#define ARM_LOAD_RELPC(p, r) \
	do {							\
		ARM_LDR_IMM (p, r, ARMREG_PC, 2);		\
		ARM_B (p, 1);					\
	} while (0)
#define ARM_RELPC_OFFSET	6	/* Size of ARM_LOAD_RELPC sequence */

#define ARM_FLOADS_RELPC(p, r) \
	do {							\
		ARM_CNOP (p);					\
		ARM_FLDS (p, r, ARMREG_PC, 4);			\
		ARM_B (p, 2);					\
		ARM_NOPS (p);					\
	} while (0)

#define ARM_FLOADD_RELPC(p, r) \
	do {							\
		ARM_CNOP (p);					\
		ARM_FLDD (p, r, ARMREG_PC, 4);			\
		ARM_B (p, 4);					\
		ARM_NOPS (p);					\
	} while (0)

#define ARM_LOAD_REGPC(p, r) \
	do {							\
		ARM_ADD_REG_REG (code, r, r, ARMREG_PC);	\
		ARM_LDR_IMM (code, r, r, 4);			\
	} while (0)

#define ARM_LOAD_PCOFF(p, r) \
	do {							\
		ARM_ADD_REG_REG (code, r, r, ARMREG_PC);	\
		ARM_LDR_IMM (code, ARMREG_PC, r, 0);		\
	} while (0)

#define ARM_SWITCH(p, r) 								\
	do {										\
		ARM_ADR_IMM(p, ARMREG_IP, 12);						\
		ARM_LDR_REG_REG_SHIFT(p, ARMREG_IP, ARMREG_IP, r, ARMSHIFT_LSL, 2);     \
		ARM_BX(p, ARMREG_IP);							\
		ARM_NOPS(p);								\
	} while (0)

#define ARM_JUMP_REG(p, r, t)		ARM_BX(p, r)

#define ARM_CALL_REG(p,r)	ARM_BLX_REG(p, r)
#define ARM_CALL_REG_PARM(p,r)	ARM_BLX_REG_ALIGN (p, r)

#define ARM_JUMP_REG_PARM(p, r, addr)			\
	do {						\
		ARM_LDR_IMM(p, r, ARMREG_PC, 6);	\
		ARM_BX(p, r);				\
		*(guint32 *)p = (guint32) addr;		\
		p += 4;					\
	} while (0)

#define ARM_JUMP_REG_PARMA(p, r1, arg, addr)		\
	do {						\
		ARM_ADR_IMM(p, r1, 4);			\
		ARM_LDR_IMM(p, ARMREG_PC, ARMREG_PC, 4);\
		*(guint32 *)p = (guint32) arg;		\
		p += 4;					\
		*(guint32 *)p = (guint32) addr;		\
		p += 4;					\
	} while (0)

#define ARM_JUMP_REG_PARM2(p, r1, arg, addr)		\
	do {						\
		ARM_LDR_IMM(p, r1, ARMREG_PC, 4);	\
		ARM_LDR_IMM(p, ARMREG_PC, ARMREG_PC, 4);\
		*(guint32 *)p = (guint32) arg;		\
		p += 4;					\
		*(guint32 *)p = (guint32) addr;		\
		p += 4;					\
	} while (0)

#define ARM_CALL_REG_PARM2(p, r1, arg, addr)				\
	do {								\
		ARM_LDR_IMM(p, r1, ARMREG_PC, 2 * sizeof(gpointer));	\
		ARM_LDR_IMM(p, ARMREG_LR, ARMREG_PC, ARMDISP_LDRPC);	\
		p = emit_bx(p, r1);					\
		*(guint32 *)p = (guint32) arg;				\
		p += 4;							\
		*(guint32 *)p = (guint32) addr;				\
		p += 4;							\
	} while (0)

#define ARM_JUMP_OFFSET(p, r, o) 		\
	do {					\
		ARM_LDR_IMM (code, r, r, o);	\
		ARM_BX (code, r);		\
	} while (0)

#define ARM_CALL_TARGET(c)	c = (guint8 *) (((uintptr_t) c) | 0x1)

/*
 * NOP if word alignment is required 
 */
#define ARM_CNOP(c)						\
	do {							\
		if (((uintptr_t) (c) & 0x3) != 0)		\
			arm_nop16((void **) &c, OP_NOP16);	\
	} while (0)

/*
 * Short nop
 */
#define ARM_NOPS(c) arm_nop16((void **) &c, OP_NOP16)

/*
 * Load Double Constant
 */
#define ARM_LOAD_IMMD(c, rd, v)					\
	do {							\
		guchar *jmp;					\
		ARM_CNOP (c);					\
		ARM_FLDD(c, rd, ARMREG_PC, ARMDISP_LDRPC);	\
		jmp = c;					\
		ARM_B (c, 0);					\
		*(double *)c = v;				\
		c += sizeof(double);				\
		arm_patch (jmp, c);				\
	} while (0)

/*
 * Load Float Constant
 */
#define ARM_LOAD_IMMS(c, rd, v)					\
	do {							\
		guchar *jmp;					\
		ARM_CNOP (c);					\
		ARM_FLDS(c, rd, ARMREG_PC, ARMDISP_LDRPC);	\
		jmp = c;					\
		ARM_B (c, 0);					\
		*(float *)c = v;				\
		c += sizeof(float);				\
		arm_patch (jmp, c);				\
	} while (0)

/*
 * Offset from PLT for GOT entry
 */
#define ARM_PLTOFF	8

/*
 * To be compatible with full ARM we pad the thunk
 * with a NOP
 */
#define ARM_THUNK(p, r, t)				\
	do {						\
		ARM_LDR_IMM (p, r, ARMREG_PC, 8);	\
		ARM_BX (p, r);				\
		ARM_NOPS (p);				\
	} while (0)

typedef struct {
	uint32_t ldr;		/* LDR */
	uint32_t oi;		/* ORR */
	uint16_t bx;		/* BX */
	uint16_t nop;		/* NOP */
	uint32_t tgt;		/* Target */
} arm_thunk_t;
		
#define CODE_ADDR(x) (typeof(x))((uintptr_t) x | 0x1)
#define CODE_PTR(x) (typeof(x))((uintptr_t) x & ~1L)

#ifdef __cplusplus
}
#endif

#endif
