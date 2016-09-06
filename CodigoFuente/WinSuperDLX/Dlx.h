/* 
Este fichero es parte de WinSuperDLX, el simulador interactivo de la
    m√°quina DLX 

    Copyright (C) 2002 Mario David Barrag√°n Garc√≠a (jabade@supercable.com)
                 Antonio Jes√∫s Contreras S√°nchez   (anjeconsa@yahoo.es)
                Emilio Jos√© Garnica L√≥pez   (ejgl@hotmail.com)
                Julio Ortega Lopera         (julio@atc.ugr.es)

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los t√©rminos de la Licencia P√∫blica General GNU publicada 
    por la Fundaci√≥n para el Software Libre, ya sea la versi√≥n 3 
    de la Licencia, o (a su elecci√≥n) cualquier versi√≥n posterior.

    Este programa se distribuye con la esperanza de que sea √∫til, pero 
    SIN GARANT√çA ALGUNA; ni siquiera la garant√≠a impl√≠cita 
    MERCANTIL o de APTITUD PARA UN PROP√ìSITO DETERMINADO. 
    Consulte los detalles de la Licencia P√∫blica General GNU para obtener 
    una informaci√≥n m√°s detallada. 

    Deber√≠a haber recibido una copia de la Licencia P√∫blica General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.

*/
/* AÒadido */

#ifndef DLX_H
#define DLX_H

/*
    Copyright (C) 1993 McGill University.
    Copyright (C) 1993 ACAPS Group.
    Copyright (C) 1993 Cecile Moura

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * dlx.h --
 *
 * Declarations of structures used to simulate a superscalar architecture.
 *
 * Copyright 1989 Regents of the University of California
 *
 * $Header: /auto/home/acaps/u1/cecile/xsimul/RCS/dlx.h,v 1.9 93/01/30 19:19:16
 * cecile Exp Locker: cecile $ SPRITE (Berkeley)
 *
 * Incorporates additions from Cecile Moura to add data structures for
 * superscalar dlx
 */

/*
 * $Id: dlx.h,v 1.16 93/04/23 12:52:48 cecile Exp $ $Log:
 * dlx.h,v $ Revision 1.9  93/01/30  19:19:16  cecile revised branch
 * prediction: a new field is added in the reorder buffer structure, giving
 * the instruction count of the next predicted branch that will execute. This
 * field is used at ResultCommit of supersim.c
 *
 * Revision 1.8  92/09/28  14:29:34  cecile Version with a new structure for
 * Load and Store Buffers: there is now a single load buffer for both integer
 * and floating poit instead of two. Same thing for Store buffer. Besides,
 * instead of having growing linked lists for the load/store buffers we
 * maintainstatic arrays which embed a free and occupied list. The main
 * reason for adopting a single buffer for both integer and fp loads and a
 * single buffer for integer and fp store was to maintain a strict order in
 * memory accesses, and to ensure that loads correctly bypassed stores. In
 * fact integer and fp load/store can manipulate the same memory area, so
 * treating them separately lead to problem (ex with benchmarks like
 * tomcatv).
 *
 * Revision 1.7  92/08/27  14:27:35  cecile Implementation of the Branch Target
 * Buffer
 *
 * Revision 1.6  92/08/26  10:18:12  cecile New Data structure for the
 * instruction queue, with a free list and an occupied list.
 *
 * Revision 1.5  92/08/24  14:30:47  cecile New data structure for the
 * instruction window, so as to avoid too many mallocs. The window will be a
 * list that is allocated a constant space. with a pointer to the first free
 * slot of the list (free list) and pointers to the head an tail element of
 * the list. This is to replace the shrinkable and expandable previous list
 *
 * Revision 1.4  92/08/16  12:40:12  cecile Version of supersim that works for
 * programs such as fib, queen, quickrand... That is traps have been
 * implemented, recursive calls are handled correctly. But there is no branch
 * prediction (the branch target buffer is not used). When a branch is
 * encountered, fetch stops and branch is simply executed at execute stage
 *
 * Revision 1.3  92/08/09  18:22:07  cecile Implementation of branches.
 * Implementation of the functional units (reservation stations etc..)
 *
 * Revision 1.2  92/06/11  14:49:30  cecile The structures for the superscalar
 * architecture have been changed again, so as to be more "generic". One more
 * level of indirection is necessary to reach the buffers and windows
 *
 * Revision 1.1  92/06/07  16:01:37  cecile Initial revision
 *
 */


