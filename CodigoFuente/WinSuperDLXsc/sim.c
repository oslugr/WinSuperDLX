
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
 * sim.c --
 *
 *
 * Copyright 1989 Regents of the University of California
 *
 * This file contains the functions initializing the super-dlx machine
 * structure: Sim_Init and Sim_Create
 * It also contains all the functions that are called when the user enters
 * the on-line commands such as "step", "go", "next" etc.. 
 * Finally, utility functions to compile, read/write memory, multiply have 
 * been reused from dlxsim.
 *
 */
static char rcsid[] = "$Id: sim.c,v 1.19 1993/12/05 01:09:12 lozano Exp lozano $";
/* $Log: sim.c,v $
 * Revision 1.19  1993/12/05  01:09:12  lozano
 * added HELP_PATH environment variable
 *
 * Revision 1.18  93/04/22  18:54:33  cecile
 * fixing the memory size
 * 
 * Revision 1.17  93/04/14  00:43:23  cecile
 * *** empty log message ***
 * 
 * Revision 1.16  93/04/13  23:42:35  cecile
 * *** empty log message ***
 * 
 * Revision 1.15  93/04/13  14:05:29  cecile
 * Supersimulator source file
 * 
 * Revision 1.14  93/04/11  21:07:18  cecile
 * now the memory size (mem_size) is in byte
 * stats fixed (put unsigned for big numbers)
 * 
 * Revision 1.13  93/04/09  19:39:05  cecile
 * *** empty log message ***
 *
 * Revision 1.12  93/04/01  18:10:48  cecile
 * integration with X version
 * 
 * Revision 1.11  93/03/22  19:54:07  cecile
 * Revision of the address/bus error functions.
 * New structure: the list of pending operation.
 * New item in the machine configuration file: the branch target buffer size
 * Completion of the stats display.
 * 
 * Revision 1.10  93/02/16  11:56:29  cecile
 * new stats command
 * 
 * Revision 1.9  93/02/03  12:25:56  cecile
 * New command implemented: reset. It cleans the entire machine memory so we 
 * do not have to quit to reload files.
 * 
 * Revision 1.8  93/01/30  19:22:12  cecile
 * New commands have been added: next, print, help, stats... and step and go
 * have been revised
 *
 * Revision 1.7  92/09/28  14:37:19  cecile
 * Neww structure for load and store buffers: now a single buffer is used 
 * for integer and floating point loads, and a single buffer is used for integer
 * and floating point stores
 * 
 * Revision 1.6  92/08/27  14:32:05  cecile
 * Implementation of the Branch Target Buffer.
 * 
 * Revision 1.5  92/08/26  10:22:47  cecile
 * New data structure for instruction queue.
 *
 * Revision 1.4  92/08/24  14:38:39  cecile
 * Changes in the structure of the instruction window: so the initializatio
 * phase is different.
 * 
 * Revision 1.3  92/08/16  12:57:33  cecile
 * Part of the supersim version that handles traps and works for recursive 
 * programs
 * 
 * Revision 1.2  92/06/08  11:55:59  cecile
 * changed the structures of superscalar dlx
 * 
 * Revision 1.1  92/06/07  17:17:50  cecile
 * Initial revision: adaptation of simCreate to new superscalar structures.
 * contains the simulation functions of dlxsim which are reused for superscalar
 * dlx. The function Simulate, that is the core of the simulator has to be 
 * rewritten: it has thus been removed from here and the new code of this 
 * function can be found in supersim.c.

 * 
 * Revision 1.1  92/06/03  15:36:11  cecile
 * Initial revision
 * 
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include "tcl\tcl.h"
#include "asm.h"
#include "dlx.h"
#include "sym.h"
#include "superdlx.h"
#include "extern.h"    // variables globales

/* 
 * the tables below are reused from dlxsim,  they are only used to
 * compile instructions (see function Compile)
 */
/*
 * The table below is used to translate bits 31:26 of the instruction
 * into a value suitable for the "opCode" field of a MemWord structure,
 * or into a special value for further decoding.
 */

#define SPECIAL 120
#define FPARITH	121

#define IFMT 1
#define JFMT 2
#define RFMT 3


typedef struct {
    int opCode;		/* Translated op code. */
    int format;		/* Format type (IFMT or JFMT or RFMT) */
} OpInfo;

/*
 * LAL, Nov/93, Swap the values for OP_BFPF, OP_BFPT, It seems that they were
 * wrong since the dlxsim
 */
   
OpInfo opTable[] = {
    {SPECIAL, RFMT}, {FPARITH, RFMT}, {OP_J, JFMT}, {OP_JAL, JFMT},
    {OP_BEQZ, IFMT}, {OP_BNEZ, IFMT}, {OP_BFPF, IFMT}, {OP_BFPT, IFMT},
    {OP_ADDI, IFMT}, {OP_ADDUI, IFMT}, {OP_SUBI, IFMT}, {OP_SUBUI, IFMT},
    {OP_ANDI, IFMT}, {OP_ORI, IFMT}, {OP_XORI, IFMT}, {OP_LHI, IFMT},
    {OP_RFE, IFMT}, {OP_TRAP, IFMT}, {OP_JR, IFMT}, {OP_JALR, IFMT},
    {OP_SLLI, IFMT}, {OP_RES, IFMT}, {OP_SRLI, IFMT}, {OP_SRAI, IFMT},
    {OP_SEQI, IFMT}, {OP_SNEI, IFMT}, {OP_SLTI, IFMT}, {OP_SGTI, IFMT},
    {OP_SLEI, IFMT}, {OP_SGEI, IFMT}, {OP_RES, IFMT}, {OP_RES, IFMT},
    {OP_LB, IFMT}, {OP_LH, IFMT}, {OP_RES, IFMT}, {OP_LW, IFMT},
    {OP_LBU, IFMT}, {OP_LHU, IFMT}, {OP_LF, IFMT}, {OP_LD, IFMT},
    {OP_SB, IFMT}, {OP_SH, IFMT}, {OP_RES, IFMT}, {OP_SW, IFMT},
    {OP_RES, IFMT}, {OP_RES, IFMT}, {OP_SF, IFMT}, {OP_SD, IFMT},
    {OP_SEQUI, IFMT}, {OP_SNEUI, IFMT}, {OP_SLTUI, IFMT}, {OP_SGTUI, IFMT},
    {OP_SLEUI, IFMT}, {OP_SGEUI, IFMT}, {OP_RES, IFMT}, {OP_RES, IFMT},
    {OP_RES, IFMT}, {OP_RES, IFMT}, {OP_RES, IFMT}, {OP_RES, IFMT},
    {OP_RES, IFMT}, {OP_RES, IFMT}, {OP_RES, IFMT}, {OP_RES, IFMT}
};

/*
 * the table below is used to convert the "funct" field of SPECIAL
 * instructions into the "opCode" field of a MemWord.
 */

int specialTable[] = {
    OP_NOP, OP_RES, OP_RES, OP_RES, OP_SLL, OP_RES, OP_SRL, OP_SRA,
    OP_RES, OP_RES, OP_RES, OP_RES, OP_TRAP, OP_RES, OP_RES, OP_RES,
    OP_SEQU, OP_SNEU, OP_SLTU, OP_SGTU, OP_SLEU, OP_SGEU, OP_RES, OP_RES,
    OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
    OP_ADD, OP_ADDU, OP_SUB, OP_SUBU, OP_AND, OP_OR, OP_XOR, OP_RES,
    OP_SEQ, OP_SNE, OP_SLT, OP_SGT, OP_SLE, OP_SGE, OP_RES, OP_RES,
    OP_MOVI2S, OP_MOVS2I, OP_MOVF, OP_MOVD, OP_MOVFP2I, OP_MOVI2FP,
    OP_RES, OP_RES,
    OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES
};

/*
 * the table below is used to convert the "funct" field of FPARITH
 * instructions into the "opCode" field of a MemWord.
 */
int FParithTable[] = {
  OP_ADDF, OP_SUBF, OP_MULTF, OP_DIVF, OP_ADDD, OP_SUBD, OP_MULTD, OP_DIVD,
  OP_CVTF2D, OP_CVTF2I, OP_CVTD2F, OP_CVTD2I, OP_CVTI2F, OP_CVTI2D, 
  OP_MULT, OP_DIV,
  OP_EQF, OP_NEF, OP_LTF, OP_GTF, OP_LEF, OP_GEF, OP_MULTU, OP_DIVU,
  OP_EQD, OP_NED, OP_LTD, OP_GTD, OP_LED, OP_GED, OP_RES, OP_RES,
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES
  };
/* operationNames removed */

/* ERA, 11-3-91 dlxsim :  Added this variable */
 extern  Hash_Table *curr_LocalTbl;

/*
 * Forward declarations for procedures defined later in this file:
 */

static void ResetCounts();


void *mymalloc(size)
 long size ;
 {
       void * s;
       
       s = (void *) calloc(size,sizeof(char)) ;
       if(s == NULL)
       {
          printf("\n Not Enough Memory to Allocate\n") ;
          exit(0) ;
       }
       return s ;
 }

 /* Añadido ********************************************************* */
 /*
 * ----------------------------------------------------------------------
 *
 * Main_QuitCmd --
 *
 * This procedure is invoked to process the "quit" Tcl command. Reused from
 * dlxsim.
 *
 * Results: None:  this command never returns.
 *
 * Side effects: The program exits.
 *
 * ----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Main_QuitCmd(machPtr, interp, argc, argv)
    DLX *machPtr;		/* Machine description. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    printf("\n");
    exit(0);
    return TCL_OK;		/* Never gets executed. */
}


/* @Añadido ********************************************************* */
/*
 * ----------------------------------------------------------------------
 * Sim_Init --
 *
 *    	Initializes the machine with the default parameters from
 *	superdlx.h. This is before reading the machine configuration
 *	file. If a machine configuration file is given, then the
 * 	default parameters will be overwritten (see read.c)
 *
 * Results:
 * 	the superscalar machine structure
 * Called by:
 *	main in main.c
 *	xVersion in xmain.c
 * ----------------------------------------------------------------------
 */


