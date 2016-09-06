
/* 
 * asm.c --
 *
 *	The procedures in this file do assembly and dis-assembly of
 *	DLX assembler instructions.
 *
 * Copyright 1989 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /auto/labs/acaps/compiler/dlx/superdlx/RCS/asm.c,v 1.2 93/04/13 23:43:16 cecile Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <ctype.h>
#include <errno.h>
#include "hash.h"   // cambiado
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcl/tcl.h" // cambiado
#include "dlx.h"
#include "asm.h"
#include "gp.h"
#include "sym.h"
#include "traps.h"  // añadido
#include "gp.h"     // añadido

static unsigned end_pgm_address;	/* End of program loaded from file */

int		num_asm_files;
asmHTbl		*asmHashTbl;

#define DEFAULT_NUM_ASM_FILES		16

/*
 * The following structure type encapsulates the state of loading one
 * or more assembler files, and is used for communication between the
 * procedures that do assembly.
 */

/*
 * Flags for LoadInfo structures:
 *
 * ASM_CODE -			1 means currently assembling into code
 *				area;  0 means currently assembling into
 *				data area.
 * ASM_SIZE_ONLY -		1 means this is the first pass, where the
 *				only important thing is size (suppress
 *				all error messages).
 * ASM_ALIGN_0 -		1 means that an "align 0" command is in
 *				effect.
 */

#define ASM_CODE	1
#define ASM_SIZE_ONLY	2
#define ASM_ALIGN_0	4

#define ASM_MAX_ERRORS	20

/*
 * The #defines below specify the different classes of instructions,
 * as defined on pp. D-4 to D-6 of Kane's book.  These classes are used
 * during assembly, and indicate the different formats that may be taken
 * by operand specifiers for a particular opcode.
 * 
 * NO_ARGS -		no operands
 * LOAD -		(register, address)
 * STORE -		(address, register)
 * LUI -		(dest, 16-bit expression)
 * ARITH -		(dest, src1, sr2c) OR (dest/src1, src2)
 *			OR (dest, src1, 16-bit immediate)
 *			OR (dest/src1, 16-bit immediate)
 * MULDIV -		same as ARITH (special subset to handle
 *			mult/divide/rem instructions because they
 *                      operate on floating point registers)
 * BRANCH_0_OP -	(label) the source register is implied
 * BRANCH_1_OP -	(src1, label)
 * JUMP -		(label) OR (src1)
 * SRC1 -		(src1)
 * LABEL -		(label)
 * MOVE -		(dest,src1)
 * FLOAD -		like LOAD, for floating/double
 * FSTORE -		like STORE, for floating/double
 * FARITH -		like ARITH, for floating point
 * FCOMPARE -		compare two floating point numbers
 * TRAP -		(trap number)
 */

#define NO_ARGS		0
#define LOAD		1
#define STORE		2
#define LUI		3
#define ARITH		4
#define MULDIV		5
#define BRANCH_0_OP	7
#define BRANCH_1_OP	8
#define JUMP		9
#define SRC1		10
#define LABEL		11
#define MOVE		12
#define FLOAD		13
#define FSTORE		14
#define FARITH		15
#define FCOMPARE	16
#define TRAP		17

/*
 * The tables below give the maximum and minimum # of arguments
 * permissible for each class above.
 */

static int minArgs[] =
    {0, 3, 3, 2, 3, 3, 3, 1, 2, 1, 1, 1, 2, 3, 3, 3, 2, 1};
static int maxArgs[] =
    {0, 3, 3, 2, 3, 3, 3, 1, 2, 1, 1, 1, 2, 3, 3, 3, 2, 1};

/*
 * Structures of the following type are used during assembly and
 * disassembly.  One such structure exists for each defined op code.
 */

typedef struct {
    char *name;			/* Opcode name, e.g. "add". */
    int class;			/* Class of instruction (see table above). */
    int op;			/* Bit pattern corresponding to this
				 * instruction. */
    int mask;			/* Used for disassembly:  if these bits match
				 * op, then use this opcode for disassembly.
				 * 0 means this is a synthesized instruction
				 * that doesn't exist in native form, so
				 * it should be ignored during disassembly. */
    int other;			/* This field is used when the assembler
				 * is generating multiple instructions for
				 * a single opcode, or when different
				 * instructions may be generated for the
				 * same opcode (e.g. add -> addi).  The
				 * meaning of the field depends on class;
				 * see the code in Asm_Assemble. */
    int flags;			/* OR-ed combination of bits, giving various
				 * information for use during assembly, such
				 * as for range checking.  See below for
				 * values. */
    int rangeMask;		/* Mask for use in range check:  for unsigned
				 * check, none of these bits must be set.  For
				 * sign-extended check, either all or none
				 * must be set. */
} OpcodeInfo;

/*
 * Bits for "flags" field, used for range checking:
 *
 * CHECK_FIRST -		1 means check first argument, if it is an immediate
 * CHECK_LAST -			1 means check last argument, if it is
 *				an immediate.
 * CHECK_NEXT_TO_LAST -		1 means check next-to-last argument, if it
 *				is an immediate.
 * IMMEDIATE_REQ -		1 means the argument given above MUST be
 *				an immediate.
 * SIGN_EXTENDED -		1 means the immediate will be sign-extended.
 * FIRST_F
 * SECOND_F
 * THIRD_F			Must the 1st/2nd/3rd be a single or double prec.
 *				floating point register
 * FIRST_D
 * SECOND_D
 * THIRD_D			Must the 1st/2nd/3rd be a double prec. register?
 */

#define CHECK_LAST		0x1
#define CHECK_NEXT_TO_LAST	0x2
#define IMMEDIATE_REQ		0x4
#define SIGN_EXTENDED		0x8
#define CHECK_FIRST		0x10
#define FIRST_INT		0x20
#define SECOND_INT		0x40
#define THIRD_INT		0x80
#define FIRST_F			0x100
#define SECOND_F		0x200
#define THIRD_F			0x400
#define FIRST_D			0x800
#define SECOND_D		0x1000
#define THIRD_D			0x2000

/*
 * Table of all known instructions:
 */