#ifndef _MIPS
#define _MIPS

#define MEMSIZE  66000			/* bytes*/


#ifndef _TCL
#include "tcl/tcl.h"  //cambiado
#endif
#ifndef _HASH
#include "hash.h"
#endif

/*
 * Each memory word is represented by a structure of the following format. In
 * order to interpret instructions efficiently, they get decoded into several
 * fields on the first execution after each change to the word.
 */

typedef struct {
    int value;			/* Contents of the memory location. */
    char opCode;		/* Type of instruction.  see #defines below
				 * for details. */
    char rs1, rs2, rd;		/* Three registers from instruction. */
    int extra;			/* Immediate or target or shamt field or
				 * offset.  Immediates are sign-extended. */
} MemWord;


/*
 * The functional units description for the Superscalar dlx: defines each
 * type of unit
 */

typedef struct {
    int latency;		/* latency of execution in the unit */
    int num_units;		/* number of available units of this type */
    int num_used;		/* number of unit currently in used */
} FunctionalUnit;

/*
 * Declaration of the Branch Target Buffer (BTB) elements for Superscalar dlx
 */
/*
typedef struct {
    unsigned int address;	/* pc of the branch *
    int predictState;		/* two bit prediction number:TAKEN TAKEN?
				 * NOT_TAKEN NOT_TAKEN (see superdlx.h) *
    unsigned int predictTarget;
} BranchInfo;
*/
#define BR_DEFAULT_COUNT 1  //valor por defecto de contadores para la prediccÌon de salto
#define BR_DEFAULT_HISTORY_BITS 2
#define BR_MAX_BITS 16//cada posiciÛn guarda un estado (2bit): 8 contadores de 2 bit
typedef struct {
    unsigned int address;	/* pc of the branch */
    int predictState[BR_MAX_BITS];		/* two bit prediction number:TAKEN TAKEN?  este es el vector que guardar· los contadores
				 * NOT_TAKEN NOT_TAKEN (see superdlx.h) */
    int predictCounter[BR_MAX_BITS]; //aÒadido despuÈs: indicar· el contador; el +1 es para hacer el desplazamiento a la derecha de forma comoda
//    int countNumber; //indicar· el n˙mero de contadores que hay (hasta 16)
    unsigned int predictTarget;
    int lastSecuence;//guardar· la ˙ltima secuencia de salto, y que me servir· para hacer la predicciÛn: es un valor decimal
  //	int currentIns; //indice de la instrucciÛn actual y que es por donde va a insertar la predicciÛn hecha
} BranchInfo;

/*
 * The instruction queue, where the fetched instructions are placed by the
 * fetch process
 */

typedef struct InstructionElement {
    MemWord *wordPtr;		/* memory word fetched */
    unsigned int address;	/* PC of the instruction */
    unsigned int insCount;	/* dynamic instruction count */
    unsigned int cycleCount;	/* clock cycle when the instruction was
				 * fetched */
    int prediction;		/* prediction information taken from the BTB:
				 * TAKEN or NOT_TAKEN */
    unsigned int bbEntry;	/* branch target buffer entry */
    struct InstructionElement *nextPtr;
    struct InstructionElement *prevPtr;
} InstructionElement;

typedef struct {
    int numEntries;		/* size of the instruction queue */
    int entryCount;		/* number of occupied entries */
    InstructionElement *table;
    InstructionElement *freeList;	/* list of free slots */
    InstructionElement *lastElement;	/* tail of the occupied list */
    InstructionElement *firstElement;	/* head of the occupied list */
} InstructionQueue;