DLX *Sim_Init(memSize, branchPred)
     int memSize;
     int branchPred;
{
  
    register DLX *machPtr; 
    int i, numWords;
    FunctionalUnit *unit;
    ReorderElement *reorder;
    WindowElement *element;
    InstructionElement *instruction;
    DataOp *dataOp;
    //BranchInfo *branch;

    machPtr = (DLX *) calloc(1, sizeof(DLX));
    numWords = (memSize + 3) >> 2;
    machPtr->numWords = (numWords +3) & ~0x3;
    machPtr->numChars = machPtr->numWords * 4;

    machPtr-> numFetch = NUM_FETCH;
    machPtr-> numDecode = NUM_DECODE;
    machPtr->numCommit = NUM_COMMIT;
    machPtr->branchPred = branchPred;
    machPtr->bb_size = NUM_BB_ENTRIES;
   /* sizes of queues, window, buffers .. have default parameters */
    machPtr->insQueue = (InstructionQueue *)calloc(1,sizeof(InstructionQueue));
    machPtr->insQueue->numEntries = NUM_INS_QUEUE;


    machPtr->iWindow = (InstructionWindow *)calloc(1,sizeof(InstructionWindow));
    machPtr->iWindow->numEntries = NUM_I_WINDOW;

	
    machPtr->fpWindow =(InstructionWindow *)calloc(1,sizeof(InstructionWindow));
    machPtr->fpWindow->numEntries = NUM_FP_WINDOW;
  
    machPtr->iReorderBuffer = (ReorderBuffer *)calloc(1,sizeof(ReorderBuffer));
    machPtr->iReorderBuffer->numEntries = NUM_I_REORDER;
    machPtr->fpReorderBuffer= (ReorderBuffer *)calloc(1,sizeof(ReorderBuffer));
    machPtr->fpReorderBuffer->numEntries= NUM_FP_REORDER;
  
    machPtr->storeBuffer = (DataBuffer *)calloc(1,sizeof(DataBuffer));
    machPtr->storeBuffer->numEntries = NUM_STORE_BUFF;

    machPtr->loadBuffer = (DataBuffer *)calloc(1,sizeof(DataBuffer));
    machPtr->loadBuffer->numEntries = NUM_LOAD_BUFF;
 
    machPtr->num_mem_access = NUM_MEM_ACCESS;
    machPtr->mem_latency = MEM_LATENCY;

    /* functional units  latencies and numbers*/
    for(unit=&machPtr->intUnits[0],i=0;i<MAX_INT_UNITS;++unit,++i) {
	switch(i){
	    case INT_ALU:
			unit->latency = INT_ALU_LATENCY;
			unit->num_units = NUM_ALU_UNITS;
			break;
	    case INT_SHIFT:
			unit->latency = INT_SHIFT_LATENCY;
			unit->num_units = NUM_SHIFT_UNITS;
			break;
	    case INT_DATA:
			unit->latency = INT_ALU_LATENCY;
			unit->num_units = NUM_DATA_UNITS;
			break;
	    case INT_COMP:
			unit->latency = INT_COMP_LATENCY;
			unit->num_units = NUM_COMP_UNITS;
			break;
	    case INT_BRANCH:
			unit->latency = INT_ALU_LATENCY;
			unit->num_units = NUM_BRANCH_UNITS;
			break;
	    default:
			unit->latency = 0;
			unit->num_units = 0;
			break;
	  }
    }


    for(unit=&machPtr->fpUnits[0],i=0;i<MAX_FP_UNITS;++unit,++i) {
	switch(i){
   	    case FP_ADD:
		unit->latency = FP_ADD_LATENCY;
		unit->num_units = NUM_FADD_UNITS;
		break;
	    case FP_DIV:
			unit->latency = FP_DIV_LATENCY;
			unit->num_units = NUM_FDIV_UNITS;
			break;
	    case FP_MUL:
			unit->latency = FP_MUL_LATENCY;
			unit->num_units = NUM_FMUL_UNITS;
			break;
	    case FP_CONV:
			unit->latency = FP_CONV_LATENCY;
			unit->num_units = NUM_FCONV_UNITS;
			break;
	    case FP_DATA:
			unit->latency = INT_ALU_LATENCY;
			unit->num_units = NUM_FDATA_UNITS;
			break;
	    case FP_COMP:
			unit->latency = FP_COMP_LATENCY;
			unit->num_units = NUM_FCOMP_UNITS;
			break;
	    case FP_BRANCH:
			unit->latency = INT_ALU_LATENCY;
			unit->num_units = NUM_FBRANCH_UNITS;
			break;
	    default:
			unit->latency = 0;
			unit->num_units = 0;
			break;	
	}
    }

    machPtr->numTrap = 0;
    

    return machPtr;
}


/*
 * ----------------------------------------------------------------------
 * Create_Interp -- 
 *
 *    	Link the superdlx user commands with the Tcl-interpretor
 *
 * Results:
 *	None
 *
 * Called by:
 *	main in main.c
 *	xVersion in xmain.c
 * ----------------------------------------------------------------------
 */


void Create_Interp(machPtr,interp) 
    DLX *machPtr;
    Tcl_Interp *interp;
{
    extern int Main_QuitCmd();

    machPtr->interp = interp;


    Tcl_CreateCommand(interp, "asm", Asm_AsmCmd, (ClientData) machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "go", Sim_GoCmd, (ClientData) machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "help", Sim_HelpCmd, (ClientData) machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "load", Asm_LoadCmd, (ClientData) machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp,"next",Sim_NextCmd,(ClientData) machPtr,
		      (void (*)()) NULL);		
    Tcl_CreateCommand(interp,"print",Sim_InspectCmd,(ClientData)machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "quit", Main_QuitCmd, (ClientData) machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "reset", Sim_ResetCmd, (ClientData) machPtr,
		      (void (*)()) NULL);    
    Tcl_CreateCommand(interp, "stats", Sim_DumpStats, (ClientData) machPtr,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "step", Sim_StepCmd, (ClientData) machPtr,
		      (void (*)()) NULL);


}


/*
 *----------------------------------------------------------------------
 *
 * Sim_Create --
 *
 *	Completes the description the superscalar machine. In particular,
 * 	the contents of the buffers and quesus are allocated and 
 *	initialized, as well as statistics counts
 *	This procedure is invoked at the beginning of the simulation (main.c)
 *	and/or when the machine is reset
 *
 * Results:
 *	The return value is a pointer to the description of the super-dlx
 *	machine.
 *
 * Called by:
 *	main in main.c
 *	xVersion in xmain.c
 *----------------------------------------------------------------------
 */

void
Sim_Create(machPtr, memory)
    DLX *machPtr;
    int memory;