OpcodeInfo opcodes[] = {
    {"add", ARITH, 0x20, 0xfc00003f, 0x20000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"addd", FARITH, 0x04000004, 0xfc00003f, 0, FIRST_D|SECOND_D|THIRD_D, 0},
    {"addf", FARITH, 0x04000000, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"addi", ARITH, 0x20000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"addu", ARITH, 0x21, 0xfc00003f, 0x24000000, CHECK_LAST, 0xffff0000},
    {"addui", ARITH, 0x24000000, 0xfc000000, 0,
	    CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"and", ARITH, 0x24, 0xfc00003f, 0x30000000, CHECK_LAST, 0xffff0000},
    {"andi", ARITH, 0x30000000, 0xfc000000, 0,
	    CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"beqz", BRANCH_1_OP, 0x10000000, 0xfc000000, 0, 0, 0},
    {"bfpf", BRANCH_0_OP, 0x18000000, 0xfc000000, 0, 0, 0},
    {"bfpt", BRANCH_0_OP, 0x1c000000, 0xfc000000, 0, 0, 0},
    {"bnez", BRANCH_1_OP, 0x14000000, 0xfc000000, 0, 0, 0},
    {"cvtd2f", MOVE, 0x0400000a, 0xfc00003f, 0, FIRST_F|SECOND_D, 0},
    {"cvtd2i", MOVE, 0x0400000b, 0xfc00003f, 0, FIRST_F|SECOND_D, 0},
    {"cvtf2d", MOVE, 0x04000008, 0xfc00003f, 0, FIRST_D|SECOND_F, 0},
    {"cvtf2i", MOVE, 0x04000009, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"cvti2d", MOVE, 0x0400000d, 0xfc00003f, 0, FIRST_D|SECOND_F, 0},
    {"cvti2f", MOVE, 0x0400000c, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"div", MULDIV, 0x0400000f, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"divd", FARITH, 0x04000007, 0xfc00003f, 0, FIRST_D|SECOND_D|THIRD_D, 0},
    {"divf", FARITH, 0x04000003, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"divu", MULDIV, 0x04000017, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"eqd", FCOMPARE, 0x04000018, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"eqf", FCOMPARE, 0x04000010, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"ged", FCOMPARE, 0x0400001d, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"gef", FCOMPARE, 0x04000015, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"gtd", FCOMPARE, 0x0400001b, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"gtf", FCOMPARE, 0x04000013, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"j", JUMP, 0x08000000, 0xfc000000, 0x48000000, 0, 0},
    {"jal", JUMP, 0x0c000000, 0xfc000000, 0x4c000000, 0, 0},
    {"jalr", SRC1, 0x4c000000, 0xfc000000, 0, 0, 0},
    {"jr", SRC1, 0x48000000, 0xfc000000, 0, 0, 0},
    {"lb", LOAD, 0x80000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"lbu", LOAD, 0x90000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"ld", FLOAD, 0x9c000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED|FIRST_D, 0xffff8000},
    {"led", FCOMPARE, 0x0400001c, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"lef", FCOMPARE, 0x04000014, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"lf", FLOAD, 0x98000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED|FIRST_F, 0xffff8000},
    {"lh", LOAD, 0x84000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"lhi", LUI, 0x3c000000, 0xfc000000, 0,
	    CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"lhu", LOAD, 0x94000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"ltd", FCOMPARE, 0x0400001a, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"ltf", FCOMPARE, 0x04000012, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"lw", LOAD, 0x8c000000, 0xfc000000, 0,
	    CHECK_NEXT_TO_LAST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"movd", MOVE, 0x00000033, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"movf", MOVE, 0x00000032, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"movfp2i", MOVE, 0x00000034, 0xfc00003f, 0, SECOND_F, 0},
    {"movi2fp", MOVE, 0x00000035, 0xfc00003f, 0, FIRST_F, 0},
    {"movi2s", MOVE, 0x00000030, 0xfc00003f, 0, 0, 0},
    {"movs2i", MOVE, 0x00000031, 0xfc00003f, 0, 0, 0},
    {"mult", MULDIV, 0x0400000e, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"multd", FARITH, 0x04000006, 0xfc00003f, 0, FIRST_D|SECOND_D|THIRD_D, 0},
    {"multf", FARITH, 0x04000002, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"multu", MULDIV, 0x04000016, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"ned", FCOMPARE, 0x04000019, 0xfc00003f, 0, FIRST_D|SECOND_D, 0},
    {"nef", FCOMPARE, 0x04000011, 0xfc00003f, 0, FIRST_F|SECOND_F, 0},
    {"nop", NO_ARGS, 0x0, 0xffffffff, 0, 0, 0},
    {"or", ARITH, 0x25, 0xfc00003f, 0x34000000, CHECK_LAST, 0xffff0000},
    {"ori", ARITH, 0x34000000, 0xfc000000, 0,
	    CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"rfe", LABEL, 0x40000000, 0xfc000000, 0, 0, 0xfe000000},
    {"sb", STORE, 0xa0000000, 0xfc000000, 0,
	    CHECK_FIRST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"sd", FSTORE, 0xbc000000, 0xfc000000, 0,
	    CHECK_FIRST|IMMEDIATE_REQ|SIGN_EXTENDED|THIRD_D, 0xffff8000},
    {"seq", ARITH, 0x00000028, 0xfc00003f, 0x60000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"seqi", ARITH, 0x60000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"sequ", ARITH, 0x00000010, 0xfc00003f, 0xc0000000,
            CHECK_LAST, 0xffff0000},
    {"sequi", ARITH, 0xc0000000, 0xfc000000, 0,
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sf", FSTORE, 0xb8000000, 0xfc000000, 0,
	    CHECK_FIRST|IMMEDIATE_REQ|SIGN_EXTENDED|THIRD_F, 0xffff8000},
    {"sge", ARITH, 0x0000002d, 0xfc00003f, 0x74000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"sgei", ARITH, 0x74000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"sgeu", ARITH, 0x00000015, 0xfc00003f, 0xd4000000, 
            CHECK_LAST, 0xffff0000},
    {"sgeui", ARITH, 0xd4000000, 0xfc000000, 0,
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sgt", ARITH, 0x0000002b, 0xfc00003f, 0x6c000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"sgti", ARITH, 0x6c000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"sgtu", ARITH, 0x00000013, 0xfc00003f, 0xcc000000, 
            CHECK_LAST, 0xffff0000},
    {"sgtui", ARITH, 0xcc000000, 0xfc000000, 0,
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sh", STORE, 0xa4000000, 0xfc000000, 0,
	    CHECK_FIRST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"sle", ARITH, 0x0000002c, 0xfc00003f, 0x70000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"slei", ARITH, 0x70000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"sleu", ARITH, 0x00000014, 0xfc00003f, 0xd0000000, 
            CHECK_LAST, 0xffff0000},
    {"sleui", ARITH, 0xd0000000, 0xfc000000, 0,
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sll", ARITH, 0x4, 0xfc00003f, 0x50000000, CHECK_LAST, 0xffffffe0},
    {"slli", ARITH, 0x50000000, 0xfc000000, 0, 
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"slt", ARITH, 0x0000002a, 0xfc00003f, 0x68000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"slti", ARITH, 0x68000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"sltu", ARITH, 0x00000012, 0xfc00003f, 0xc8000000, 
            CHECK_LAST, 0xffff0000},
    {"sltui", ARITH, 0xc8000000, 0xfc000000, 0,
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sne", ARITH, 0x00000029, 0xfc00003f, 0x64000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"snei", ARITH, 0x64000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"sneu", ARITH, 0x00000011, 0xfc00003f, 0xc4000000, 
            CHECK_LAST, 0xffff0000},
    {"sneui", ARITH, 0xc4000000, 0xfc000000, 0,
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sra", ARITH, 0x7, 0xfc00003f, 0x5c000000, CHECK_LAST, 0xffffffe0},
    {"srai", ARITH, 0x5c000000, 0xfc000000, 0, 
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"srl", ARITH, 0x6, 0xfc00003f, 0x58000000, CHECK_LAST, 0xffffffe0},
    {"srli", ARITH, 0x58000000, 0xfc000000, 0, 
            CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sub", ARITH, 0x22, 0xfc00003f, 0x28000000,
	    CHECK_LAST|SIGN_EXTENDED, 0xffff8000},
    {"subd", FARITH, 0x04000005, 0xfc00003f, 0, FIRST_D|SECOND_D|THIRD_D, 0},
    {"subf", FARITH, 0x04000001, 0xfc00003f, 0, FIRST_F|SECOND_F|THIRD_F, 0},
    {"subi", ARITH, 0x28000000, 0xfc000000, 0,
	    CHECK_LAST|SIGN_EXTENDED|IMMEDIATE_REQ, 0xffff8000},
    {"subu", ARITH, 0x23, 0xfc00003f, 0x2c000000, CHECK_LAST, 0xffff0000},
    {"subui", ARITH, 0x2c000000, 0xfc000000, 0,
	    CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {"sw", STORE, 0xac000000, 0xfc000000, 0,
	    CHECK_FIRST|IMMEDIATE_REQ|SIGN_EXTENDED, 0xffff8000},
    {"trap", TRAP, 0x44000000, 0xfc000000, 0,
	    CHECK_FIRST|IMMEDIATE_REQ, 0xfc000000},
    {"xor", ARITH, 0x26, 0xfc00003f, 0x38000000, CHECK_LAST, 0xffff0000},
    {"xori", ARITH, 0x38000000, 0xfc000000, 0,
	    CHECK_LAST|IMMEDIATE_REQ, 0xffff0000},
    {NULL, NO_ARGS, 0, 0, 0, 0, 0}
};

/*
 * Opcode values that are used in the code of this module:
 */

#define SUBU_OP			0x23
#define ADDI_OP			0x20000000
#define ADDIU_OP		0x24000000
#define LUI_OP			0x3c000000
#define ORI_OP			0x34000000
#define BEQ_OP			0x10000000
#define BNE_OP			0x14000000
#define LOAD_IMM(reg, x)	(0x20000000 | ((reg) << 16) | ((x) & 0xffff))
#define LOAD_IMM_UNS(reg, x)	(0x34000000 | ((reg) << 16) | ((x) & 0xffff))

/*
 * Table mapping from register number to register name.
 */

char *Asm_RegNames[] = {
    "r0", "r1", "r2", "r3",
    "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11",
    "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19",
    "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "r27",
    "r28", "r29", "r30", "r31",
    "f0", "f1", "f2", "f3",
    "f4", "f5", "f6", "f7",
    "f8", "f9", "f10", "f11",
    "f12", "f13", "f14", "f15",
    "f16", "f17", "f18", "f19",
    "f20", "f21", "f22", "f23",
    "f24", "f25", "f26", "f27",
    "f28", "f29", "f30", "f31",
    "hi", "lo", "pc", "npc",	/* some of these will be removed later <bly> */
    "status", "cause", "epc"
};

/*
 * Size of largest instruction we can assemble, in words:
 */

#define ASM_MAX_WORDS 5		/* probably 1 <bly> */

/*
 * Forward declarations for procedures defined in this file:
 */

static void	AddErrMsg();
static void	IndicateError();
static void	ReadFile();
static int	StoreWords();


/*
 *----------------------------------------------------------------------
 *
 * Asm_Assemble --
 *
 *	Given a string describing an assembler instruction, return
 *	the binary code corresponding to the instruction.
 *
 * Results:
 *	The return value is a standard Tcl result (normally TCL_OK plus
 *	an empty string).  If the assembly completed successfully, then
 *	*sizePtr gets filled in with the # of instruction words assembled
 *	(may be more than 1 for special pseudo-instructions), and the
 *	word(s) at *codePtr get filled in with the actual instruction.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Asm_Assemble(machPtr, localTbl, fileName, string, dot, wholeLine, sizeOnly,
	sizePtr, codePtr)
    DLX *machPtr;			/* Machine for which assembly is done:
					 * used for symbol table and error
					 * reporting. */
    Hash_Table *localTbl;		/* Local Symbol Table for this file */
    char *fileName;			/* File name to use for symbol
					 * lookups.  See GetSym procedure in
					 * sym.c for more information. */
    char *string;			/* DLX assembler instruction. */
    unsigned int dot;			/* Where in memory instruction(s) will
					 * be placed. */
    char *wholeLine;			/* Entire line containing string;  used
					 * when printing error messages. */
    int sizeOnly;			/* Non-zero means this is the first
					 * pass, so ignore errors and just
					 * compute the size of the instruction.
					 */
    int *sizePtr;			/* Fill in with # words assembled
					 * for this instruction. */
    int *codePtr;			/* Pointer to ASM_MAX_WORDS words
					 * storage, which get filled in with
					 * assembled code. */
{
    register OpcodeInfo *insPtr;	/* Info about instruction. */
    register char *p;			/* Current character in string. */
    char *errMsg;
    char *opStart;
    int length;
    char msg[100];
    int isReg[3];			/* Tells whether each operand in the
					 * instruction is a register. */
    int operands[3];			/* Value of each operand (reg #,
					 * immediate, shift amount, etc.). */
    char *argStart[3];			/* First chars. of arguments (for
					 * error reporting). */
    int numOps;				/* Number of operands in the
					 * instruction. */
    int done;
    int requireF, requireD;		/* set if the current operand must be
					 * float or double */

    /*
     * Parse off the instruction name, and look it up in the table.
     */

    for (p = string; (*p == ' ') || (*p == '\t'); p++) {
	/* Empty loop body. */
    }
    opStart = p;
    for ( ; isalnum(*p); p++) {
	/* Empty loop body. */
    }
    length = p-opStart;
    if (length > 0) {
	for (insPtr = opcodes; insPtr->name != NULL; insPtr++) {
	    if ((insPtr->name[0] == opStart[0])
		    && (strncmp(insPtr->name, opStart, length) == 0)
		    && (insPtr->name[length] == 0)) {
		codePtr[0] = insPtr->op;
		goto gotIns;
	    }
	}
    }
    errMsg= "unknown opcode";
    p =  opStart;
    goto error;

    /*
     * Parse up to three operand fields in the instruction, storing
     * information in isReg[], operands[], and numOps.
     */

    gotIns:
    isReg[0] = isReg[1] = isReg[2] = 0;
    operands[0] = operands[1] = operands[2] = 0;
    for (numOps = 0; numOps < 3; numOps++) {
	char *end, savedChar;
	int result;

	/*
	 * Find the starting character for this operand specifier.
	 */

	while ((*p == ' ') || (*p == '\t')) {
	    p++;
	}
	argStart[numOps] = p;

	/*
	 * The code below is a special case to handle the second
	 * specifier for instructions in the load-store class.  Accept
	 * an optional expression followed by an optional register
	 * name in parentheses.
	 */

	if (((numOps == 0) && ((insPtr->class == STORE) || (insPtr->class == FSTORE))) ||
	    ((numOps == 1) && ((insPtr->class == LOAD) || (insPtr->class == FLOAD)))) {
	    if (*p == '(') {
		operands[numOps] = 0;
	    } else {
		if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly,
			&operands[numOps], &end) != TCL_OK) {
		    errMsg = machPtr->interp->result;
		    goto error;
		}
		p = end;
	    }
	    while ((*p == ' ') || (*p == '\t')) {
		p++;
	    }
	    argStart[++numOps] = p;
	    isReg[numOps] = 1;
	    operands[numOps] = 0;
	    if (*p == '(') {
		for (p++; *p != ')'; p++) {
		    if ((*p == 0)  || (*p == ';')) {
			p = argStart[numOps];
			errMsg = "missing ) after base register";
			goto error;
		    }
		}
		savedChar = *p;
		*p = 0;
		result = Sym_GetSym(machPtr, localTbl, fileName,
			argStart[numOps]+1, SYM_REGS_OK,
			(unsigned int *) &operands[numOps]);
		*p = savedChar;
		if (result != SYM_REGISTER) {
		    p = argStart[numOps]+1;
		    errMsg = (result == SYM_FREG_FOUND)
				? "floating register invalid here"
				: "bad base register name";
		    goto error;
		}
		p++;
	    }
	/* read till we find a separator */
	    for (done = 0; !done; ) {
		switch (*p) {
		case ',' :
		case '#' :
		case ';' :
		case '\n' :
		case '\0' :
		    done = 1;
		    break;
		case ' ' :
		case '\t' :
		    break;
		default :
		    errMsg = "unknown garbage in expression";
		    goto error;
		}
		if (!done)
		    p++;
	    }
		    
	    end = p;
	    goto about_to_continue;
	}

	/*
	 * Back to the normal case.  Find the end of the current
	 * operand specifier.
	 */

	while ((*p != ',') && (*p != ';') && (*p != 0) && (*p != '\n')) {
	    p++;
	}
	end = p;
	if (p == argStart[numOps]) {
	    if (numOps == 0) {
		break;
	    }
	    errMsg = "empty operand specifier";
	    goto error;
	}
	for (p--; (*p == ' ') || (*p == '\t'); p--) {
	    /* Null loop body;  just backspace over space */
	}
	p++;

	/*
	 * Figure out what kind of operand this is.
	 */

	savedChar = *p;
	*p = 0;

	/* determine if we need a floating/double register now */
	requireF = ((numOps == 0) && (insPtr->flags & FIRST_F)) ||
		   ((numOps == 1) && (insPtr->flags & SECOND_F)) ||
		   ((numOps == 2) && (insPtr->flags & THIRD_F));
	requireD = ((numOps == 0) && (insPtr->flags & FIRST_D)) ||
		   ((numOps == 1) && (insPtr->flags & SECOND_D)) ||
		   ((numOps == 2) && (insPtr->flags & THIRD_D));
	if (requireF || requireD) {
	    result = Sym_GetSym(machPtr, localTbl, fileName, argStart[numOps],
	        SYM_FREGS_OK, (unsigned int *) &operands[numOps]);
	    if (result != SYM_REGISTER) {
		*p = savedChar;
		p = argStart[numOps];
		errMsg = "floating pointer register required";
		goto error;
	    }
	    isReg[numOps] = 1;
	    if (requireD && (operands[numOps] & 1)) {
		*p = savedChar;
		p = argStart[numOps];
		errMsg = "double floating point register required";
		goto error;
	    }
	} else {
	    result = Sym_GetSym(machPtr, localTbl, fileName, argStart[numOps],
	        SYM_REGS_OK, (unsigned int *) &operands[numOps]);
	    if (result == SYM_REGISTER) {
	        isReg[numOps] = 1;
	    } else if (result == SYM_FREG_FOUND) {
		*p = savedChar;
		p = argStart[numOps];
		errMsg = "floating register invalid here";
		goto error;
	    } else if (result != SYM_FOUND) {
	        char *term;

/*	        if (Sym_EvalExpr(machPtr, localTbl, (char *) NULL, argStart[numOps],
		        sizeOnly, (int *) &operands[numOps], &term) != TCL_OK) {
*/
	        if ((result=Sym_EvalExpr(machPtr, localTbl, (char *) NULL, argStart[numOps],
		        sizeOnly, (int *) &operands[numOps], &term)) != TCL_OK) {
		    *p = savedChar;
		    p = argStart[numOps];
		    errMsg = "unrecognizable operand specifier";
		    goto error;
	        }
	        if (*term != 0) {
		    *p = savedChar;
		    p = term;
		    errMsg = "unknown garbage in expression";
		    goto error;
	        }
	    }
	}
	*p = savedChar;

	/*
	 * See if this is the last argument.  If not, skip over the
	 * separating comma.
	 */

about_to_continue:
	p = end;
	if (*p != ',') {
	    numOps++;
	    break;
	}
	if (numOps == 2) {
	    errMsg = "more than three operands";
	    goto error;
	}
	p++;
    }

    /*
     * Check argument count for propriety.
     */

    if ((numOps < minArgs[insPtr->class])
	    || (numOps > maxArgs[insPtr->class])) {
	if (minArgs[insPtr->class] == maxArgs[insPtr->class]) {
	    sprintf(msg, "wrong # operands (must be %d)",
		    minArgs[insPtr->class]);
	} else {
	    sprintf(msg, "wrong # operands (must be %d or %d)",
		    minArgs[insPtr->class], maxArgs[insPtr->class]);
	}
	p = argStart[0];
	errMsg = msg;
	goto error;
    }

    /*
     * Check immediate arguments for proper range.
     */

    if (insPtr->flags & (CHECK_LAST | CHECK_NEXT_TO_LAST | CHECK_FIRST)) {
	int i;

	if (insPtr->flags & CHECK_LAST) {
	    i = numOps-1;
	} else if (insPtr->flags & CHECK_FIRST) {
	    i = 0;
	} else {
	    i = numOps-2;
	}
	if (i >= 0) {
	    if (isReg[i]) {
		if (insPtr->flags & IMMEDIATE_REQ) {
		    p = argStart[i];
		    regIllegal:
		    errMsg = "register operand not allowed";
		    goto error;
		}
	    } else {
		int j, range_err;
		j = operands[i] & insPtr->rangeMask;

		if (j == 0) range_err = FALSE;
		else {
		    range_err = TRUE;

		    if (!(insPtr->flags & SIGN_EXTENDED))
			if (!((insPtr->rangeMask ^ operands[i]) &
			     insPtr->rangeMask)) range_err = FALSE;
			
		    if (j == insPtr->rangeMask) range_err = FALSE;
		}
		if (range_err) {
		    p = argStart[i];
		    sprintf(msg, "immediate operand 0x%x out of range",
			    operands[i]);
		    errMsg = msg;
		    goto error;
		}
	    }
	}
    }

    /*
     * Dispatch based on the class of instruction, and handle everything
     * else in a class-specific fashion.
     */

    *sizePtr = 1;
    switch (insPtr->class) {
	case NO_ARGS:
	    codePtr[0] = insPtr->op;
	    break;

	case LOAD:
	case FLOAD:
	    codePtr[0] = insPtr->op | (operands[0] << 16)
		    | (operands[1] & 0xffff) | (operands[2] << 21);
	    break;

	case STORE:
	case FSTORE:
	    codePtr[0] = insPtr->op | (operands[0] & 0xffff)
		    | (operands[1] << 21) | (operands[2] << 16);
	    break;

	case LUI:
	    codePtr[0] = insPtr->op | (operands[0] << 16)
		    | (operands[1] & 0xffff);
	    break;

	/*
	 * The main class of arithmetic instructions can get assembled
	 * in many different ways.  Most instructions can end using either
	 * the normal register-to-register opcode, or an immediate opcode,
	 * which is stored in insPtr->other.  If the instruction MUST use
	 * only the immediate form, a special value of insPtr->other
	 * indicates this fact.
	 */

	case ARITH:
	case FARITH:
	case MULDIV:
	    if (!isReg[0]) {
		p = argStart[0];
		regRequired:
		errMsg = "operand must be a register";
		goto error;
	    } else if (!isReg[1]) {
		p = argStart[1];
		goto regRequired;
	    }
	    if (insPtr->class == ARITH) {
		if (isReg[2]) {
		    codePtr[0] = insPtr->op | (operands[0] << 11)
			    | (operands[1] << 21) | (operands[2] << 16);
		} else if (insPtr->flags & IMMEDIATE_REQ) {
		    codePtr[0] = insPtr->op | (operands[0] << 16)
			    | (operands[1] << 21) | (operands[2] & 0xffff);
		} else {
		    codePtr[0] = insPtr->other | (operands[0] << 16)
			    | (operands[1] << 21) | (operands[2] & 0xffff);
		}
	    } else if ((insPtr->class == MULDIV) || (insPtr->class == FARITH)) {
		if (!isReg[2]) {
		    p = argStart[2];
		    goto regRequired;
		}
		codePtr[0] = insPtr->op | (operands[0] << 11)
			| (operands[1] << 21) | (operands[2] << 16);
	    }
	    break;

	/*
	 * Branches:  generate (and check) the branch displacement, which
	 * is done the same for all branch instructions.  Then handle
	 * different sub-classes differently.
	 */

	case JUMP:
	    if (isReg[0]) {	/* treat it like SRC1 */
		codePtr[0] = insPtr->other | (operands[0] << 21);
		break;
	    }
	    /* I know this falls through, this is just here to allow for
	     * the user to use j instead of jr for instance */
	case BRANCH_0_OP:
	case BRANCH_1_OP:
	case LABEL: {
	    int disp, mask;

	    if (isReg[numOps-1]) {
		p = argStart[numOps-1];
		goto regIllegal;
	    }
	    disp = operands[numOps-1];
	    if (disp & 0x3) {
		p = argStart[numOps-1];
		errMsg = "branch target not word-aligned";
		goto error;
	    }
	    disp = disp - (dot+4);
	    if ((insPtr->class == JUMP) || (insPtr->class == LABEL))
		mask = 0xfe000000;
	    else
		mask = 0xffff8000;
	    if ((disp & mask) && ((disp & mask) != mask)) {
		// badDisp:  esta etiqueta no se usa
		p = argStart[numOps-1];
		sprintf(msg, "branch target too far away (offset 0x%x)", disp);
		errMsg = msg;
		goto error;
	    }
	    if (insPtr->class == BRANCH_1_OP) {
		codePtr[0] = insPtr->op | (disp & 0xffff) | (operands[0] << 21);
	    } else if (insPtr->class == BRANCH_0_OP) {
		codePtr[0] = insPtr->op | (disp & 0xffff);
	    } else {  /* JUMP or LABEL */
		codePtr[0] = insPtr->op | (disp & 0x3ffffff);
	    }
	    break;
        }

	case TRAP:
	    if (isReg[0]) {
		p = argStart[0];
		goto regIllegal;
	    }
	    codePtr[0] = insPtr->op | (operands[0] & 0x3ffffff);
	    break;

	case SRC1:
	    if (!isReg[0]) {
		p = argStart[0];
		goto regRequired;
	    }
	    codePtr[0] = insPtr->op | (operands[0] << 21);
	    break;

	case MOVE:
		/* these are all R-type instructions */
	    if (!isReg[0]) {
		p = argStart[0];
		goto regRequired;
	    }
	    if (!isReg[1]) {
		p = argStart[1];
		goto regRequired;
	    }
	    codePtr[0] = insPtr->op | (operands[0] << 11) | (operands[1] << 21);
	    break;

	case FCOMPARE:
		/* these are all R-type instructions */
	    if (!isReg[0]) {
		p = argStart[0];
		goto regRequired;
	    }
	    if (!isReg[1]) {
		p = argStart[1];
		goto regRequired;
	    }
	    codePtr[0] = insPtr->op | (operands[0] << 21) | (operands[1] << 16);
	    break;

	default:
	    errMsg = "internal error:  unknown class for instruction";
	    goto error;
    }

    /*
     * Make sure that there's no garbage left on the line after the
     * instruction.
     */

    while (isspace(*p)) {
	p++;
    }
    if ((*p != 0) && (*p != ';')) {
	errMsg = "extra junk at end of line";
	goto error;
    }
    return TCL_OK;

    /*
     * Generate a reasonably-human-understandable error message.
     */

    error:
    IndicateError(machPtr->interp, errMsg, wholeLine, p);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Asm_Disassemble --
 *
 *	Given an instruction, return a string describing the instruction
 *	in assembler format.
 *
 * Results:
 *	The return value is a string, which either describes the
 *	instruction or contains a message saying that the instruction
 *	didn't make sense.  The string is statically-allocated, meaning
 *	that it will change on the next call to this procedure.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Asm_Disassemble(machPtr, ins, pc)
    DLX *machPtr;		/* Machine to use for symbol table info. */
    int ins;			/* The contents of the instruction. */
    unsigned int pc;		/* Memory address at which instruction is
				 * located. */
{
    register OpcodeInfo *opPtr;
    OpcodeInfo *bestPtr;
    static char string[200];
    int field, bestMask;
    int reg1, reg2;

    /*
     * Match this instruction against our instruction table to find
     * out what instruction it is.  Look
     */

    for (bestMask = 0, opPtr = opcodes; opPtr->name != NULL; opPtr++) {
	if (opPtr->mask == 0) {
	    continue;
	}
	if ((ins & opPtr->mask) != (opPtr->op)) {
	    continue;
	}
	if ((bestMask & opPtr->mask) != opPtr->mask) {
	    bestMask = opPtr->mask;
	    bestPtr = opPtr;
	}
    }
    if (bestMask == 0) {
	sprintf(string, "unrecognized instruction (0x%x)", ins);
	return string;
    }
    opPtr = bestPtr;

    /*
     * Dispatch on the type of the instruction.
     */

    switch (opPtr->class) {
	case NO_ARGS:
	    sprintf(string, "%s", opPtr->name);
	    break;

	case LOAD:
	    field = ins & 0xffff;
	    sprintf(string, "%s %s,%s(%s)", opPtr->name,
		    Asm_RegNames[(ins >> 16) & 0x1f],
		    Sym_GetString(machPtr, (unsigned) field),
		    Asm_RegNames[(ins >> 21) & 0x1f]);
	    break;

	case FLOAD:
	    field = ins & 0xffff;
	    sprintf(string, "%s %s,%s(%s)", opPtr->name,
		    Asm_RegNames[((ins >> 16) & 0x1f) + 32],
		    Sym_GetString(machPtr, (unsigned) field),
		    Asm_RegNames[(ins >> 21) & 0x1f]);
	    break;

	case STORE:
	    field = ins & 0xffff;
	    sprintf(string, "%s %s(%s),%s", opPtr->name,
		    Sym_GetString(machPtr, (unsigned) field),
		    Asm_RegNames[(ins >> 21) & 0x1f],
		    Asm_RegNames[(ins >> 16) & 0x1f]);
	    break;

	case FSTORE:
	    field = ins & 0xffff;
	    sprintf(string, "%s %s(%s),%s", opPtr->name,
		    Sym_GetString(machPtr, (unsigned) field),
		    Asm_RegNames[(ins >> 21) & 0x1f],
		    Asm_RegNames[((ins >> 16) & 0x1f) + 32]);
	    break;

	case LUI:
	    field = ins & 0xffff;
	    sprintf(string, "%s %s,0x%x", opPtr->name, 
		    Asm_RegNames[(ins >> 16) & 0x1f], field);
	    break;

	case ARITH:
	    if (opPtr->flags & IMMEDIATE_REQ) {
		field = ins & 0xffff;
		sprintf(string, "%s %s,%s,0x%x", opPtr->name,
			Asm_RegNames[(ins >> 16) & 0x1f],
			Asm_RegNames[(ins >> 21) & 0x1f], field);
	    } else {
		sprintf(string, "%s %s,%s,%s", opPtr->name,
			Asm_RegNames[(ins >> 11) & 0x1f],
			Asm_RegNames[(ins >> 21) & 0x1f],
			Asm_RegNames[(ins >> 16) & 0x1f]);
	    }
	    break;

	case FARITH:
	case MULDIV:
	    sprintf(string, "%s %s,%s,%s", opPtr->name,
		    Asm_RegNames[((ins >> 11) & 0x1f) + 32],
		    Asm_RegNames[((ins >> 21) & 0x1f) + 32],
		    Asm_RegNames[((ins >> 16) & 0x1f) + 32]);
	    break;

	case BRANCH_0_OP:
	    field = (ins & 0xffff);
	    if (field & 0x8000) {
		field |= 0xffff0000;
	    }
	    field += pc + 4;
	    sprintf(string, "%s %s", opPtr->name,
		    Sym_GetString(machPtr, (unsigned) field));
	    break;

	case BRANCH_1_OP:
	    field = (ins & 0xffff);
	    if (field & 0x8000) {
		field |= 0xffff0000;
	    }
	    field += pc + 4;
	    sprintf(string, "%s %s,%s", opPtr->name,
		    Asm_RegNames[(ins >> 21) & 0x1f],
		    Sym_GetString(machPtr, (unsigned) field));
	    break;

	case TRAP:
	    field = (ins & 0x3ffffff);
	    sprintf(string, "%s 0x%x", opPtr->name, field);
	    break;

	case LABEL:
	case JUMP:
	    field = (ins & 0x3ffffff);
	    if (field & 0x2000000) {
		field |= 0xfc000000;
	    }
	    field += pc + 4;
	    sprintf(string, "%s %s", opPtr->name,
		    Sym_GetString(machPtr, (unsigned) field));
	    break;

	case SRC1:
	    sprintf(string, "%s %s", opPtr->name,
		    Asm_RegNames[(ins >> 21) & 0x1f]);
	    break;
	
	case MOVE:
	case FCOMPARE:
		/* this is a R-type instruction */
	    reg1 = (ins >> 11) & 0x1f;
	    reg2 = (ins >> 21) & 0x1f;
	    if ((opPtr->flags & FIRST_F) || (opPtr->flags & FIRST_D))
		reg1 += 32;
	    if ((opPtr->flags & SECOND_F) || (opPtr->flags & SECOND_D))
		reg2 += 32;
	    sprintf(string, "%s %s,%s", opPtr->name,
		    Asm_RegNames[reg1],
		    Asm_RegNames[reg2]);
	    break;

	default:
	    sprintf(string, "instruction confused dis-assembler (0x%x)", ins);
	    break;
    }
    return string;
}

/*
 *----------------------------------------------------------------------
 *
 * Asm_AsmCmdOld --
 *
 *	This procedure is invoked to process the "asm" Tcl command.
 *	See the user documentation for details on what it does.
 *	This version does not work well.  See Asm_AsmCmd for details.
 *	It also expects a different format
 *
 *		asm instruction [address]
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Asm_AsmCmdOld(machPtr, interp, argc, argv)
    DLX *machPtr;			/* Machine description. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int result;
    unsigned int pc;
    char *end;
    int size, code[ASM_MAX_WORDS];

    if ((argc != 2) && (argc != 3)) {
	sprintf(interp->result,
		"wrong # args:  should be \"%.50s\" instruction [pc]",
		argv[0]);
	return TCL_ERROR;
    }

    if (argc == 3) {
	pc = strtoul(argv[2], &end, 0);
	if ((*end != 0) || (end == argv[2])) {
	    sprintf(interp->result, "bad pc \"%.50s\"", argv[2]);
	    return TCL_ERROR;
	}
    } else {
	pc = 0;
    }

    result = Asm_Assemble(machPtr, NULL, (char *) NULL, argv[1], pc, argv[1], 1,
	    &size, code);
    if (result != TCL_OK) {
	return result;
    }
    sprintf(interp->result, "0x%x", code[0]);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Asm_AsmCmd --
 *
 *	This procedure is invoked to process the "asm" Tcl command.
 *	See the user documentation for details on what it does.
 *	Unlike the original user documentation, the format of the
 * 	command is
 *
 *		asm [address] instruction
 *
 *	This is consistent with the other commands get, put, etc.
 *	It also makes parsing simpler.  If the first argument after
 *	"asm" is a number, it is taken to be an address.  Otherwise
 *	it is assumed to be an instruction mnemonic.  If "address"
 *	comes after "instruction", this routine is required to know
 *	too much about the format of assembly language statements.
 *	In particular, some assembly language statements have no
 *	operands (NOP) while others have multiple operands (ADD).
 *	Asm_Assemble deals with this, but it gets upset if there
 *	is an extra "address" on the line after the instruction.
 *	The problem is avoided by putting the address first.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Asm_AsmCmd(machPtr, interp, argc, argv)
    DLX *machPtr;			/* Machine description. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int result;
    int i, start;
    unsigned int pc;
    char *end;
    int size, code[ASM_MAX_WORDS];
    char buff[132];

    if (argc == 1) {
	sprintf(interp->result,
		"wrong # args:  should be \"%.50s\" [pc] instruction",
		argv[0]);
	return TCL_ERROR;
    }

    start = 1;

    pc = strtoul(argv[start], &end, 0);

    if (pc || *(end-1) == '0') {		/* Address given */
       start++;
       if (argc == 2) {
	  sprintf(interp->result,
		  "No instruc given:  should be \"%.50s\" [pc] instruction",
		  argv[0]);
	   return TCL_ERROR;
       }
    }

    strcpy (buff, argv[start++]);
    strcat (buff, "    ");
    for (i = start; i < argc; i++) {
       strcat (buff, argv[i]);
    }

    result = Asm_Assemble(machPtr, NULL, (char *) NULL, buff, pc, buff, 1,
	    &size, code);
    if (result != TCL_OK) {
	return result;
    }
    sprintf(interp->result, "0x%x", code[0]);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * alloc_asmHashTbl --
 *
 *	Initialize and/or get memory for asmHashTbl, an array
 *	of pointers to hash tables and their names.
 *
 *	ERA, 11-4-91:  Original
 *
 * Results:
 *	A standard Tcl return result.
 *
 * Side effects:
 *	Change asmHashTbl and allocate memory.
 *
 *----------------------------------------------------------------------
 */

int
alloc_asmHashTbl(machPtr)
    DLX *machPtr;		/* Machine for which to allocate */
{
    static int hash_tables_allocated;

    if (hash_tables_allocated == num_asm_files) {
	hash_tables_allocated += DEFAULT_NUM_ASM_FILES;
	if (!asmHashTbl) {
	    asmHashTbl = (asmHTbl *) malloc (
		       (hash_tables_allocated+1) * sizeof (asmHTbl));

	    if (!asmHashTbl) {
		printf("Could not allocate memory in alloc_asmHashTbl\n");
	        return TCL_ERROR;
	    }

	    asmHashTbl[0].tbl = &machPtr->symbols;
	    asmHashTbl[0].name = "";
	}
	else {
	    asmHashTbl = (asmHTbl *) realloc (asmHashTbl,
			(hash_tables_allocated+1) * sizeof (asmHTbl));

	    if (!asmHashTbl) {
		sprintf(machPtr->interp->result,
			"Could not allocate memory in alloc_asmHashTbl");
			return TCL_ERROR;
	    }
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Asm_LoadCmd --
 *
 *	This procedure is invoked to process the "load" Tcl command.
 *	See the user documentation for details on what it does.
 *
 *	ERA, 11-3-91:  Added support for local symbol tables.
 *
 * Results:
 *	A standard Tcl return result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Asm_LoadCmd(machPtr, interp, argc, argv)
    DLX *machPtr;		/* Machine whose memory should be loaded. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Count of number of files in argv. */
    char **argv;		/* Array containing names of files to
				 * assemble. */
{
    unsigned int codeStart, dataStart;
    char *p, *end;
    LoadInfo info;
    int i, j;
    Hash_Table **load_LocalTbl;
    //FILE * fichero;

    /*
     * Figure out the starting addresses for code and data (check for
     * variables "codeStart" and "dataStart", and use their values if
     * they're defined;  otherwise use defaults).
     */

    codeStart = 0x100;
    p = Tcl_GetVar(machPtr->interp, "codeStart", 1);
    if (*p != 0) {
	codeStart = strtoul(p, &end, 0);
	if (*end != 0) {
	    sprintf(machPtr->interp->result,
		    "\"codeStart\" variable doesn't contain an address: \"%.50s\"",
		    p);
	    return TCL_ERROR;
	}
    }
    dataStart = 0x1000;
    p = Tcl_GetVar(machPtr->interp, "dataStart", 1);
    if (*p != 0) {
	dataStart = strtoul(p, &end, 0);
	if (*end != 0) {
	    sprintf(machPtr->interp->result,
		    "\"dataStart\" variable doesn't contain an address: \"%.50s\"",
		    p);
	    return TCL_ERROR;
	}
    }

    load_LocalTbl = (Hash_Table **) malloc (argc * sizeof (Hash_Table *));
    if (!load_LocalTbl) {
	sprintf(machPtr->interp->result,
		"Could not allocate memory in Asm_Load_Cmd\n");
	free(load_LocalTbl); /* added Cecile */
	return TCL_ERROR;
    }

    /*
     * Pass 1: delete old symbol definitions.
     */

    for (i = 1; i < argc; i++) {
	Sym_DeleteSymbols(&machPtr->symbols, argv[i]);
	load_LocalTbl[i-1] = (Hash_Table *) NULL;
	if (asmHashTbl) {
	    for (j = 1; j <= num_asm_files; j++) {
	        if (!strcmp (asmHashTbl[j].name, argv[i])) {
	            Sym_DeleteSymbols(asmHashTbl[j].tbl, argv[i]);
		    load_LocalTbl[i-1] = asmHashTbl[j].tbl;
		    break;
		}
	    }
	}
    }

    /*
     * Pass 2: Create local symbol tables for any files we haven't
               seen previously.
     */

    for (i = 1; i < argc; i++) {
	if (!load_LocalTbl[i-1]) {

	   if (alloc_asmHashTbl (machPtr) != TCL_OK)
		return TCL_ERROR;

	   asmHashTbl[++num_asm_files].name = (char *) strdup (argv[i]);
	   asmHashTbl[num_asm_files].tbl = (Hash_Table *) 
					   calloc (1, sizeof (Hash_Table));
	   if (!asmHashTbl[num_asm_files].tbl) {
	       sprintf(machPtr->interp->result,
			"Could not allocate memory in Asm_Load_Cmd\n");
	       return TCL_ERROR;
	   }

	   load_LocalTbl[i-1] = asmHashTbl[num_asm_files].tbl;
	   Hash_InitTable(load_LocalTbl[i-1], 0, HASH_STRING_KEYS);
	}
    }

    /*
     * Pass 3:  read through all of the files to build the symbol tables.
     */

    info.codeAddr = codeStart;
    info.dataAddr = dataStart;
    info.message = NULL;
    info.end = NULL;
    info.totalBytes = 0;
    info.errorCount = 0;
    info.flags = ASM_SIZE_ONLY;
    for (i = 1; i < argc; i++) {
	ReadFile(argv[i], load_LocalTbl[i-1], machPtr, &info);
    }

    /*
     * Pass 4: read through the files a second time to actually assemble
     * the code.
     */

    info.codeAddr = codeStart;
    info.dataAddr = dataStart;
    info.flags = 0;
    for (i = 1; i < argc; i++) {
	ReadFile(argv[i], load_LocalTbl[i-1], machPtr, &info);
	if (info.errorCount > ASM_MAX_ERRORS) {
	    break;
	}
    }

    free (load_LocalTbl);

    if (do_malloc_init (machPtr, end_pgm_address))
        info.message = "";

    if (info.message == NULL) {
   /*     fichero = fopen("ficherotemporal.txt","wt");
        fprintf(fichero,"He estado por aquí\n");
        fclose(fichero);*/
	return TCL_OK;

    }
    Tcl_Return(machPtr->interp, info.message, TCL_DYNAMIC);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * ReadFile --
 *
 *	Read in an assembler file.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information gets loaded into *machPtr's memory, and *infoPtr
 *	gets modified (to point to an error message, for example).
 *
 *----------------------------------------------------------------------
 */

static void
ReadFile(fileName, localTbl, machPtr, infoPtr)
    char *fileName;		/* Name of assembler file to read. */
    Hash_Table *localTbl;	/* Local Symbol Table for this file */
    DLX *machPtr;		/* Machine into whose memory the information
				 * is to be loaded. */
    register LoadInfo *infoPtr;	/* Information about the state of the
				 * assembly process. */
{
#define MAX_LINE_SIZE 200
#define MAX_NAME_SIZE 10
    char line[MAX_LINE_SIZE];
    char pseudoOp[MAX_NAME_SIZE+1];
    FILE *f;
    register char *p;
    int i, nullTerm;
    int sizeOnly;
    char *end, *curToken;
    char savedChar;
    double strtod();

    /*
     * Non-zero means this is the first pass, so ignore errors and just
     * compute the size of the instruction.
     */
    sizeOnly = infoPtr->flags & ASM_SIZE_ONLY;

    f = fopen(fileName, "r");
    if (f == NULL) {
	if (infoPtr->flags & ASM_SIZE_ONLY) {
	    return;
	}
	sprintf(machPtr->interp->result,
		"couldn't open file \"%.50s\": %.100s", fileName,
		strerror(errno));
	AddErrMsg(machPtr->interp, infoPtr, 0);
	return;
    }

    /*
     * Process the file one line at a time.
     */

    infoPtr->file = fileName;
    infoPtr->dot = (infoPtr->codeAddr + 3) & ~3;
    infoPtr->flags |= ASM_CODE;
    for (infoPtr->lineNum = 1; ; infoPtr->lineNum++) {
	infoPtr->line = fgets(line, MAX_LINE_SIZE, f);
	if (infoPtr->line == NULL) {
	    if (!feof(f)) {
		sprintf(machPtr->interp->result, "error reading file: %.100s",
			strerror(errno));
		AddErrMsg(machPtr->interp, infoPtr, 1);
	    }
	    break;
	}

	/*
	 * Skip leading blanks.
	 */

	for (p = line; (*p == ' ') || (*p == '\t'); p++) {
	    /* Null body:  just skip spaces. */
	}

	/*
	 * Parse off an optional symbol at the beginning of the line.
	 * Note:  force symbol-related error messages to be output
	 * during pass 1, even though most other error messages get
	 * ignored during pass 1.
	 */

	if (isalpha(*p) || (*p == '_') || (*p == '$')) {
	    curToken = p;
	    for (p++; isalnum(*p) || (*p == '_') || (*p == '$'); p++) {
		/* Null body:  just skip past symbol. */
	    }
	    if (*p == ':') {
		*p = 0;
		if (infoPtr->flags & ASM_SIZE_ONLY) {
		    Sym_AddSymbol(machPtr, localTbl, fileName, curToken, infoPtr->dot, 0);
		    if (*machPtr->interp->result != 0) {
			AddErrMsg(machPtr->interp, infoPtr, 1);
		    }
		}
		*p = ':';
		p++;
	    } else {
		p = curToken;
	    }
	}

	/*
	 * Skip empty lines.
	 */

	while ((*p == ' ') || (*p == '\t')) {
	    p++;
	}
	if ((*p == '\n') || (*p == 0)) {
	    continue;
	}

	/*
	 * If this isn't an assembler pseudo-op, just assemble the
	 * instruction and move on.
	 */

	while ((*p == ' ') || (*p == '\t')) {
	    p++;
	}
	if (*p == ';') {
	    continue;
	}
	if (*p != '.') {
	    int size, code[ASM_MAX_WORDS], result;

	    infoPtr->dot = (infoPtr->dot + 3) & ~3;
	    result = Asm_Assemble(machPtr, localTbl, fileName, p, infoPtr->dot,
		    line, infoPtr->flags & ASM_SIZE_ONLY, &size, code);
	    if (result == TCL_OK) {
		result = StoreWords(machPtr, infoPtr->dot, code, size);
	    }
	    infoPtr->dot += size*4;
	    goto endOfLine;
	}

	/*
	 * Handle an assembler pseudo-op.
	 */

	curToken = p;
	for (i = 0, p++; (i < MAX_NAME_SIZE) && isalpha(*p); i++, p++) {
	    pseudoOp[i] = *p;
	}
	if (i >= MAX_NAME_SIZE) {
	    IndicateError(machPtr->interp, "pseudo-op name too long",
		    line, curToken);
	    goto endOfLine;
	}
	pseudoOp[i] = 0;
	while ((*p == ' ') || (*p == '\t')) {
	    p++;
	}
	if ((pseudoOp[0] == 'a') && (strcmp(pseudoOp, "align") == 0)) {
	    if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly, &i, &end) != TCL_OK) {
		IndicateError(machPtr->interp, machPtr->interp->result,
			line, p);
		goto endOfLine;
	    }
	    p = end;
	    if (i == 0) {
		machPtr->interp->result = "\".align 0\" not supported";
		goto endOfLine;
	    } else {
		i = (1 << i) - 1;
		infoPtr->dot = (infoPtr->dot + i) & ~i;
	    }
	} else if ((pseudoOp[0] == 'a') && (strcmp(pseudoOp, "ascii") == 0)) {
	    nullTerm = 0;

	    /*
	     * Read one or more ASCII strings from the input line.  Each
	     * must be surrounded by quotes, and they must be separated
	     * by commas.
	     */

	    doString:
	    while (1) {
		while ((*p == ' ') || (*p == '\t')) {
		    p++;
		}
		if (*p != '"') {
		    IndicateError(machPtr->interp,
			    "missing \" at start of string", line, p);
		    goto endOfLine;
		}
		p++;
		i = Gp_PutString(machPtr, p, '"', infoPtr->dot, nullTerm, &end);
		if (infoPtr->dot + i > end_pgm_address)
		    end_pgm_address = infoPtr->dot + i;
		if (*end != '"') {
		    IndicateError(machPtr->interp,
			    "missing \" at end of string", line, end-1);
		    goto endOfLine;
		}
		p = end+1;
		infoPtr->dot += i;
		while ((*p == ' ') || (*p == '\t')) {
		    p++;
		}
		if (*p != ',') {
		    break;
		}
		p++;
	    }
	} else if ((pseudoOp[0] == 'a') && (strcmp(pseudoOp, "asciiz") == 0)) {
	    nullTerm = 1;
	    goto doString;
	} else if ((pseudoOp[0] == 'b') && (strcmp(pseudoOp, "byte") == 0)) {
	    while (1) {
		curToken = p;
		if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly, &i, &end)
			!= TCL_OK) {
		    IndicateError(machPtr->interp, machPtr->interp->result,
			    line, p);
		    goto endOfLine;
		}
		Gp_PutByte(machPtr, infoPtr->dot, i);
		infoPtr->dot += 1;
		if (infoPtr->dot > end_pgm_address)
		    end_pgm_address = infoPtr->dot;
		for (p = end; (*p == ' ') || (*p == '\t'); p++) {
		    /* Null body;  just skip space. */
		}
		if (*p != ',') {
		    break;
		}
		p++;
	    }
	} else if ((pseudoOp[0] == 'd') && (strcmp(pseudoOp, "data") == 0)) {
	    if (infoPtr->flags & ASM_CODE) {
		infoPtr->codeAddr = infoPtr->dot;
	    } else {
		infoPtr->dataAddr = infoPtr->dot;
	    }
	    if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly, &i, &end) != TCL_OK) {
		Tcl_Return(machPtr->interp, (char *) NULL, TCL_STATIC);
	    } else {
		p = end;
		infoPtr->dataAddr = i;
	    }
	    infoPtr->dot = infoPtr->dataAddr;
	    infoPtr->flags &= ~ASM_CODE;
	} else if ((pseudoOp[0] == 'g') && (strcmp(pseudoOp, "global") == 0)) {
	    if (!isalpha(*p) && (*p != '_')) {
		IndicateError(machPtr->interp,
			"symbol name must start with letter or '_'", line, p);
		goto endOfLine;
	    }
	    curToken = p;
	    while (isalnum(*p) || (*p == '_') || (*p == '$')) {
		p++;
	    }
	    savedChar = *p;
	    *p = 0;
	    if (infoPtr->flags & ASM_SIZE_ONLY) {
		Sym_AddSymbol(machPtr, localTbl, fileName, curToken, 0,
			SYM_GLOBAL|SYM_NO_ADDR);
		if (*machPtr->interp->result != 0) {
		    AddErrMsg(machPtr->interp, infoPtr, 1);
		}
	    }
	    *p = savedChar;
	} else if ((pseudoOp[0] == 's') && (strcmp(pseudoOp, "space") == 0)) {
	    if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly, &i, &end) != TCL_OK) {
		IndicateError(machPtr->interp, machPtr->interp->result,
			line, p);
		goto endOfLine;
	    }
	    p = end;
	    while (i > 0) {
		Gp_PutByte(machPtr, infoPtr->dot, 0);
		infoPtr->dot += 1;
		if (infoPtr->dot > end_pgm_address)
		    end_pgm_address = infoPtr->dot;
		i -= 1;
	    }
	} else if ((pseudoOp[0] == 't') && (strcmp(pseudoOp, "text") == 0)) {
	    if (infoPtr->flags & ASM_CODE) {
		infoPtr->codeAddr = infoPtr->dot;
	    } else {
		infoPtr->dataAddr = infoPtr->dot;
	    }
	    if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly, &i, &end) != TCL_OK) {
		Tcl_Return(machPtr->interp, (char *) NULL, TCL_STATIC);
	    } else {
		p = end;
		infoPtr->codeAddr = i;
	    }
	    infoPtr->dot = infoPtr->codeAddr;
	    infoPtr->flags |= ASM_CODE;
	} else if ((pseudoOp[0] == 'w') && (strcmp(pseudoOp, "word") == 0)) {
	    while (1) {
		curToken = p;
		if (Sym_EvalExpr(machPtr, localTbl, fileName, p, sizeOnly, &i, &end)
			!= TCL_OK) {
		    IndicateError(machPtr->interp, machPtr->interp->result,
			    line, p);
		    goto endOfLine;
		}
		infoPtr->dot = (infoPtr->dot + 3) & ~3;
		(void) StoreWords(machPtr, infoPtr->dot, &i, 1);
		infoPtr->dot += 4;
		for (p = end; (*p == ' ') || (*p == '\t'); p++) {
		    /* Null body;  just skip space. */
		}
		if (*p != ',') {
		    break;
		}
		p++;
	    }
	} else if ((pseudoOp[0] == 'f') && (strcmp(pseudoOp, "float") == 0)) {
	    while (1) {
		float f;
		int *pi = (int *)&f;

		curToken = p;
		f = strtod(p, &end);
		if (p == end) {
		    IndicateError(machPtr->interp, "illegal floating number", line, p);
		    goto endOfLine;
		}
		infoPtr->dot = (infoPtr->dot + 3) & ~3;
		(void) StoreWords(machPtr, infoPtr->dot, pi, 1);
		infoPtr->dot += 4;
		for (p = end; (*p == ' ') || (*p == '\t'); p++) {
		    /* Null body;  just skip space. */
		}
		if (*p != ',') {
		    break;
		}
		p++;
	    }
	} else if ((pseudoOp[0] == 'd') && (strcmp(pseudoOp, "double") == 0)) {
	    while (1) {
		double d;
		int *pi = (int *)&d;

		curToken = p;
		d = strtod(p, &end);
		if (p == end) {
		    IndicateError(machPtr->interp, "illegal double number", line, p);
		    goto endOfLine;
		}
		infoPtr->dot = (infoPtr->dot + 3) & ~3;
		(void) StoreWords(machPtr, infoPtr->dot, pi, 2);
		infoPtr->dot += 8;
		for (p = end; (*p == ' ') || (*p == '\t'); p++) {
		    /* Null body;  just skip space. */
		}
		if (*p != ',') {
		    break;
		}
		p++;
	    }
	} else {
	    IndicateError(machPtr->interp, "unknown pseudo-op", line,
		    curToken);
	    goto endOfLine;
	}

	/*
	 * Check for extraneous garbage at the end of the line.
	 */

	while (isspace(*p)) {
	    p++;
	}
	if ((*p != '#') && (*p != 0)) {
	    IndicateError(machPtr->interp, "extra junk at end of line",
		    line, p);
	}

	/*
	 * Done with the line.  If there has been an error, add it onto
	 * the list of error messages that has accumulated during the
	 * assembly.  Increase the storage allocated to error messages
	 * if necessary to accommodate the new message.
	 */

	endOfLine:
	if (*machPtr->interp->result != 0) {
	    if (infoPtr->flags & ASM_SIZE_ONLY) {
		Tcl_Return(machPtr->interp, (char *) NULL, TCL_STATIC);
	    } else {
		AddErrMsg(machPtr->interp, infoPtr, 1);
		if (infoPtr->errorCount > ASM_MAX_ERRORS) {
		    goto endOfFile;
		}
	    }
	}
    }

    endOfFile:
    fclose(f);
    if (infoPtr->flags & ASM_CODE) {
	infoPtr->codeAddr = infoPtr->dot;
    } else {
	infoPtr->dataAddr = infoPtr->dot;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * AddErrMsg --
 *
 *	Given an error message in an interpreter, add it onto a list of
 *	error messages being accumulated for an assembly and clear the
 *	interpreter's message.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The message is added to the list of messages in infoPtr, and
 *	the interpreter's result is re-initialized.
 *
 *----------------------------------------------------------------------
 */

static void
AddErrMsg(interp, infoPtr, addHeader)
    Tcl_Interp *interp;			/* Interpreter containing error
					 * message. */
    register LoadInfo *infoPtr;		/* State of assembly, to which error
					 * message is to be added. */
    int addHeader;			/* Non-zero means tack on message
					 * header identifying file and line
					 * number. */
{
    int length, hdrLength, totalLength;
    char header[100];

    length = strlen(interp->result);
    if (length == 0) {
	return;
    }
    if (addHeader) {
	sprintf(header, "%.50s(%d): ", infoPtr->file,
		infoPtr->lineNum);
    } else {
	header[0] = 0;
    }
    hdrLength = strlen(header);
    totalLength = hdrLength + length + 2;

    /*
     * Grow the error message area if the current area isn't large
     * enough.
     */

    if (totalLength > ((infoPtr->message + infoPtr->totalBytes)
	    - (infoPtr->end + 1))) {
	char *newMsg;

	if (infoPtr->totalBytes == 0) {
	    infoPtr->totalBytes = 4*totalLength;
	} else {
	    infoPtr->totalBytes = 2*(infoPtr->totalBytes + totalLength);
	}
	newMsg = calloc(1, (unsigned) infoPtr->totalBytes);
	if (infoPtr->message != NULL) {
	    strcpy(newMsg, infoPtr->message);
	    infoPtr->end += newMsg - infoPtr->message;
	} else {
	    infoPtr->end = newMsg;
	}
	infoPtr->message = newMsg;
    }
    if (infoPtr->end != infoPtr->message) {
	*infoPtr->end = '\n';
	infoPtr->end += 1;
    }
    sprintf(infoPtr->end, "%s%s", header, interp->result);
    infoPtr->end += hdrLength + length;
    infoPtr->errorCount += 1;

    Tcl_Return(interp, (char *) NULL, TCL_STATIC);
}

/*
 *----------------------------------------------------------------------
 *
 * StoreBytes --
 *
 *	Place a given range of bytes in the memory of a machine.
 *
 * Results:
 *	A standard Tcl result (normally TCL_OK plus empty string);  error
 *	information is returned through machPtr->interp.
 *
 * Side effects:
 *	MachPtr's memory is modified to hold new information.
 *
 *----------------------------------------------------------------------
 */

static int
StoreWords(machPtr, address, wordPtr, numWords)
    register DLX *machPtr;		/* Machine into which to store. */
    unsigned int address;		/* Word-aligned byte address in
					 * machine's memory. */
    int *wordPtr;			/* Words to store into machine's
					 * memory. */
    int numWords;			/* Number of words to store. */
{
    int index;
    register MemWord *memPtr;

    for ( ; numWords > 0; wordPtr++, address += 4, numWords--) {
	index = ADDR_TO_INDEX(address);
	if (index >= machPtr->numWords) {
	    sprintf(machPtr->interp->result,
		    "can't store at address 0x%x:  no such memory location",
		    address);
	    return TCL_ERROR;
	}
	memPtr = machPtr->memPtr + index;
	memPtr->value = *wordPtr;
	memPtr->opCode = OP_NOT_COMPILED;
    }

    if (address > end_pgm_address) end_pgm_address = address;

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * IndicateError --
 *
 *	Generate an error message that also points out the position
 *	in a string where the error was detected.
 *
 * Results:
 *	There is no return value.  Interp's result is modified to hold
 *	errMsg followed by string, with position pos highlighted in
 *	string.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
IndicateError(interp, errMsg, string, pos)
    Tcl_Interp *interp;		/* Interpreter to hold error message.  The
				 * result area must be in the initial
				 * empty state. */
    char *errMsg;		/* Message describing the problem. */
    char *string;		/* Input string that contained the problem. */
    char *pos;			/* Location in string of the character where
				 * problem was detected. */
{
    int msgLength, stringLength;
    char *newMsg;

    msgLength = strlen(errMsg);
    stringLength = strlen(string);
    if (string[stringLength-1] == '\n') {
	stringLength -= 1;
    }

    /*
     * Always allocate new storage for the new message.  This is needed
     * because (a) the space required may exceed the size of the static
     * result buffer, and (b) "errMsg" may actually be in the static
     * buffer so we have to be careful not to trash it while generating
     * the new message.
     */

    newMsg = calloc(1, (unsigned) (msgLength + stringLength + 10));
    sprintf(newMsg, "%s: %.*s => %.*s", errMsg, pos-string,
	    string, stringLength - (pos-string), pos);
    Tcl_Return(interp, newMsg, TCL_DYNAMIC);
}