/* description of an operand: its value, type and validity */
typedef struct {
    long value[2];		/* value: two integer are used for doubles */
    int type;			/* type: INTEGER, FP_SIMPLE, FP_DOUBLE,
				 * IMMEDIATE, NONE */
    int valid;			/* indicates if the value is available (i.e
				 * has been computed) */
} Operand;


/*
 * the load and store buffer structures, where decoded stores/loads are
 * placed in order, before being released to the data cache
 */
typedef struct DataOp {
    char opCode;		/* opcode number (see #defines below) */
    unsigned int address;	/* address of the data to write/read */
    unsigned int pcAddress; 	/* address of the instruction*/
    Operand *dataToStore;	/* data to be written to memory (for stores) */
    int reorderEntry;		/* corresponding reorder buffer entry number */
    unsigned int insCount;
    int released;		/* indicates if the data operation can
				 * proceed to memory */
    int flush;			/* indicates if the operation has been
				 * flushed (i.e follows a wrong branch
				 * prediction */
    int loadBlck;		/* indicates if the load is blocked, because
				 * it cannot bypass a store which has the
				 * same memory address (for loads only, and
				 * for statistics) */
    struct DataOp *nextPtr;
    struct DataOp *prevPtr;
} DataOp;			/* describes a load/store operation */

typedef struct {
    int numEntries;
    int entryCount;
    DataOp *table;
    DataOp *freeList;
    DataOp *lastElement;
    DataOp *firstElement;
} DataBuffer;			/* the load or store buffer list */

/*
 * The instruction window structures, from where instructions are issued out
 * of order
 */


typedef struct WindowElement {
    char opCode;		/* opcode number */
    int classe;			/* instruction classe, see the opInfo table in
				 * supersim.c */
    int unit;			/* operation unit */
    unsigned int address;	/* instruction pc */
    int cycleDecode;		/* clock cycle when the instruction was
				 * decoded */
    int reorderEntry;		/* instruction reorder buffer entry number */
    int prediction;		/* prediction made at fetch stage (for
				 * branches) */
    unsigned int bbEntry;	/* branch target buffer entry (for branches) */
    int otherRentry;		/* entry in the other reorde buffer: for
				 * branches only */
    Operand *firstOperand;
    Operand *secondOperand;
    DataOp *dataOp;
    struct WindowElement *nextPtr;
    struct WindowElement *prevPtr;
} WindowElement;		/* an operation in the instruction window */


typedef struct {
    int numEntries;		/* size of the buffer */
    int entryCount;		/* number of elements */
    WindowElement *table;
    WindowElement *freeList;
    WindowElement *firstElement;
    WindowElement *lastElement;
} InstructionWindow;		/* the instruction window header */



/*
 * The reorder buffer entries, where instructions are allocated in order by
 * the decoder
 */

typedef struct TReorderElement{

    char opCode;
    int unit;
    int classe;
    int registerNb;		/* result register number */
    long result[2];		/* result value (two int for the doubles) */
    int type;			/* type of the result: NONE, INTEGER,
				 * FP_SIMPLE, FP_DOUBLE */
    int valid;			/* validity of the result: indicates if the
				 * computation is complete */
    int ready;			/* clock cycle when the result is available */
    unsigned int insCount;	/* dynamic instruction count */
    unsigned int cycleCount;	/* cycle count when the instruction was
				 * fetched */
    int flush;			/* flush indicator */
    struct TReorderElement *otherReorder;	/* for branches only */
    DataOp *dataOp;		/* load/store buffer entry               */


    struct TReorderElement *nextPtr;
    struct TReorderElement *prevPtr;

} ReorderElement;

/*
 * The reorder buffer, implemented as a circular queue with a head and tail.
 */

typedef struct {
    int numEntries;
    int entryCount;
    int tail;
    int head;
    int checkOps;		/* number of operation currently in
				 * execution*/
    int ready; 			/* gives the sonnest completion time*/
    ReorderElement *listOfExec;
    ReorderElement *element;
} ReorderBuffer;