{
  /*  int memSize;*/
    register MemWord *wordPtr;
    int i;
    FunctionalUnit *unit;
    ReorderElement *reorder;
    WindowElement *element;
    InstructionElement *instruction;
    DataOp *dataOp;
    BranchInfo *branch;
    FILE *fp;


    if (memory) {
	/* I multiply by 1.5, to avoid problems (like addresses referenced
	    out of memory bounds) */
	/*machPtr->numWords = (int) (machPtr->numWords * 1.5);*/
    	machPtr->memPtr = (MemWord *)
	    calloc(1, (unsigned) (sizeof(MemWord) * machPtr->numWords));
    	for (i = machPtr->numWords, wordPtr = machPtr->memPtr; i > 0;
	    i--, wordPtr++) {
	    wordPtr->value = 0;
	    wordPtr->opCode = OP_NOT_COMPILED;
        }
        machPtr->memScratch = (char *) malloc (machPtr->numChars);

        machPtr->endScratch = machPtr->memScratch + machPtr->numChars;

        for (i = 0; i < NUM_GPRS; i++) {
	    machPtr->regs[i] = 0;
        }

    }

    machPtr->regs[PC_REG] = 0;
    machPtr->regs[NEXT_PC_REG] = 1;
    machPtr->badPC = 0;
    machPtr->addrErrNum = 0;
    machPtr->insCount = 0;
    Hash_InitTable(&machPtr->symbols, 0, HASH_STRING_KEYS);
    alloc_asmHashTbl (machPtr);			/* ERA, 11-4-91 */
    Init_Handle_Trap(machPtr);

    machPtr->FPstatusReg = 0;

    machPtr->cycleCount = 0;

   machPtr->stopFetch = FETCH;
   machPtr->state = CYCLE_PROCESS;

   /* branch target buffer */

    machPtr->branchBuffer = (BranchInfo *)
            calloc(1, (unsigned) (sizeof(BranchInfo) * machPtr->bb_size));
    /*
    for (i=0, branch = machPtr->branchBuffer;i< (machPtr->bb_size);i++, branch++) {
    	branch->address = 0;
    	branch->predictState = TAKEN_;
    	branch->predictTarget = 0;
    } */
//guarda un par que es en primer lugar el nº de bits de historia y en 2º el nº de contadores
    machPtr->branchSchema[0]=BR_DEFAULT_HISTORY_BITS;
    machPtr->branchSchema[1]=BR_DEFAULT_COUNT;

    for (i=0, branch = machPtr->branchBuffer;i< (machPtr->bb_size);i++, branch++) {
        branch->address = 0;
        /*
        for ( j=0;j<BR_MAX_BITS ;j++ )
	{
                branch->predictCounter[j]=NOT_TAKEN;//inicializamos a 0
		branch->predictState[j] = TAKEN_;//inicializamos
        }
        branch->lastSecuence=NOT_TAKEN;//inicializamos la última secuencia como todo 0
        branch->countNumber=BR_DEFAULT_COUNT; //asignamos al valor estandar
        */
        brInit(branch,machPtr->branchSchema); //esta función hace exactamente lo que el comentario anterior
  	branch->predictTarget = 0;

    }

    /* Instruction Queue */
    machPtr->insQueue->lastElement = NULL;
    machPtr->insQueue->firstElement = NULL;
    machPtr->insQueue->entryCount = 0;
    machPtr->insQueue->table = (InstructionElement *)
			calloc(1,(unsigned)(sizeof(InstructionElement) *
			(machPtr->insQueue->numEntries)));


    machPtr->insQueue->freeList = machPtr->insQueue->table;


    for (i=0, instruction=machPtr->insQueue->freeList;
	i<machPtr->insQueue->numEntries;
  	i++,instruction++) {
	instruction->prevPtr = NULL;
	if (i!= (machPtr->insQueue->numEntries - 1))
	    instruction->nextPtr = &machPtr->insQueue->freeList[i+1];
	else
	    instruction->nextPtr = NULL;
    }

    /* Integer Instruction Window */
    machPtr->iWindow ->lastElement= NULL;
    machPtr->iWindow->firstElement = NULL;
    machPtr->iWindow->entryCount = 0;
    machPtr->iWindow->table = (WindowElement *)
		calloc(1,(unsigned)(sizeof(WindowElement) *
			(machPtr->iWindow->numEntries)));

    machPtr->iWindow->freeList = machPtr->iWindow->table;

    for (i=0, element=machPtr->iWindow->freeList;
	i<machPtr->iWindow->numEntries;
	i++,element++) {

	element->prevPtr = NULL;
	if (i != (machPtr->iWindow->numEntries - 1))
	    element->nextPtr = &machPtr->iWindow->freeList[i+1];
	else
	    element->nextPtr = NULL;
	element->firstOperand = (Operand *)mymalloc(sizeof(Operand));
    	element->secondOperand = (Operand *)mymalloc(sizeof(Operand));

    }

    /* Fp Instruction Window*/
    machPtr->fpWindow->lastElement = NULL;
    machPtr->fpWindow->firstElement = NULL;
    machPtr->fpWindow->entryCount = 0;
    machPtr->fpWindow->table = (WindowElement *)
		calloc(1,(unsigned)(sizeof(WindowElement) *
			(machPtr->fpWindow->numEntries)));

    machPtr->fpWindow->freeList = machPtr->fpWindow->table;

    for (i=0, element=machPtr->fpWindow->freeList;
	i<machPtr->fpWindow->numEntries;
	i++,element++) {

	element->prevPtr = NULL;
	if (i != (machPtr->fpWindow->numEntries - 1))
	    element->nextPtr = &machPtr->fpWindow->freeList[i+1];
	else
	    element->nextPtr = NULL;
	element->firstOperand = (Operand *)mymalloc(sizeof(Operand));
    	element->secondOperand = (Operand *)mymalloc(sizeof(Operand));

    }

    /* Integer Reorder Buffer */
    machPtr->iReorderBuffer->entryCount = 0;
    machPtr->iReorderBuffer->element = (ReorderElement *)
	    calloc(1, (unsigned) (sizeof(ReorderElement) *
			 (machPtr->iReorderBuffer->numEntries + 1)));

    machPtr->iReorderBuffer->tail = 0;
    machPtr->iReorderBuffer->head = 0;
    machPtr->iReorderBuffer->checkOps = 0;
    machPtr->iReorderBuffer->listOfExec = NULL;
    for (i=machPtr->iReorderBuffer->numEntries + 1,
           reorder=machPtr->iReorderBuffer->element;i>0;
           i--,reorder++){
      reorder->registerNb = -1;
      reorder->result[1] = 0;
      reorder->result[0] = 0;
      reorder->valid = 0;
      reorder->flush = 0;
      reorder->otherReorder = NULL;
      reorder->nextPtr = NULL;
      reorder->prevPtr = NULL;

      }

   /* fp reorder buffer */
    machPtr->fpReorderBuffer->entryCount= 0;
    machPtr->fpReorderBuffer->checkOps = 0;
    machPtr->fpReorderBuffer->element =  (ReorderElement *)
	    calloc(1, (unsigned) (sizeof(ReorderElement) *
		(machPtr->fpReorderBuffer->numEntries + 1)));

    machPtr->fpReorderBuffer->tail = 0;
    machPtr->fpReorderBuffer->head =0;
    machPtr->fpReorderBuffer->checkOps = 0;
    machPtr->fpReorderBuffer->listOfExec = NULL;
    for (i=machPtr->fpReorderBuffer->numEntries +  1,
           reorder=machPtr->fpReorderBuffer->element;i>0;i--,reorder++){
      	reorder->registerNb = -1;
      	reorder->result[1] = 0;
      	reorder->result[0] = 0;
      	reorder->valid = 0;
      	reorder->flush = 0;
      	reorder->otherReorder = NULL;
        reorder->nextPtr = NULL;
        reorder->prevPtr = NULL;
   }

    /* store buffer */
    machPtr->storeBuffer->entryCount = 0;
    machPtr->storeBuffer->firstElement = NULL;
    machPtr->storeBuffer->lastElement = NULL;
    machPtr->storeBuffer->table = (DataOp *)
		 calloc (1, (unsigned)(sizeof(DataOp) *
			(machPtr->storeBuffer->numEntries)));
    machPtr->storeBuffer->freeList = machPtr->storeBuffer->table;
    for (i=0, dataOp = machPtr->storeBuffer->freeList;
	i<machPtr->storeBuffer->numEntries;
	i++,dataOp++) {

	dataOp->prevPtr = NULL;
	if (i != (machPtr->storeBuffer->numEntries - 1))
	    dataOp->nextPtr = &machPtr->storeBuffer->freeList[i+1];
	else
	    dataOp ->nextPtr = NULL;
	dataOp->dataToStore = (Operand *) mymalloc(sizeof(Operand));
    }

    /* load buffer */
    machPtr->loadBuffer->entryCount = 0;
    machPtr->loadBuffer->firstElement = NULL;
    machPtr->loadBuffer->lastElement = NULL;
    machPtr->loadBuffer->freeList = (DataOp *)
		 calloc (1, (unsigned)(sizeof(DataOp) *
			(machPtr->loadBuffer->numEntries)));

    for (i=0, dataOp = machPtr->loadBuffer->freeList;
	i<machPtr->loadBuffer->numEntries;
	i++,dataOp++) {

	dataOp->prevPtr = NULL;
	if (i != (machPtr->loadBuffer->numEntries - 1))
	    dataOp->nextPtr = &machPtr->loadBuffer->freeList[i+1];
	else
	    dataOp->nextPtr = NULL;
	dataOp->dataToStore = NULL;
    }

    /* functional Units */

    for(unit=&machPtr->intUnits[0],i=0;i<MAX_INT_UNITS;++unit,++i) {
	unit->num_used=0;
    }

    for(unit=&machPtr->fpUnits[0],i=0;i<MAX_FP_UNITS;++unit,++i) {
	unit->num_used=0;
    }

    /* counts  do calloc instead..*/
    machPtr->counts = (Statistics *) calloc(1,sizeof(Statistics));

    machPtr->numTrap = 0;


    /* ERA, 11-3-91:  Added 2nd NULL parm. */
    Sym_AddSymbol(machPtr, NULL, "", "memSize", mem_size, SYM_GLOBAL);

}

/*
 *----------------------------------------------------------------------
 *
 * Sim_NextCmd --
 *
 *	This procedure is invoked to process the "next" Tcl command.
 *	The "next" command permits to execute a specified number of
 *	cycles or the number of cycles necessary to execute a specified
 *	number of instructions
 *
 * Results:
 *	A standard Tcl result.
 *
 * Comments:
 * 	New command for superdlx
 *----------------------------------------------------------------------
 */

int 
Sim_NextCmd(machPtr, interp, argc, argv)
    DLX *machPtr;
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */

{

    char *p ;
    int i, count;


    /* parse the command line: (superdlx)next -i# or (superdlx)next -c# */
    if (argc == 2) {
    	for (p = argv[1], i=0; *p != 0 ; p++,i++) {
	    if (isdigit(*p)) {
	    	char *end;
	    	count = strtoul(p,&end,10);
	    	p = end -1;	    
	     } else if (*p == 'i') {
	    	return(Simulate(machPtr,interp, count, INSTRUCTION));
	     } else if (*p == 'c') {
	    	return(Simulate(machPtr,interp, count, CYCLE));
	     } else {
	    /*error*/
	    	sprintf(interp->result, "bad flag \"%c\" in \"%.50s\" command",
			*p, argv[0]);
	    	return TCL_ERROR;
	     }
    	 }

    sprintf(interp->result, "bad flag in \"%.50s\" command", argv[0]);
    return TCL_ERROR;

    } else if (argc == 1) {
	return (Simulate(machPtr,interp,1,CYCLE));
    } else {
	sprintf(interp->result,
		"wrong # args: should be \n    \"%.50s <number of cycles><'c'>\"\n    or \"%.50s <number of instructions><'i'>\"\n    \"%.50s\"\n",
		 argv[0], argv[0],argv[0]);
	return TCL_ERROR;

    }	
}   

/*
 *----------------------------------------------------------------------
 *
 * Sim_InspectCmd --
 *
 *	This procedure is invoked to process the "print" command, which
 *	inspects the various buffers, queues, windows and functional 
 *	units of the superscalar machine . See print.hlp for more 
 *	details
 *
 * Results:
 *	A standard Tcl result
 *
 *----------------------------------------------------------------------
 */
 
int 
Sim_InspectCmd(machPtr, interp, argc, argv)
    DLX *machPtr;
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */

{
    int doWindow = 0, doQueue = 0, doRBuffer = 0, doDBuffer = 0, doUnit = 0, doHelp = 0;
    char *opName = *argv;
    int argNb = argc;
    char str[40];

    
    if (argc == 1) {
    	doWindow = doQueue = doRBuffer = doDBuffer = doUnit = 1;
    } else while (++argv, --argc) {
	if (!strcmp(*argv,"queue")) {
	    doQueue = 1;
    	} else if (!strcmp(*argv,"window")) {
	    doWindow = 1;
	} else if (!strcmp(*argv,"reorder")) {
	    doRBuffer = 1;
	} else if (!strcmp(*argv,"data")) {
	    doDBuffer = 1;
	} else if (!strcmp(*argv,"unit")) {
	    doUnit = 1;
	} else if (!strcmp(*argv,"help")) {
	    doHelp = 1;
	    if (argNb == 2) {
    	       doWindow = doQueue = doRBuffer = doDBuffer = doUnit = 1;
	    }
	} else if (!strcmp(*argv,"all")) {
    	    doWindow = doQueue = doRBuffer = doDBuffer = doUnit = 1;
	} else {
	    sprintf(interp->result,"Illegal Option: \nshould be \"%.50s\" [queue] [window] [reorder] [data] [unit] [all] [help] \nFor more information type \"help print\"",opName);
	    return TCL_ERROR;
	 }
    }

    if (!doHelp) printf("\n---> Clock Cycle %d",machPtr->cycleCount);

    if (doQueue) {
	char *dispQueue;
	if (!doHelp) {
	    printf("\n\n ** Instruction Fetch Queue\n");
	    printf("Number of entries: %d\nMaximum number of entries: %d\n",
		machPtr->insQueue->entryCount,
		machPtr->insQueue->numEntries);
	    if (machPtr->insQueue->entryCount) {
	        displayQueue(&dispQueue,machPtr->insQueue,NON_X);
	    	printf(" ______________________________________________________________\n");
	    	printf("|  #      icount  address    opcode    rs1  rs2   rd     extra |\n");
	    	printf("|--------------------------------------------------------------|\n");
	    	printf("%s",dispQueue);
	    	printf("+--------------------------------------------------------------+\n");
                // Se copia "dispQueue" en la variable global "contenidoQueue"
                memcpy(contenidoQueue,dispQueue,strlen(dispQueue));
                contenidoQueue[strlen(dispQueue)] = '\0';

	    }else { // si no hay nada en la cola, se vacía la cadena
               contenidoQueue[0] ='\0';
            }

	} else {

    	    /*FILE *fp;
	    int c;
	    sprintf(str, "%squeue.hlp",HelpPath);
    	    if ( (fp = fopen(str,"r") ) == NULL) {
	     	printf("File %s is missing: no help available\n", str);
	   	return TCL_OK;
	     }

	     while ((c= getc(fp)) != EOF)
	     	putc(c,stdout);
             */

	}

    }

    if (doWindow) {
	if (!doHelp) {
	    printf("\n\n ** Integer Instruction Window\n");
	    printf("Number of entries: %d \nMaximum number of entries: %d\n",
		machPtr->iWindow->entryCount,
		machPtr->iWindow->numEntries);
	    if (machPtr->iWindow->entryCount) {
            	displayWindow(machPtr->iWindow, NON_X);

	 	printf(" _____________________________________________________________________________ \n");
	    	 printf("|  #  opcode  address     rb_entry  operand1 ok1 typ1  operand2 ok2 typ2 pred |\n");

		printf("|-----------------------------------------------------------------------------|\n");
	    	printf("%s",dispWindow);

	    		printf("+-----------------------------------------------------------------------------+\n");
                if (dispWindow != NULL) {
                  memcpy(contenidoWindowInt,dispWindow,strlen(dispWindow));
                  contenidoWindowInt[strlen(dispWindow)] = '\0';
                }
	    	free(dispWindow);
	    }else contenidoWindowInt[0] = '\0';

	    displayWindow(machPtr->fpWindow, NON_X);
	    printf("\n\n ** Floating Point Instruction Window\n");

	    printf("Number of entries: %d \nMaximum number of entries: %d\n",
		machPtr->fpWindow->entryCount,
		machPtr->fpWindow->numEntries);

	 if (machPtr->fpWindow->entryCount) {


	   printf(" _____________________________________________________________________________ \n");
	   printf("|  #  opcode  address     rb_entry  operand1 ok1 typ1  operand2 ok2 typ2 pred |\n");
	   printf("|-----------------------------------------------------------------------------|\n");
	   printf("%s",dispWindow);
           if (dispWindow != NULL) {
            memcpy(contenidoWindowFloat,dispWindow,strlen(dispWindow));
            contenidoWindowFloat[strlen(dispWindow)] = '\0';
           }
	   printf("+-----------------------------------------------------------------------------+\n");

	 }else contenidoWindowFloat[0] = '\0';

     	} else {

    	   /* FILE *fp;
	    int c;
	    sprintf(str, "%swindow.hlp",HelpPath);

    	    if ( (fp = fopen(str,"r") ) == NULL) {
	     	printf("File %s is missing: no help available\n", str);
	   	return TCL_OK;
	     }

	     while ((c= getc(fp)) != EOF)
	     	putc(c,stdout);*/
	}


    }
    if (doRBuffer) {
    	int compte;
        if (!doHelp) {
	    ReorderElement *reorder;
            displayReorderBuffer(machPtr->iReorderBuffer,NON_X,&intUnit[0]);
	    printf("\n\n ** Integer Reorder Buffer\n");

	    printf("Number of entries: %d \nMaximum number of entries: %d\n",
		machPtr->iReorderBuffer->entryCount,
		machPtr->iReorderBuffer->numEntries);

	    printf("Head: %d  Tail: %d\n",
		machPtr->iReorderBuffer->head,
		machPtr->iReorderBuffer->tail);

	    if (machPtr->iReorderBuffer->entryCount) {

	        printf(" __________________________________________________________________________ \n");
	        printf("|  #  opcode     icount  reg#    unit        result    ok      ready  flsh |\n");
	        printf("|--------------------------------------------------------------------------|\n");
	        printf("%s",dispRBuffer);
                if (dispRBuffer != NULL) {
                  memcpy(contenidoRBufferInt,dispRBuffer,strlen(dispRBuffer));
                  contenidoRBufferInt[strlen(dispRBuffer)] = '\0';
                }
	        printf("+--------------------------------------------------------------------------+\n");

      	    } else contenidoRBufferInt[0] = '\0';

	    printf("\n\n ** Floating Point Reorder Buffer \n");

	    printf("Number of entries: %d \nMaximum number of entries: %d\n",
		machPtr->fpReorderBuffer->entryCount,
		machPtr->fpReorderBuffer->numEntries);

    	    printf("Head: %d Tail: %d\n",
		machPtr->fpReorderBuffer->head,
		machPtr->fpReorderBuffer->tail);

	    if (machPtr->fpReorderBuffer->entryCount) {

	    	displayReorderBuffer(machPtr->fpReorderBuffer, NON_X,&fpUnit[0]);

	        printf(" __________________________________________________________________________ \n");
	    	printf("|  #  opcode     icount  reg#    unit        result    ok      ready  flsh |\n");
	    	printf("|--------------------------------------------------------------------------|\n");
	    	printf("%s",dispRBuffer);

                if (dispRBuffer != NULL) {
                  memcpy(contenidoRBufferFloat,dispRBuffer,strlen(dispRBuffer));
                  contenidoRBufferFloat[strlen(dispRBuffer)] = '\0';
                }
	    	printf("+--------------------------------------------------------------------------+\n");


           }else contenidoRBufferFloat[0] = '\0';

    } else {
     /*	FILE *fp;
	int c;
	    sprintf(str, "%sreorder.hlp",HelpPath);

    	if ( (fp = fopen(str,"r") ) == NULL) {
	   printf("File %s is missing: no help available\n",str);
	   return TCL_OK;
	   }

	 while ((c= getc(fp)) != EOF)
	     putc(c,stdout);
       */
     }


   }
   if (doDBuffer) {
        if (!doHelp) {
       	    displayDataBuffer(machPtr->loadBuffer,LOAD_BUFFER,NON_X);
       	    printf("\n\n ** Load Buffer\n");
	    printf("Number of entries: %d \nMaximum number of entries: %d\n",
		machPtr->loadBuffer->entryCount,
		machPtr->loadBuffer->numEntries);

	    if (machPtr->loadBuffer->entryCount) {

       	    	printf(" ____________________________________________\n");
  	    	printf("|   #  opcode    icount   address  rel  flsh |\n");


  	    	printf("|--------------------------------------------|\n");
       	    	printf("%s",dispData);
                if (dispData != NULL) {
                  memcpy(contenidoDataLoad,dispData,strlen(dispData));
                  contenidoDataLoad[strlen(dispData)] = '\0';
                }
	    	printf("+--------------------------------------------+\n");


	    }else contenidoDataLoad[0] = '\0';
       	    displayDataBuffer(machPtr->storeBuffer,STORE_BUFFER,NON_X);
       	    printf("\n\n ** Store Buffer\n");
	    printf("Number of entries: %d \nMaximum number of entries: %d\n",
		machPtr->storeBuffer->entryCount,
		machPtr->storeBuffer->numEntries);

	    if (machPtr->storeBuffer->entryCount) {

       	    	printf(" ___________________________________________________________________\n");
       	    	printf("|  #   opcode    icount   address  data to store  type ok rel  flsh |\n");
  	    	printf("|-------------------------------------------------------------------|\n");
       	    	printf("%s",dispData);
                if (dispData != NULL) {
                  memcpy(contenidoDataStore,dispData,strlen(dispData));
                  contenidoDataStore[strlen(dispData)] = '\0';
                }
		free(dispData);
                printf("+-------------------------------------------------------------------+\n");

	    }else contenidoDataStore[0] = '\0';
	} else  {
    	   /* FILE *fp;
	    int c;
	    sprintf(str, "%sdata.hlp",HelpPath);


    	    if ( (fp = fopen(str,"r") ) == NULL) {
	     	printf("File str is missing: no help available\n",str);
	   	return TCL_OK;
	     }

	     while ((c= getc(fp)) != EOF)
	     	putc(c,stdout);
             */
	}


   }

    if (doUnit) {

        if (!doHelp) {
	  char *dispUnits;

	    printf("\n ** Integer Functional Units \n\n");
	    printf(" _____________________________________________\n");
	    printf("|   Unit         Number    Latency     Used   |\n");
	    printf("|---------------------------------------------|\n");
	    displayFuncUnits(&dispUnits,&machPtr->intUnits[1],5,&intUnit[0],NON_X);
	  printf("%s",dispUnits);
	  free(dispUnits);
	    printf("+---------------------------------------------+\n");

	    printf("\n ** Floating Point Functional Units \n");
	    printf(" _____________________________________________\n");
	    printf("|   Unit         Number    Latency     Used   |\n");
	    printf("|---------------------------------------------|\n");

	    displayFuncUnits(&dispUnits,&machPtr->fpUnits[1],7,&fpUnit[0],NON_X);
	  printf("%s",dispUnits);
	  free(dispUnits);
	    printf("+---------------------------------------------+\n");


	} else  {
    	  /*  FILE *fp;
	    int c;
	    sprintf(str, "%sunit.hlp",HelpPath);

    	    if ( (fp = fopen(str,"r") ) == NULL) {
	     	printf("File helpfiles/unit.hlp is missing: no help available\n");
	   	return TCL_OK;
	     }

	     while ((c= getc(fp)) != EOF)
	     	putc(c,stdout);
             */
	}

    }

   if (!doHelp)
       printf("\nFor information on the notation used: \"print help\" or \"print <buffername> help\"\n");

   return TCL_OK;


}