#define NUM_OPS         50	/* estimated maximum number of operations in
				 * execution in one clock cycle       */

#define NUM_RENAME     200

#define NUM_INT_REGS 32
#define NUM_FP_REGS 32
#define NUM_SPECIAL_REGS 3
#define TOTAL_REGS (NUM_INT_REGS + NUM_FP_REGS + NUM_SPECIAL_REGS)
#define NUM_GPRS 64
#define PC_REG (NUM_GPRS)
#define NEXT_PC_REG (NUM_GPRS+1)
#define FP_STATUS (NUM_GPRS + 2)

typedef struct {
    unsigned int fetched;	/* number of fetched instructions        */
    unsigned int decoded;	/* number of decoded instructions        */
    unsigned int issued[3];	/* number of issued instructions (ALL,
				 * INTEGER, FLOAT) */
    unsigned int commited[3];	/* number of commited instructions (ALL,
				 * INTEGER, FLOAT) */
    unsigned int uselessWrites[3];/* useless writes, i.e that could be avoided
				 * because the same destination register
				 * appears again in the reorder buffer */
    unsigned int writes[3];	/* number of writes to registers */
    unsigned int searchedOp[3];	/* number of operands seached in the reorder
				 * buffer or the register file */
    unsigned int renames[3];	/* number of renamed operands, i.e taken from
				 * the reorder buffer */
   int flushed;			/* number of flushed instructions        */
   int branches;		/* number of branches                    */
   int correctBranch;		/* number of correct branch prediction  */
   int branchYes;		/* number of taken branches              */
   int correctBranchYes;	/* number of branches which are correctly
				 * predicted taken                       */
    int branchNo;		/* number of not taken branches          */
    int correctBranchNo;	/* number of branches which are correctly
				 * predicted not taken                   */
    int lostCycles;		/* number of cycles lost cycles due to  wrong
				 * prediction: the  fetch process fetches the
				 * wrong path            */
    int fetchStalls;		/* number of cycles where the fetch process
				 * do nothing                    */
    int decodeStalls;
    int branchFetchStalls;	/* number of fetch stalls due to a branch
				 * (the branch prediction mechanism is off)                                      */
    int fullBufferStalls;	/* number of fetch stalls due to  full
				 * buffers                               */
    int branchCollisions;
    int regWrites[TOTAL_REGS];	/* number of pending writes to a register i.e
				 * : number of instances of the register in
				 * the reorder buffer */
    int numRename[3][NUM_RENAME];	/* renaming frequency for ALL,
					 * INTEGER FLOAT. ex: if
					 * numRename[FLOAT][10] = 30, this
					 * means that, 10 operand renamings
					 * have taken place  in each of 30
					 * cycles of simulation */
    int numSearched[3][NUM_RENAME];	/* searching frequency for ALL,
					 * INTEGER, FLOAT operands: this is
					 * to compare with the frequency of
					 * renamed operands. */
    int numIssued[3][NUM_OPS];	/* issue frequency for ALL, FLOAT, INTEGER */
    int numCommit[3][NUM_OPS];	/* commit frequency for ALL, FLOAT, INTEGER */
    int numWait[3][NUM_OPS];	/* records how many cycles instructions wait
				 * in the instruction window */
    int intBufferOcc[2][11];	/* occupancy rate of the integer instruction
				 * window and reorder buffer : WINDOW and
				 * REORDER */
    int fpBufferOcc[2][11];	/* occupancy ratio of the floating point
				 * window and reorder buffer */
    int loadBlck;		/* how many loads are blocked because they
				 * cannot bypass stores */
    int numLoads;		/* number of loads sent to cache */


} Statistics;

#define MAX_FP_UNITS  9		/* number of floating  point functional unit. */
#define MAX_INT_UNITS 7		/* idem for integer fus */
#define MAX_UNITS  MAX_FP_UNITS + MAX_INT_UNITS



#define CYC_CNT_RESET 16384	/* How often to reset the cycle count to
				 * prevent wrap around problems. not
				 * implemented in superdlx yet */