/*
 *----------------------------------------------------------------------
 *
 * Sim_GoCmd --
 *
 *	This procedure is invoked to process the "go" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Comments:
 *	Reused from dlxsim . Except that the machine counts is reset, to 
 *	permit several consecutive execution of the same loaded .s program 
 *
 *----------------------------------------------------------------------
 */

int
Sim_GoCmd(machPtr, interp, argc, argv)
    DLX *machPtr;			/* Machine description. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    if (argc > 2) {
	sprintf(interp->result,
		"too many args:  should be \"%.50s\" [address]", argv[0]);
	return TCL_ERROR;
    }

    if (argc == 2) {
	char *end;
	int newPc;

	/* ERA dlxsim, 11-3-91:  Added 2nd curr_LocalTbl parm. */
	if (Sym_EvalExpr(machPtr, curr_LocalTbl, (char *) NULL, argv[1], 0, &newPc, &end)
		!= TCL_OK) {
	    return TCL_ERROR;

	  }
	if ((*end != 0) || (newPc & 0x3)) {
	    sprintf(interp->result,
		    "\"%.50s\" isn't a valid starting address", argv[1]);
	    return TCL_ERROR;
	}
	machPtr->regs[PC_REG] = ADDR_TO_INDEX(newPc);
	machPtr->badPC = 0;
    }

   /* if the command is invoked after a simulation has completed, the 
 	machine counts are reset */
/*
   if (machPtr->state == END_OF_SIMULATION && argc == 2) {
  	machPtr->state = CYCLE_PROCESS;
	ResetCounts(machPtr);
   }
*/
    return Simulate(machPtr, interp, 0, CYCLE);
}

/*
 *----------------------------------------------------------------------
 *
 * PrintHelpFiles--
 *
 *	Displays the content of a help file describing a command
 * 	on user's request. The file directory is given by HelpPath
 *	defined in superdlx.h
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

static
void  PrintHelpFile(commandName,fileName)
    char *commandName;
    char *fileName;
{
/*    FILE *fp;
    char helpFile[50];
    sprintf(helpFile,"%s%s",HelpPath,fileName);
    if ((fp = fopen(helpFile,"r")) == NULL) {
    	printf("File %s does not exist: no more help is available on \"%s\" command\n",helpFile, commandName);
	printf("Change the value of the	SUPERDLXHELP environment variable to the correct value\n");
    } else {
        int c;
         while ( (c= getc(fp)) != EOF)
		putc(c,stdout);
    }
*/
}

/*
 *----------------------------------------------------------------------
 *
 * Sim_HelpCmd --
 *
 *	This procedure is invoked by the "help" Tcl command.
 *	See help.hlp for more details on what is does.
 *
 * Results:
 *	A standard Tcl result.
 *
 *----------------------------------------------------------------------
 */


int 
Sim_HelpCmd(machPtr, interp, argc, argv) 
DLX *machPtr;
Tcl_Interp *interp;
int argc;
char **argv;

{

    int doGo = 0, doLoad = 0, doNext = 0, doQuit = 0, doPrint = 0, doStats = 0,
        doStep = 0, doReset = 0;
    char *opName = *argv;

/*
    if (argc > 1){
	sprintf(interp->result,
		"too many args:  should be \"%.50s\" [address]", argv[0]);
	return TCL_ERROR;
    }
*/

    if (argc == 1) {

    	printf("\nList of the available commands:\n");
    
    	printf("		- go [address] \n");
    	printf("		- load file1 file2...\n");
    	printf("		- next [number][flag]\n");
    	printf("		- quit\n");
    	printf("		- print queue window reorder data units all help\n");
    	printf(" 		- stats [-p#] [-g] [-r] [-i] [-b] [-f <filename>] [-a]\n");
    	printf("		- step [address]\n");
    	printf("		- reset\n");

	printf("\nFor more information on a command, type \"help <command name>\"\n\n");

	printf("If you find any bugs, please report them to:\n\n");
	printf("        superdlx@acaps.cs.mcgill.ca\n\n");
	
    } else {

	while (++argv,--argc) {

	    if (!strcmp(*argv,"go")) {
		doGo = 1;
	    } else if (!strcmp(*argv,"load")) {
		doLoad = 1;
	    } else if (!strcmp(*argv,"next")) {
		doNext = 1;
	    } else if (!strcmp(*argv,"quit")) {
		doQuit = 1;
	    } else if (!strcmp(*argv,"print")) {
		doPrint = 1;
	    } else if (!strcmp(*argv,"stats")) {
		doStats = 1;
	    } else if (!strcmp(*argv,"step")) {
		doStep = 1;
	    } else if (!strcmp(*argv,"reset")) {
	 	doReset = 1;
	    } else {
	        sprintf(interp->result,"illegal option: should be \"%.50s\"[load] [next] [quit] [print] [reset] [stats] [step]",opName);
	   	 return TCL_ERROR;
	    }
	}
    }

    if (doGo) {
       //	PrintHelpFile("go","go.hlp");
    }
    if (doLoad) {
       //	PrintHelpFile("load","load.hlp");
    }
    if (doNext) {
       //	PrintHelpFile("next","next.hlp");
    }
    if (doQuit) {
      //	printf("\nCommand:    quit\n\n");
      //	printf("Argument:   none\n\n");
      //	printf("Description:\n\nExits the simulator\n\n");
    }
    if (doPrint) {
      //	PrintHelpFile("print","print.hlp");
    }
    if (doStats) {
      //	PrintHelpFile("stats","stats.hlp");
   }
    if (doStep) {
      //	PrintHelpFile("step","step.hlp");
    }

     if (doReset) {
      //	printf("\nCommand:    reset\n\n");
      //	printf("Argument:   none\n\n");
      //	printf("Description:\nReset the machine: in particular, the memory is cleaned so a new file can be \nloaded  and executed.\n\n");
   }




    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * ResetCounts--
 * 	Resets the machine counts, to allow several consecutive runs of
 *	the same loaded .s file
 *
 *
 * Results:
 *	None.
 *
 *----------------------------------------------------------------------
 */



static void ResetCounts(machPtr)
DLX *machPtr;
{
    machPtr->cycleCount = 0;
    machPtr->insCount = 0;
    free(machPtr->counts);
    machPtr->counts = (Statistics *) calloc(1,sizeof(Statistics));

}

/*
 *----------------------------------------------------------------------
 *
 * Reset --
 *
 *	Resets the superscalar machine, though keeping the parameters
 *	specified by the machine configuration file. This permits 
 * 	to run different simulations without quiting the simulator. 
 *	After resetting the machine , the .s files have to be reloaded.
 *
 * Results
 *	None
 *----------------------------------------------------------------------
 */

void Reset(machPtr,memory)
DLX *machPtr;
int memory;

{
    WindowElement *element;
    DataOp *dataOp;
    int i;

    if (memory) {
	free(machPtr->memPtr);
	free(machPtr->memScratch);
    }
    free(machPtr->branchBuffer);

    free(machPtr->insQueue->table);

    for (i=0, element=machPtr->iWindow->table;
		i<machPtr->iWindow->numEntries;
		i++,element++) {
	    free(element->firstOperand);
    	    free(element->secondOperand);

    }
   free(machPtr->iWindow->table);
   for (i=0, element=machPtr->fpWindow->table;
		i<machPtr->fpWindow->numEntries;
		i++,element++) {
	    free(element->firstOperand);
    	    free(element->secondOperand);

    }

   free(machPtr->fpWindow->table);

    
    free(machPtr->iReorderBuffer->element);
    free(machPtr->fpReorderBuffer->element);


   for (i=0,dataOp = machPtr->storeBuffer->table;
		i<machPtr->storeBuffer->numEntries;
		i++, dataOp++) {
       free(dataOp->dataToStore);
    }
    free(machPtr->storeBuffer->table);
    free(machPtr->loadBuffer->table);
    free(machPtr->counts);
    Sim_Create(machPtr,1);
}

/*
 *----------------------------------------------------------------------
 *
 * Sim_ResetCmd --
 *
 *	This procedure is invoked to process the "reset" Tcl command.
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int Sim_ResetCmd(machPtr, interp, argc, argv)
    DLX *machPtr;
    Tcl_Interp *interp;
    int argc;
    char **argv;

{
    int doMachine = 0, doStats = 0;
    char *opName = *argv;

     if (argc > 1) {
	sprintf(interp->result,
	"too many arguments");
	return TCL_ERROR;
    }

 	Reset(machPtr,1);


    return TCL_OK;

}
/*
 *----------------------------------------------------------------------
 *
 * Sim_StepCmd --
 *
 *	This procedure is invoked to process the "step" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

int
Sim_StepCmd(machPtr, interp, argc, argv)
    DLX *machPtr;			/* Machine description. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    if (argc > 2) {
	sprintf(interp->result,
		"too many args:  should be \"%.50s\" [address]", argv[0]);
	return TCL_ERROR;
    }

    if (argc == 2) {
	char *end;
	int newPc;

	/* ERA, 11-3-91:  Added 2nd curr_LocalTbl parm. */
	if (Sym_EvalExpr(machPtr, curr_LocalTbl, (char *) NULL, argv[1], 0, &newPc, &end)
		!= TCL_OK) {
	    return TCL_ERROR;
	}
	if ((*end != 0) || (newPc & 0x3)) {
	    sprintf(interp->result,
		    "\"%.50s\" isn't a valid address", argv[1]);
	    return TCL_ERROR;
	}

	machPtr->regs[PC_REG] = ADDR_TO_INDEX(newPc);
	machPtr->badPC = 0;
    }

    return Simulate(machPtr, interp, 1, CYCLE);
}


/*
 *----------------------------------------------------------------------
 *
 * Sim_DumpStats --
 *
 *	This procedure is invoked to process the "stats" Tcl command.
 *	See stats.hlp for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Comments:
 *	Same procedure name as in dlxsim, but has been rewritten entirely.
 *	(the stats are different)
 *
 *----------------------------------------------------------------------
 */

int
Sim_DumpStats(machPtr, interp, argc, argv)
    DLX *machPtr;			/* Machine description. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *p, *command = *argv;
    int argNb = argc;
    float precision = 0.0, bound;
    int precGiven = 0, fileGiven = 0;
    int total;
    int i, j;
    int doGen = 0, doRen = 0, doIns = 0,  doBuf = 0;
    FILE *fp;
    char *fileName;

    Statistics *count = machPtr->counts;

   fp = stdout;
   if (argNb == 1) {
 	doGen = doRen = doIns = doBuf = 1;

   } else {
	 while (argv++, --argc) {
	    if (*(p = *argv) != '-') {
usageError:
	    	sprintf(interp->result,
		    "usage: %s [-p#] [-g] [-r] [-i] [-b] [-f <filename>] [-a]\n", command);
	    	return TCL_ERROR;
		}
	    switch (*(p+1)) {
	    case 'p':

	    	precision = (float) (atof(p+2));
		precGiven = 1;
	    	if (argNb == 2) {
			doGen = doRen = doIns = doBuf = 1;
	    	}

	   	break;
	    case 'g':
	    	doGen = 1;
	    	break;
	    case 'r':
	    	doRen = 1;
	    	break;
	    case 'i':
	    	doIns = 1;
  	        break;
	    case 'b':
	        doBuf = 1;
	        break;
	    case 'a':
	        doGen = doRen = doIns = doBuf = 1;
   	        break; 
	    case 'f':
	        --argc;
		if (argc >= 1) {
	            fileName = *(++argv);
	 	    fileGiven = 1;
		} else {
		    goto usageError;
		}
	        if (argNb == 3) {
   		    doGen = doRen = doIns = doBuf = 1;
	        }
	        break;
	    default:
		goto usageError;
	    }
        }
    }

   if (fileGiven) {
   	if ((fp = fopen(fileName,"w+")) == NULL) {
   	    sprintf(interp->result,
	     	"usage: %s [-p#] [-g] [-r] [-i] [-b] [-f <filename>] [-a]\n", 
		command);
	    return TCL_ERROR;
        }
    }

   if ((precGiven) && (fileGiven) && (argNb == 4)){
	doGen = doRen = doIns = doBuf = 1;
    }
	
   if (!machPtr->cycleCount) {
	printf("The program has not been run\n");
        return TCL_OK;
   }
   if (doGen) {
        fprintf(fp,"Number of cycles: %u\n\n", machPtr->cycleCount);
        fprintf(fp,"Instructions fetched	%12u \n", machPtr->insCount);

	fprintf(fp,"Instructions decoded	%12u 	(%5.2f%% of total fetched)\n",
		 count->decoded, 
		100.0 * count->decoded / machPtr->insCount);
    	
	count->issued[ALL] = 
			count->issued[INTEGER] +
			count->issued[FLOAT];

	fprintf(fp,"\nInstructions issued	%12u 	(%5.2f%% of total fetched)\n",
		count->issued[ALL],
		100.0 * count->issued[ALL]/ machPtr->insCount);

	if (count->issued[ALL]) {
	    fprintf(fp,"-> integers		%12u	(%5.2f%% of total issued)\n",
	  	count->issued[INTEGER],
		100.0 * count->issued[INTEGER] /
		count->issued[ALL]);
	    fprintf(fp,"-> floating points 	%12d 	(%5.2f%% of total issued)\n",
		count->issued[FLOAT],
		100.0 * count->issued[FLOAT] /
		count->issued[ALL]);
	}
	count->commited[ALL] = 
			count->commited[INTEGER] +
			count->commited[FLOAT];

	fprintf(fp,"\nInstructions committed	%12u 	(%5.2f%% of total fetched)\n",
		count->commited[ALL],
		100.0 * count->commited[ALL] / machPtr->insCount);

	if (count->commited[ALL]) {
	    fprintf(fp,"-> integers		%12u	(%5.2f%% of total committed)\n",
		count->commited[INTEGER],
		100.0 * count->commited[INTEGER] / 
		count->commited[ALL]);

	    fprintf(fp,"-> floating points 	%12u	(%5.2f%% of total committed)\n",
		count->commited[FLOAT],
		100.0 * count->commited[FLOAT] / 
		count->commited[ALL]);


	    count->writes[ALL] = 
		count->writes[INTEGER] +
		count->writes[FLOAT];
	    fprintf(fp,"-> writes to registers	%12u	(%5.2f%% of total committed)\n",
			count->writes[ALL],
			100.0 *count->writes[ALL] /
			count->commited[ALL]);

            
	    count->uselessWrites[ALL] = 
			count->uselessWrites[INTEGER] +
			count->uselessWrites[FLOAT];
	    if (count->uselessWrites[ALL]) {
	    	fprintf(fp,"-> useless writes	%12u	(%5.2f%% of total writes)\n",
				count->uselessWrites[ALL],
				100.0 *count->uselessWrites[ALL] /
				count->writes[ALL]);
	    }
		
	   if(count->loadBlck){
	   		fprintf(fp,"\nLoads Blocked by Stores: %12d (%5.2f%% of total loads)\n",
							count->loadBlck,
							100.0 * count->loadBlck / count->numLoads);
		}
			 

   	 }
	fprintf(fp,"\nPer Cycle Rates\n");
	fprintf(fp,"-> fetch 	%-.2f instructions/cycle\n",
    		(float)machPtr->insCount/machPtr->cycleCount );
	fprintf(fp,"-> decode	%-.2f instructions/cycle\n", 
    		(float)count->decoded/machPtr->cycleCount);
	fprintf(fp,"-> issue	%-.2f instructions/cycle\n",
    		(float)count->issued[ALL]/machPtr->cycleCount);
	fprintf(fp,"-> commit	%-.2f instructions/cycle\n",
    	 	(float)count->commited[ALL]/machPtr->cycleCount);

 
    
	total = count->branchYes + count->branchNo;
    
	if (!total) 
		fprintf (fp,"\nNo branch instructions executed.\n");
      
	else {
	    int totalCorrect;
		totalCorrect = count->correctBranchYes + count->correctBranchNo;
		
	    fprintf (fp,"\nNumber of branches: %d, taken %d (%.2lf%%), untaken %d (%.2lf%%)\n",
			total, count->branchYes, 
			100.0 * count->branchYes / total, 
			count->branchNo, 
			100.0 * count->branchNo / total);
  	
	    if (machPtr->branchPred) {
	
	    	fprintf(fp,"Correct predictions	%12d (%5.2f%% of total branches)\n",
    		totalCorrect, 
		100.0 * totalCorrect / total);
	    
		if (count->branchYes) {
		    fprintf(fp,"-> taken		%12d (%5.2f%% of total taken branches)\n",
			count->correctBranchYes,
			100.0 * count->correctBranchYes / count->branchYes);
	        }

	    	if (count->branchNo) {
	       	    fprintf(fp,"-> not taken		%12d (%5.2f%% of not taken branches)\n", 
			count->correctBranchNo,
			100.0 * count->correctBranchNo
			 / count->branchNo);
	    	}
	       	fprintf(fp,"Wrong predictions\n");
	    	fprintf(fp,"-> lost cycles 		%12d (%5.2f%% of total cycles)\n",
	    	   	count->lostCycles,
		   	100.0 * count->lostCycles 
			/ machPtr->cycleCount);
	    	fprintf(fp,"-> flushed instructions	%12d (%5.2f%% of total fetched)\n",
    		 count->flushed,
		 100.0 * count->flushed / count->fetched);
		
		fprintf(fp, "Collisions in the BTB	%12d (%5.2f%% of total branches)\n",
		 	count->branchCollisions,
		  	100.0 * count->branchCollisions);
		 
	   
   	     }
	
         }
    
    	if (count->fetchStalls)  {
            fprintf(fp,"\nFetch Stalls: %d (%.2f%% of total cycle count)\n",
	 	count->fetchStalls,
		100.0 * count->fetchStalls / machPtr->cycleCount); 

	
 		
	     fprintf(fp,"Fetch stalls due to full buffers: %d (%.2f%% of total stalls)\n",
			count->fullBufferStalls,
			100.0 * count->fullBufferStalls
			/ count->fetchStalls); 	
     	}	    	

    	if (count->decodeStalls) {
	    fprintf(fp,"\nDecode Stalls: %d (%.2f%% of total cycle count)\n",
	 	count->decodeStalls,
		100.0 * count->decodeStalls / machPtr->cycleCount); 
    	}
    }
	bound = precision * machPtr->cycleCount / 100.0;

    if (doRen) {
   /* register renaming */


    	count->renames[ALL] = count->renames[INTEGER] + 
				    count->renames[FLOAT];

	count->searchedOp[ALL] = 
				count->searchedOp[INTEGER] +
					count->searchedOp[FLOAT];

    	if (count->searchedOp[ALL]) {
	fprintf (fp,"\n **Operands Renaming (flow/anti-dependencies)\n");

   	    fprintf(fp,"\nRenamed operands:	%6.2f%%  of total operands\n", 
		100.0 * count->renames[ALL] /
			count->searchedOp[ALL]);

	    if (count->searchedOp[FLOAT]) {
	    	fprintf(fp,"-> integers		%6.2f%% of integer operands\n", 
			100.0 * count->renames[INTEGER] /
				count->searchedOp[INTEGER]);
	     	fprintf(fp,"-> floats 		%6.2f%% of floating point operands\n",
			100.0 * count->renames[FLOAT] /
				count->searchedOp[FLOAT]);
			
	    }
    	    displayDistrib(fp,machPtr,
		 	&count->numRename[0][0], 
			bound,
		 	NUM_RENAME, 
			machPtr->cycleCount,
			count->issued[FLOAT]); 
	
	    fprintf(fp,"(\"NUM\": number of operands renamed ;\"total\": total number of clock cycles;\n \"%%\": percentage of clock cycles)\n");
	   

	    fprintf(fp,"\n**Operands Searching Information\n");    


	    displayDistrib(fp,machPtr,
			&count->numSearched[0][0],
			bound, 
			NUM_RENAME,
			machPtr->cycleCount,
			count->searchedOp[FLOAT]);

	    fprintf(fp,"(\"NUM\": number of operands searched;\"total\": total number of clock cycles;\n \"%%\"	: percentage of clock cycles)\n");	
                                                                           
        }
    }

    if ((doIns) && (count->issued[ALL])) {
        
	fprintf(fp,"\n**Instruction Issue Distribution\n");

    	displayDistrib(fp,machPtr,
			&count->numIssued[0][0],
			bound, 
			NUM_OPS,
			machPtr->cycleCount,
			count->issued[FLOAT]);
	fprintf(fp,"(\"NUM\": number of instructions issued:\"total\": total number of clock cycles;\n \"%%\"	: percentage of clock cycles\n");
	bound = precision * machPtr->insCount / 100.0;

	fprintf(fp,"\n**Issue Delay Distribution\n");
	displayDistrib(fp,machPtr,
			&count->numWait[0][0],
			bound, 
			NUM_OPS,
			count->issued[ALL],
			count->issued[FLOAT]);
	fprintf(fp,"(\"NUM\": number of clock cycles;\"total\": total number of instructions;\n \"%%\": percentage of issued instructions)\n");

    }

    if ((doIns) && (count->commited[ALL])) {
	bound = precision * machPtr->cycleCount / 100.0;
	fprintf(fp,"\n**Instruction Commit Distribution\n");
	displayDistrib(fp,machPtr,
			&count->numCommit[0][0],
			bound, 
			NUM_OPS,
			machPtr->cycleCount,
			count->commited[FLOAT]);
	fprintf(fp,"(\"NUM\": number of clock cycles;\"total\": total number of instructions;\n \"%%\": percentage of clock cycles)\n");
    }

    if ((doBuf) && (count->commited[INTEGER])) {
	bound = precision * machPtr->cycleCount / 100.0;
    	fprintf(fp,"\n**Occupancy of the Integer Buffers\n");
    	displayBufferOcc(fp,machPtr,
			&count->intBufferOcc[0][0],bound,11);
     }

    if ((doBuf) && (count->commited[FLOAT])) {
	bound = precision * machPtr->cycleCount / 100.0;
    	 fprintf(fp,"\n**Occupancy of the Floating Point Buffers\n");
    	displayBufferOcc(fp,machPtr,
			&count->fpBufferOcc[0][0],bound,11);
    }
	    
    fflush(fp);

    if (fileGiven) fclose(fp); //close(fp)

     return TCL_OK;
 }