#define FD_SIZE 32		/* Number of simulated file descriptors */



/*
 * OpCode values for MemWord structs.  These are straight from the MIPS
 * manual except for the following special values:
 *
 * OP_NOT_COMPILED -    means the value of the memory location has changed so
 * the instruction needs to be recompiled. OP_UNIMP -           means that
 * this instruction is legal, but hasn't been implemented in the simulator
 * yet. OP_RES -             means that this is a reserved opcode (it isn't
 * supported by the architecture).
 */

#define OP_ADD           1
#define OP_ADDI          2
#define OP_ADDU          3
#define OP_ADDUI         4
#define OP_AND           5
#define OP_ANDI          6
#define OP_BEQZ          7
#define OP_BFPF          8
#define OP_BFPT          9
#define OP_BNEZ          10
#define OP_J             11
#define OP_JAL           12
#define OP_JALR          13
#define OP_JR            14
#define OP_LB            15
#define OP_LBU           16
#define OP_LD            17
#define OP_LF            18
#define OP_LH            19
#define OP_LHI           20
#define OP_LHU           21
#define OP_LW            22
#define OP_MOVD          23
#define OP_MOVF          24
#define OP_MOVFP2I       25
#define OP_MOVI2FP       26
#define OP_MOVI2S        27
#define OP_MOVS2I        28
#define OP_OR            29
#define OP_ORI           30
#define OP_RFE           31
#define OP_SB            32
#define OP_SD            33
#define OP_SEQ           34
#define OP_SEQI          35
#define OP_SEQU          36
#define OP_SEQUI         37
#define OP_SF            38
#define OP_SGE           39
#define OP_SGEI          40
#define OP_SGEU          41
#define OP_SGEUI         42
#define OP_SGT           43
#define OP_SGTI          44
#define OP_SGTU          45
#define OP_SGTUI         46
#define OP_SH            47
#define OP_SLE           48
#define OP_SLEI          49
#define OP_SLEU          50
#define OP_SLEUI         51
#define OP_SLL           52
#define OP_SLLI          53
#define OP_SLT           54
#define OP_SLTI          55
#define OP_SLTU          56
#define OP_SLTUI         57
#define OP_SNE           58
#define OP_SNEI          59
#define OP_SNEU          60
#define OP_SNEUI         61
#define OP_SRA           62
#define OP_SRAI          63
#define OP_SRL           64
#define OP_SRLI          65
#define OP_SUB           66
#define OP_SUBI          67
#define OP_SUBU          68
#define OP_SUBUI         69
#define OP_SW            70
#define OP_TRAP          71
#define OP_XOR           72
#define OP_XORI          73
#define OP_NOP           74

#define OP_ADDD          80
#define OP_ADDF          81
#define OP_CVTD2F        82
#define OP_CVTD2I        83
#define OP_CVTF2D        84
#define OP_CVTF2I        85
#define OP_CVTI2D        86
#define OP_CVTI2F        87
#define OP_DIV           88
#define OP_DIVD          89
#define OP_DIVF          90
#define OP_DIVU          91
#define OP_EQD           92
#define OP_EQF           93
#define OP_GED           94
#define OP_GEF           95
#define OP_GTD           96
#define OP_GTF           97
#define OP_LED           98
#define OP_LEF           99
#define OP_LTD           100
#define OP_LTF           101
#define OP_MULT          102
#define OP_MULTD         103
#define OP_MULTF         104
#define OP_MULTU         105
#define OP_NED           106
#define OP_NEF           107
#define OP_SUBD          108
#define OP_SUBF          109

/* special "opcodes", give these values after the above op values */

#define OP_NOT_COMPILED  111
#define OP_UNIMP         112
#define OP_RES           113
#define OP_LAST          114

typedef struct {
    Tcl_Interp *interp;		/* Interpreter associated with machine (used
				 * for interpreting commands, returning
				 * errors, etc.) */
    int numWords;		/* Number of words of memory simulated for
				 * this machine. */
    int numChars;		/* Number of characters of memory */
    MemWord *memPtr;		/* Array of MemWords, sufficient to provide
				 * memSize bytes of storage. */
    char *memScratch;		/* Scratchpad for use by trap handlers */
    char *endScratch;		/* Pointer to the end of the scratch pad */
    int fd_map[FD_SIZE];	/* maps simulated fd's to actual fd's. mainly
				 * protects the real standard input, output,
				 * and error from the program.  A value of -1
				 * means that fd is not currently in use.  */
    int regs[TOTAL_REGS];	/* General-purpose registers, followed by hi
				 * and lo multiply-divide registers, followed
				 * by program counter and next program
				 * counter.  Both pc's are stored as indexes
				 * into the memPtr array. */
    unsigned int badPC;		/* If an addressing error occurs during
				 * instruction fetch, this value records the
				 * bad address.  0 means no addressing error
				 * is pending. */
    /* not used by super-dlx, but should be */
    int addrErrNum;		/* If badPC is non-zero, this gives the
				 * serial number (insCount value) of the
				 * instruction after which the addressing
				 * error is to be registered. */
    /* not used by super-dlx */
    Hash_Table symbols;		/* Records addresses of all symbols read in
				 * by assembler for machine. */
    unsigned int insCount;	/* Count of total # of instructions executed
				 * in this machine (i.e. serial number of
				 * current instruction). */
    unsigned int cycleCount;	/* Keep track of how many cycles have been
				 * executed to monitor floating point units. */
    int state;			/* indicates end of cycle or end of
				 * simulation                            */
    int stopFetch;		/* flag for the fetch process */

    /* characteristics of the machine behaviour */

    int numFetch;		/* number of instructions fetched in one
				 * clock cycle */
    int numDecode;		/* number of instructions decoded on one
				 * clock cycle */
    int numCommit;		/* number of instructions that commit
				* from each reorder buffer, on each cc */
    int bb_size;
    int branchPred;

    /* buffer and queues of the superscalar machine */

    BranchInfo *branchBuffer;	/* Branch buffer array */
    // en la estructura de la m·quina se guarda el esquema de predicciÛn de saltos  que se va a seguir
    int branchSchema[2];//={BR_DEFAULT_HISTORY_BITS,BR_DEFAULT_COUNT}; //guarda un par que es en primer lugar el n∫ de bits de historia y en 2∫ el n∫ de contadores
								  //inicializado al esquema que traia el superdlx original
								  //se cambiar· en funciÛn de  lo que elija el usuario

    InstructionQueue *insQueue;	/* Instruction queue between fetch and
				 * decode. */

    InstructionWindow *iWindow;	/* Integer instruction window. */

    InstructionWindow *fpWindow;/* Floating point instruction window. */

    ReorderBuffer *iReorderBuffer;	/* Integer reorder buffer . */

    ReorderBuffer *fpReorderBuffer;	/* Floating Point reorder buffer. */

    DataBuffer *storeBuffer;	/* Integer Store Buffer. */
    DataBuffer *loadBuffer;	/* Integer Load Buffer */

    /* functional units */

    int num_mem_access;		/* number of data accesses allowed on a given
				 * clock cycle    */
    int mem_latency;
    FunctionalUnit intUnits[MAX_UNITS];
    FunctionalUnit fpUnits[MAX_UNITS];


    int FPstatusReg;		/* Floating point status register.  Written
				 * on floating point compares, read on BFPT
				 * and BFPF instructions. */

    int numTrap;


    Statistics *counts;

} DLX;

DLX *machPtr;
/*
 * Conversion between DLX addresses and indexes, which are stored in
 * pc/nextPc fields of DLX structures and also used to address the memPtr
 * values:
 */

#define INDEX_TO_ADDR(index)    ((unsigned) (index << 2))
#define ADDR_TO_INDEX(addr)     (addr >> 2)

/*
 * Miscellaneous definitions:
 */

#define SIGN_BIT        0x80000000
#define R31             31


/*
 * Other procedures that are exported from one file to another:
 */


#endif				/* _MIPS */

#endif