/*
 *----------------------------------------------------------------------
 *
 * ReadMem --
 *
 *	Read a word from DLX memory.
 *
 * Results:
 *	Under normal circumstances, the result is 1 and the word at
 *	*valuePtr is modified to contain the DLX word at the given
 *	address.  If no such memory address exists, <or if a stop is
 *	set on the memory location (desactivated for superdlx)>, 
 *	then 0 is returned to signify that simulation should stop.
 *
 * Side effects:
 *	None.
 *
 * Comments (superdlx):
 * 	Reused from dlxsim, Removing the stop points process of stop.c
 * 	and I/O stuff of io.c of dlxsim .
 *	The instruction address is passed as argument, as it is different
 *	from the current pc
 *----------------------------------------------------------------------
 */

int
ReadMem(machPtr, address, valuePtr, pcAddr)
    register DLX *machPtr;	/* Machine whose memory is being read. */
    unsigned int address;	/* Desired word address. */
    int *valuePtr;		/* Store contents of given word here. */
    unsigned int pcAddr;	/* address of instruction */
{
    unsigned int index;
    register MemWord *wordPtr;

    index = ADDR_TO_INDEX(address);
    if (index < machPtr->numWords) {
	wordPtr = &machPtr->memPtr[index];
	/*if (machPtr->refTraceFile)
	  fprintf( machPtr->refTraceFile, "0 %x\n", address );*/
	*valuePtr = wordPtr->value;
	return 1;
    }

    /*
     * The word isn't in the main memory.  See if it is an I/O
     * register.
     */
/*
    if (Io_Read(machPtr, (address & ~0x3), valuePtr) == 1) {
	return 1;
    }
*/
    /*
     * The word doesn't exist.  Register a bus error.  If interrupts
     * ever get implemented for bus errors, this code will have to
     * change a bit.
     */

    (void) BusError(machPtr, address, 0, INDEX_TO_ADDR(pcAddr));
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * WriteMem --
 *
 *	Write a value into the DLX's memory.
 *
 * Results:
 *	If the write completed successfully then 1 is returned.  If
 *	any sort of problem occurred (such as an addressing error or
 *	a stop) then 0 is returned;  the caller should stop simulating.
 *
 * Side effects:
 *	The DLX memory gets updated with a new byte, halfword, or word
 *	value.
 *
 * Comments (superdlx)
 *	Reused from dlxsim, commenting out the break point processing
 * 	of stop.c and IO mapping of io.c of dlxsim 
 *----------------------------------------------------------------------
 */
int
WriteMem(machPtr, address, size, value, pcAddr)
    register DLX *machPtr;	/* Machine whose memory is being read. */
    unsigned int address;	/* Desired word address. */
    int size;			/* Size to be written (1, 2, or 4 bytes). */
    int value;			/* New value to write into memory. */
    unsigned int pcAddr; 	/* address of the instruction*/
{
    unsigned int index;
    register MemWord *wordPtr;

    if (((size == 4) && (address & 0x3)) || ((size == 2) && (address & 0x1))) {
	(void) AddressError(machPtr, address, 0,pcAddr);
	return 0;
    }
    index = ADDR_TO_INDEX(address);
    if (index < machPtr->numWords) {
	wordPtr = &machPtr->memPtr[index];

	if (size == 4) {
	    wordPtr->value = value;
	} else if (size == 2) {
	    if (address & 0x2) {
		wordPtr->value = (wordPtr->value & 0xffff0000)
			| (value & 0xffff);
	    } else {
		wordPtr->value = (wordPtr->value & 0xffff)
			| (value << 16);
	    }
	} else {
	    switch (address & 0x3) {
		case 0:
		    wordPtr->value = (wordPtr->value & 0x00ffffff)
			    | (value << 24);
		    break;
		case 1:
		    wordPtr->value = (wordPtr->value & 0xff00ffff)
			    | ((value & 0xff) << 16);
			    break;
		case 2:
		    wordPtr->value = (wordPtr->value & 0xffff00ff)
			    | ((value & 0xff) << 8);
		    break;
		case 3:
		    wordPtr->value = (wordPtr->value & 0xffffff00)
			    | (value & 0xff);
		    break;
	    }
	}
	wordPtr->opCode = OP_NOT_COMPILED;
	return 1;
    }

    /*
     * Not in main memory.  See if it's an I/O device register.
     */
/* used in dlxsim but not in superdlx 
    if (Io_Write(machPtr, address, value, size) == 1) {
	return 1;
    }
*/
    (void) BusError(machPtr, address, 0, INDEX_TO_ADDR(pcAddr));
    return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * Compile --
 *
 *	Given a memory word, decode it into a set of fields that
 *	permit faster interpretation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The contents of *wordPtr are modified.
 *
 * Comments (superdlx):
 * 	Reused from dlxsim
 *
 *----------------------------------------------------------------------
 */
 void
Compile(wordPtr)
    register MemWord *wordPtr;		/* Memory location to be compiled. */
{
    register OpInfo *opPtr;

    wordPtr->rs1 = (wordPtr->value >> 21) & 0x1f;
    wordPtr->rs2 = (wordPtr->value >> 16) & 0x1f;
    wordPtr->rd = (wordPtr->value >> 11) & 0x1f;
    opPtr = &opTable[(wordPtr->value >> 26) & 0x3f];
    wordPtr->opCode = opPtr->opCode;
    if (opPtr->format == IFMT) {
        wordPtr->rd = wordPtr->rs2;
	wordPtr->extra = wordPtr->value & 0xffff;
	if (wordPtr->extra & 0x8000) {
	    wordPtr->extra |= 0xffff0000;
	}
    } else if (opPtr->format == RFMT) {
	wordPtr->extra = (wordPtr->value >> 6) & 0x1f;
    } else {
	wordPtr->extra = wordPtr->value & 0x3ffffff;
	if (wordPtr->extra & 0x2000000) {
	    wordPtr->extra |= 0xfc000000;
	}
    }
    if (wordPtr->opCode == SPECIAL) {
      wordPtr->opCode = specialTable[wordPtr->value & 0x3f];
    }
    else if (wordPtr->opCode == FPARITH) {
      wordPtr->opCode = FParithTable[wordPtr->value & 0x3f];
    }
  }

/*
 *----------------------------------------------------------------------
 *
 * BusError --
 *
 *	Handle bus error exceptions.
 *      Reused from dlxsim for superscalar dlx
 * 
 * Results:
 *	A standard Tcl return value.  If TCL_OK, then there is no return
 *	string and it's safe to keep on simulating.
 *
 * Side effects:
 *	May  simulate a trap for the machine.
 *
 * Changes for superscalar dlx:
 *      We always assume that the result is TCL_ERROR (could it be different?)
 * 	The instruction pc is passed , as not the same as the current pc;
 *
 *----------------------------------------------------------------------
 */

int
BusError(machPtr, address, iFetch, pcAddr)
    DLX *machPtr;		/* Machine description. */
    unsigned int address;	/* Location that was referenced but doesn't
				 * exist. */
    int iFetch;			/* 1 means error occurred during instruction
				 * fetch.  0 means during a load or store. */
    unsigned int pcAddr; 	/* address of the instruction*/
{

    if (iFetch) {
	sprintf(machPtr->interp->result,
		"bus error: tried to fetch instruction at 0x%x",
		address);
    } else {
	sprintf(machPtr->interp->result,
		"bus error: referenced 0x%x during load or store, pc = %.100s: %s",
		address, Sym_GetString(machPtr, pcAddr),
		Asm_Disassemble(machPtr,
			machPtr->memPtr[machPtr->regs[PC_REG]].value,
			pcAddr & ~0x3));
    }
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * AddressError --
 *
 *	Handle address error exceptions.
 *
 * Results:
 *	A standard Tcl return value.  If TCL_OK, then there is no return
 *	string and it's safe to keep on simulating.
 *
 * Side effects:
 *	May  simulate a trap for the machine.
 *
 *                        
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
 int
AddressError(machPtr, address, load,pcAddr)
    DLX *machPtr;		/* Machine description. */
    unsigned int address;	/* Location that was referenced but doesn't
				 * exist. */
    int load;			/* 1 means error occurred during instruction
				 * fetch or load, 0 means during a store. */
    unsigned int pcAddr;
{
    sprintf(machPtr->interp->result,
	    "address error:  referenced 0x%x, pc = %.100s", address,
	    Sym_GetString(machPtr, INDEX_TO_ADDR(pcAddr)));
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Overflow --
 *
 *	Handle arithmetic overflow execptions.
 *
 * Results:
 *	A standard Tcl return value.  If TCL_OK, then there is no return
 *	string and it's safe to keep on simulating.
 *
 * Side effects:
 *	May  simulate a trap for the machine.
 *
 * Changes for superscalar dlx:
 *      We always assume that the result is TCL_ERROR (how could it be
 *      different?)
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Overflow(machPtr,operation)
    DLX *machPtr;		/* Machine description. */
    WindowElement *operation;
{
    unsigned int pcAddr;
    char tmp[20];
    pcAddr = INDEX_TO_ADDR(operation->address);
    sprintf(machPtr->interp->result, "arithmetic overflow, pc = %.100s: %s\n",
	    Sym_GetString(machPtr, pcAddr),  Asm_Disassemble(machPtr,
		    machPtr->memPtr[operation->address].value,
		    pcAddr & ~0x3));
    sprintf(tmp,"First Operand: %d\n",
			operation->firstOperand->value[0]);

     strcat(machPtr->interp->result,tmp);
     sprintf(tmp,"Second Operand: %d\n",
			operation->secondOperand->value[0]);
      strcat(machPtr->interp->result,tmp);
   	
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Mult --
 *
 *	 Simulate DLX multiplication.
 *
 * Results:
 *      The word at *resultPtr is overwritten with the result
 *      of the multiplication and 0 is returned if there was no
 *      overflow.  On overflow, a negative integer is returned
 *      and *resultPtr is not changed.
 *
 * Side effects:
 *	None.
 *
 * Comments (superdlx):
 * 	Reused from dlxsim 
 *----------------------------------------------------------------------
 */

int
Mult(a, b, signedArith, resultPtr)
    int a, b;			/* Two operands to multiply. */
    int signedArith;		/* Zero means perform unsigned arithmetic,
				 * one means perform signed arithmetic. */
     int *resultPtr;            /* Put result here. */
{
    unsigned int aHi, aLo, bHi, bLo, res1, res2, res3, res4, res5;
    unsigned int *resPtr;
    int negative;

    if ((a == 0) || (b == 0)) {
	*resultPtr = 0;
	return 0;
    }

    /*
     * Compute the sign of the result, then make everything positive
     * so unsigned computation can be done in the main computation.
     */

    negative = 0;
    if (signedArith) {
	if (a < 0) {
	    negative = !negative;
	    a = -a;
	}
	if (b < 0) {
	    negative = !negative;
	    b = -b;
	}
    }

    /*
     * Compute the result in unsigned arithmetic (check a's bits one at
     * a time, and add in a shifted value of b).
     */

    aLo = (unsigned int) (a & 0xffff);
    aHi = ((unsigned int) a) >> 16;
    bLo = (unsigned int) (b & 0xffff);
    bHi = ((unsigned int) b) >> 16;

    /* printf ("\naHi %d aLo %d bHi %d bLo %d\n", aHi, aLo, bHi, bLo); */

    if (aHi && bHi) return -1;

    res1 = aLo * bLo;
    res2 = aHi * bLo;
    res3 = aLo * bHi;
    if ((res2 | res3) & 0xffff0000) return -2;
    
    res2 <<= 16;
    res3 <<= 16;
    if ((res2 & res3) & 0x80000000) return -3;
    
    res4 = res2 + res3;
    if (((res2 | res3) & 0x80000000) && (!(res4 & 0x80000000))) return -4;

    if ((res1 & res4) & 0x80000000) return -5;
    res5 = res1 + res4;
    if (((res1 | res4) & 0x80000000) && (!(res5 & 0x80000000))) return -6;

    /*
     * If the result is supposed to be negative, compute the two's
     * complement of the double-word result.
     */

    if (negative) {
        if ((res5 & 0x80000000) && (res5 & 0x7fffffff)) return -7;
	else res5 = (~res5) + 1;
      }
    
    resPtr = (unsigned int *) resultPtr;
    *resPtr = res5;
    return 0;
}



//--------------------------------------------------

void liberaStruct(DLX* machPtr){
int i;
// ************************ Memory
//	machPtr->memPtr = (MemWord *)   calloc(1, (unsigned) (sizeof(MemWord) * machPtr->numWords));
	free (machPtr->memPtr);
//	      machPtr->memScratch = (char *) malloc (machPtr->numChars);
	free(machPtr->memScratch);

// *********************** Symbol table
//	Hash_InitTable(&machPtr->symbols, 0, HASH_STRING_KEYS);
	Hash_DeleteTable(&machPtr->symbols); //es posible que haya que hacer una función para liberar la tabla hash
	//free(machPtr->symbols);
// ************************ BTB
//	  machPtr->branchBuffer = (BranchInfo *) calloc(1, (unsigned) (sizeof(BranchInfo) * machPtr->bb_size));
	free(machPtr->branchBuffer);

// ************************ Instruction queue
//	    machPtr->insQueue->table = (InstructionElement *)calloc(1,(unsigned)(sizeof(InstructionElement) *(machPtr->insQueue->numEntries)));
	free(machPtr->insQueue->table);

//********************* integer window
//elementos de la ventan de instrucciones que deben ser liberados antes que la ventana

for (i=0;i<machPtr->iWindow->numEntries;i++) {
	    free(machPtr->iWindow->freeList->firstOperand);
		free(machPtr->iWindow->freeList->secondOperand);
}
//    machPtr->iWindow->table = (WindowElement *)calloc(1,(unsigned)(sizeof(WindowElement) * (machPtr->iWindow->numEntries)));
	free(machPtr->iWindow->table);

//*********************** fp window
// ventana de flotantes;se debe hacer igual que la de enteros

for (i=0;i<machPtr->fpWindow->numEntries;i++) {
	    free(machPtr->fpWindow->freeList->firstOperand);
		free(machPtr->fpWindow->freeList->secondOperand);
}

//   machPtr->fpWindow->table = (WindowElement *)	calloc(1,(unsigned)(sizeof(WindowElement) * (machPtr->fpWindow->numEntries)));

	free(machPtr->fpWindow->table);

//*********************** reorder buffer
// machPtr->iReorderBuffer->element = (ReorderElement *) calloc(1, (unsigned) (sizeof(ReorderElement) * (machPtr->iReorderBuffer->numEntries + 1)));
	free( machPtr->iReorderBuffer->element);

// machPtr->fpReorderBuffer->element =  (ReorderElement *) calloc(1, (unsigned) (sizeof(ReorderElement) * (machPtr->fpReorderBuffer->numEntries + 1)));
	free( machPtr->fpReorderBuffer->element);
//************************ Store buffer
// liberamos los elementos del buffer store

for (i=0;i<machPtr->storeBuffer->numEntries;i++) {
	    free(machPtr->storeBuffer->freeList->dataToStore);
}

//    machPtr->storeBuffer->table = (DataOp *) calloc (1, (unsigned)(sizeof(DataOp) * (machPtr->storeBuffer->numEntries)));
	free(machPtr->storeBuffer->table);

//********************* load buffer

//    machPtr->loadBuffer->freeList = (DataOp *)calloc (1, (unsigned)(sizeof(DataOp) * (machPtr->loadBuffer->numEntries)));
	free(machPtr->loadBuffer->freeList);
//********************* Statistics
//    machPtr->counts = (Statistics *) calloc(1,sizeof(Statistics));
	free(machPtr->counts);


}
