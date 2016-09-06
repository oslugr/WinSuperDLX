
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
 * supersim.c
 * 
 * 
 * This files contains the core of the superscalar simulator that simulates and
 * synchronizes  superscalar pipeline stages.
 * 
 * 
 */
static char rcsid[] = "$Id: supersim.c,v 1.19 93/04/23 12:02:01 cecile Exp $";
/*
 * $Log:	supersim.c,v $
 * Revision 1.19  93/04/23  12:02:01  cecile
 * luis fixed the decode counter
 * 
 * Revision 1.18  93/04/13  23:43:00  cecile
 * correct the commit rate
 * 
 * Revision 1.17  93/04/13  14:05:38  cecile
 * Supersimulator source file
 * 
 * Revision 1.16  93/04/11  21:06:46  cecile
 * number of commits per clock cycle fixed
 * 
 * Revision 1.15  93/04/01  18:14:56  cecile
 * After the end of simulation, the step command has no effect.
 * New write back function, using a list of pending ops.
 * Save version before trap handling changes
 * 
 * Revision 1.14  93/03/22  19:47:13  cecile
 * Implementation of the list of pending operations: to accelerate the 
 * write back process.
 * New process for branches: when branch prediction is performed, branches are
 * entered in both reorder buffers, so the synchronization of the buffers is
 * facilitated
 * New function Execute, for the execute stage. 
 * Revision 1.13  93/02/16  11:56:55  cecile
 * collection of statistics
 * 
 * Revision 1.12  93/02/03  12:19:35  cecile Implementation of error checking:
 * if STOP_SIMULATION, ERROR type flags are raised, then simulation is
 * stopped and the error message is written in the tcl interpreter result.
 * 
 * Revision 1.11  93/01/30  19:12:02  cecile revised branch predicton mechanism:
 * we had problems with fp programs such as tomcatv.s because instructions
 * depended on branches and were not in the same reorder buffer as the
 * branch: so in rare occasions (big reorder buffers, high fetch rate..)
 * instructions depending on branches would commit before the outcome of the
 * branch is known. Now there is a field in the reorder buffer structure
 * giving the instruction count of the next predicted branch that will
 * execute: this way, we prevent instructions from the other reorder buffer
 * from commiting when their instruction count is greater than this field's
 * value.
 * 
 * Revision 1.10  92/11/04  14:10:37  cecile version without branch prediction
 * 
 * Revision 1.9  92/09/28  14:40:12  cecile New structure for load and store
 * buffers: only one load buffer and one store buffer are used instead of two
 * instances of each ( that is , instead of one per operational unit). Now
 * then only one call to SendLoads and one call to SendStores are necessary
 * 
 * Revision 1.8  92/08/26  10:19:20  cecile New Data structure for the
 * instruction queue, with a free list and an occupied list. Calls to malloc
 * and free are not needed anymore
 * 
 * Revision 1.7  92/08/24  14:33:08  cecile Optimization phase: the instruction
 * window has its structure changed in dlx.h.
 * 
 * Revision 1.6  92/08/16  12:43:12  cecile Version of supersim that works for
 * programs such as fib, queen, quickrand... That is traps have been
 * implemented, recursive calls are handled correctly. But there is no branch
 * prediction (the branch target buffer is not used). When a branch is
 * encountered, fetch stops and branch is simply executed at execute stage
 * What has changed in this file: traps (except trap#0)  are executed when
 * they arrive at the head of the integer reorder buffer, if all the
 * instructions of the fp reorder buffer that preceed them in the dynamic
 * instruction count have commited, as well as all the corresponding stores
 * in the integer and floating point store buffers.
 * 
 * Revision 1.5  92/08/09  18:16:37  cecile Branches Implemented with a
 * prediction scheme (two bit entry table) Also the store and load can now be
 * flushed out in case of bad prediction, as well as the reorder buffer
 * entries
 * 
 * Revision 1.4  92/07/22  17:07:45  cecile Integration of the store buffer and
 * load buffers. Results are thus forwarded to the store buffer when
 * computed, and the load can bypass stores when competing for the data cache
 * on the same cycle.
 * 
 * Revision 1.3  92/06/11  14:51:19  cecile Fonctions concerning decode,and
 * fetch have been placed elsewhere.
 * 
 * Revision 1.2  92/06/10  11:53:09  cecile The file now contains the main
 * functions for simulating the superscalar architectures: trace files are
 * produced which show that the overall mechanisms work. But there is still a
 * need to check the machine state. Also, the store and branches are not
 * taken into account.
 * 
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <values.h>
#include <math.h>
#include "tcl/tcl.h" // cambiado
#include "dlx.h"
#include "superdlx.h"

/* for trace files */
char *type[] = {"NO", "INT", "FPS", "FPD", "IMM"};
char *intUnit[] = {"NO_UNIT", "INT_ALU", "INT_SHIFT", "INT_DATA", "INT_COMP",
"INT_BRANCH", "INT_MEM"};
char *fpUnit[] = {"NO_UNIT", "FP_ADD ", "FP_DIV ", "FP_MULT", "FP_CONV",
"FP_DATA", "FP_COMP", "FP_BRANCH", "FP_MEM "};
/*
 * the following structure capture the behaviour of an instruction and is
 * placed in a table accessed by the opcode: this is very similar to the
 * opcode information table of asm.c but is more appropriate for us here.
 */

OpBehave infOp[] = {
    {"", OTHER, NONE, NONE, NONE, NONE, NONE},	/* 0 */
    {"ADD", INTEGER, ALU, INT_ALU, INTEGER, INTEGER, INTEGER},
    {"ADDI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"ADDU", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"ADDUI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"AND", INTEGER, ALU, INT_ALU, INTEGER, INTEGER, INTEGER},	/* 5 */
    {"ANDI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"BEQZ", INTEGER, BRANCH, INT_BRANCH, INTEGER, IMMEDIATE, INTEGER},
    {"BFPF", FLOAT, FBRANCH, FP_BRANCH, FLOAT, IMMEDIATE, INTEGER},
    {"BFPT", FLOAT, FBRANCH, FP_BRANCH, FLOAT, IMMEDIATE, INTEGER},
    {"BNEZ", INTEGER, BRANCH, INT_BRANCH, INTEGER, IMMEDIATE, INTEGER},	/* 10 */
    {"J", INTEGER, DIRECT_JUMP, INT_BRANCH, NONE, NONE, INTEGER},
    {"JAL", INTEGER, DIRECT_JUMP, INT_BRANCH, NONE, NONE, INTEGER},
    {"JALR", INTEGER, INDIRECT_JUMP, INT_BRANCH, INTEGER, NONE, INTEGER},
    {"JR", INTEGER, INDIRECT_JUMP, INT_BRANCH, INTEGER, NONE, INTEGER},
    {"LB", INTEGER, LOAD, INT_DATA, INTEGER, IMMEDIATE, INTEGER},	/* 15 */
    {"LBU", INTEGER, LOAD, INT_DATA, INTEGER, IMMEDIATE, INTEGER},
    {"LD", FLOAT, FLOAD, FP_DATA, INTEGER, IMMEDIATE, FP_DOUBLE},
    {"LF", FLOAT, FLOAD, FP_DATA, INTEGER, IMMEDIATE, FP_SIMPLE},
    {"LH", INTEGER, LOAD, INT_DATA, INTEGER, IMMEDIATE, INTEGER},
    {"LHI", INTEGER, SHIFT, INT_SHIFT, INTEGER, IMMEDIATE, INTEGER},	/* 20 */
    {"LHU", INTEGER, LOAD, INT_DATA, INTEGER, IMMEDIATE, INTEGER},
    {"LW", INTEGER, LOAD, INT_DATA, INTEGER, IMMEDIATE, INTEGER},
    {"MOVD", FLOAT, FMOVE, FP_ADD, FP_DOUBLE, NONE, FP_DOUBLE},
    {"MOVF", FLOAT, FMOVE, FP_ADD, FP_SIMPLE, NONE, FP_SIMPLE},
    {"MOVFP2I", INTEGER, MOVE, INT_ALU, FP_SIMPLE, NONE, INTEGER},	/* 25 */
    {"MOVI2FP", FLOAT, FMOVE, FP_ADD, INTEGER, NONE, FP_SIMPLE},
    {"MOVI2S", INTEGER, MOVE, INT_ALU, INTEGER, NONE, INTEGER},
    {"MOVS2I", INTEGER, MOVE, INT_ALU, INTEGER, NONE, INTEGER},
    {"OR", INTEGER, ALU, INT_ALU, INTEGER, INTEGER, INTEGER},
    {"ORI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},	/* 30 */
    {"RFE", OTHER, NONE, NONE, NONE, NONE, NONE},
    {"SB", INTEGER, STORE, INT_DATA, INTEGER, IMMEDIATE, INTEGER},
    {"SD", FLOAT, FSTORE, FP_DATA, INTEGER, IMMEDIATE, FP_DOUBLE},
    {"SEQ", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SEQI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},	/* 35 */
    {"SEQU", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SEQUI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SF", FLOAT, FSTORE, FP_DATA, INTEGER, IMMEDIATE, FP_SIMPLE},
    {"SGE", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SGEI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},	/* 40 */
    {"SGEU", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SGEUI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SGT", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SGTI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SGTU", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},	/* 45 */
    {"SGTUI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SH", INTEGER, STORE, INT_DATA, INTEGER, IMMEDIATE, INTEGER},
    {"SLE", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SLEI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SLEU", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},	/* 50 */
    {"SLEUI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SLL", INTEGER, SHIFT, INT_SHIFT, INTEGER, INTEGER, INTEGER},
    {"SLLI", INTEGER, SHIFT, INT_SHIFT, INTEGER, IMMEDIATE, INTEGER},
    {"SLT", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SLTI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},	/* 55 */
    {"SLTU", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SLTUI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SNE", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},
    {"SNEI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SNEU", INTEGER, SET, INT_COMP, INTEGER, INTEGER, INTEGER},	/* 60 */
    {"SNEUI", INTEGER, SET, INT_COMP, INTEGER, IMMEDIATE, INTEGER},
    {"SRA", INTEGER, SHIFT, INT_SHIFT, INTEGER, INTEGER, INTEGER},
    {"SRAI", INTEGER, SHIFT, INT_SHIFT, INTEGER, IMMEDIATE, INTEGER},
    {"SRL", INTEGER, SHIFT, INT_SHIFT, INTEGER, INTEGER, INTEGER},
    {"SRLI", INTEGER, SHIFT, INT_SHIFT, INTEGER, IMMEDIATE, INTEGER},	/* 65 */
    {"SUB", INTEGER, ALU, INT_ALU, INTEGER, INTEGER, INTEGER},
    {"SUBI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"SUBU", INTEGER, ALU, INT_ALU, INTEGER, INTEGER, INTEGER},
    {"SUBUI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"SW", INTEGER, STORE, INT_DATA, INTEGER, IMMEDIATE, INTEGER},	/* 70 */
    {"TRAP", INTEGER, TRAP, NONE, NONE, IMMEDIATE, INTEGER},
    {"XOR", INTEGER, ALU, INT_ALU, INTEGER, INTEGER, INTEGER},
    {"XORI", INTEGER, ALU, INT_ALU, INTEGER, IMMEDIATE, INTEGER},
    {"NOP", OTHER, NONE, NONE, NONE, NONE, NONE},
    {"", OTHER, NONE, NONE, NONE, NONE, NONE},	/* 75 */
    {"", OTHER, NONE, NONE, NONE, NONE, NONE},
    {"", OTHER, NONE, NONE, NONE, NONE, NONE},
    {"", OTHER, NONE, NONE, NONE, NONE, NONE},
    {"", OTHER, NONE, NONE, NONE, NONE, NONE},
    {"ADDD", FLOAT, FADD, FP_ADD, FP_DOUBLE, FP_DOUBLE, FP_DOUBLE},	/* 80 */
    {"ADDF", FLOAT, FADD, FP_ADD, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"CVTD2F", FLOAT, FCONV, FP_CONV, FP_SIMPLE, NONE, FP_DOUBLE},
    {"CVTD2I", FLOAT, FCONV, FP_CONV, FP_DOUBLE, NONE, FP_SIMPLE},
    {"CVTF2D", FLOAT, FCONV, FP_CONV, FP_SIMPLE, NONE, FP_DOUBLE},
    {"CVTF2I", FLOAT, FCONV, FP_CONV, FP_SIMPLE, NONE, FP_SIMPLE},	/* 85 */
    {"CVTI2D", FLOAT, FCONV, FP_CONV, FP_SIMPLE, NONE, FP_DOUBLE},
    {"CVTI2F", FLOAT, FCONV, FP_CONV, FP_SIMPLE, NONE, FP_SIMPLE},
    {"DIV", FLOAT, FDIV, FP_DIV, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"DIVD", FLOAT, FDIV, FP_DIV, FP_DOUBLE, FP_DOUBLE, FP_DOUBLE},
    {"DIVF", FLOAT, FDIV, FP_DIV, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},	/* 90 */
    {"DIVU", FLOAT, FDIV, FP_DIV, FP_SIMPLE, IMMEDIATE, FP_SIMPLE},
    {"EQD", FLOAT, FSET, FP_COMP, FP_DOUBLE, FP_DOUBLE, FP_SIMPLE},
    {"EQF", FLOAT, FSET, FP_COMP, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"GED", FLOAT, FSET, FP_COMP, FP_DOUBLE, FP_DOUBLE, FP_SIMPLE},
    {"GEF", FLOAT, FSET, FP_COMP, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},	/* 95 */
    {"GTD", FLOAT, FSET, FP_COMP, FP_DOUBLE, FP_DOUBLE, FP_SIMPLE},
    {"GTF", FLOAT, FSET, FP_COMP, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"LED", FLOAT, FSET, FP_COMP, FP_DOUBLE, FP_DOUBLE, FP_SIMPLE},
    {"LEF", FLOAT, FSET, FP_COMP, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"LTD", FLOAT, FSET, FP_COMP, FP_DOUBLE, FP_DOUBLE, FP_SIMPLE},	/* 100 */
    {"LTF", FLOAT, FSET, FP_COMP, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"MULT", FLOAT, FMULT, FP_MUL, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"MULTD", FLOAT, FMULT, FP_MUL, FP_DOUBLE, FP_DOUBLE, FP_DOUBLE},
    {"MULTF", FLOAT, FMULT, FP_MUL, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"MULTU", FLOAT, FMULT, FP_MUL, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},	/* 105 */
    {"NED", FLOAT, FSET, FP_COMP, FP_DOUBLE, FP_DOUBLE, FP_SIMPLE},
    {"NEF", FLOAT, FSET, FP_COMP, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
    {"SUBD", FLOAT, FADD, FP_ADD, FP_DOUBLE, FP_DOUBLE, FP_DOUBLE},
    {"SUBF", FLOAT, FADD, FP_ADD, FP_SIMPLE, FP_SIMPLE, FP_SIMPLE},
{"", OTHER, NONE, NONE, NONE, NONE, NONE}};	/* 110 */





FILE *fp;



static void ForwardOperand();
static void ResultCommit();
static void WriteBack();
static void StatsCounts();
static int Execute();


/*
 * --------------------------------------------------------------------------
 * Simulate --
 * 
 * This procedure is the core of dlx superscalar simulator: it corresponds
 * to Simulate in dlxsim: but the function had to be entirely restructured to
 * implement the superscalar mechanisms. The different mechanisms involved in
 * a superscalar machine are represented by a fetch stage, a decode stage, an
 * execute stage, a write back stage and a commit stage. Normally those
 * processes should overlap in execution: to  simulatethe process
 * overlapping, the  Simulate function runs the stages on a cycle basis,
 * starting with the consummer processes running before the producer
 * processes. The goal of this is to avoid mixing up data from different
 * cycles in the common resources (instruction queue, central
 * windows,buffers).
 * 
 * Result: A standard Tcl Result
 * 
 * Side Effect: Simulates a cycle or several cycles: so all buffers queues and
 * windows are updated consequently
 * 
 * Called by: Sim_StepCmd, and Sim_GoCmd of sim.c
 * ---------------------------------------------------------------------------
 * */

int
Simulate(machPtr, interp, numberOfSteps, format)
    DLX *machPtr;	/* Machine description. */
    Tcl_Interp *interp;		/* Tcl interpreter, for results and break
				 * commands. */
    int numberOfSteps;
    int format;

{
    int i, result;
    char *errMsg, msg[20];	/* messages generated during simulation */
    unsigned int tmp;
    int go = 0;
    int numLoadAccess, dataAccess;	/* used to limit the number of data
					 * accesses per cycle */
    int nbFetch, nbDecode;	/* counts for statistics  */
    int nbRename[4], nbSearched[4];	/* idem */
    int intCount, fpCount;	/* idem */
    int occupancy;

    int stepCount = 0;		/* cycleCount for format == CYCLE */
    int startFetch =		/* starting point for format */
    machPtr->counts->fetched;	/* = INSTRUCTION */

    register InstructionElement *instruction;

    int holdTrap;		/* for synchronization of the 
    				 * commits if there is a trap */
    DataOp *store;

    while (machPtr->state == CYCLE_PROCESS) {

	/* we execute simulate cycle by cycle (single Step) */

	++machPtr->cycleCount;
	++stepCount;

	if (machPtr->stopFetch == NEXT_CYCLE_FETCH) {
	    machPtr->stopFetch = FETCH;
	}


	/*
	 * Result Commit Process
	 */

	holdTrap = 0;

	intCount = 0;		/* counts the number of integer commits */

	if (HEAD(iReorderBuffer).valid || HEAD(iReorderBuffer).flush) {
	    ResultCommit(machPtr,
			 machPtr->iReorderBuffer,
			 &holdTrap, &intCount, INTEGER);

	}
	fpCount = 0;		/* counts the number of floating point
				 * commits */
	if (HEAD(fpReorderBuffer).valid || HEAD(fpReorderBuffer).flush) {
	    ResultCommit(machPtr,
			 machPtr->fpReorderBuffer,
			 &holdTrap, &fpCount,FLOAT);

	}
	if (fpCount || intCount)
	    StatsCounts(&machPtr->counts->numCommit[0][0],
			&machPtr->counts->commited[0],
			NUM_OPS,
			intCount, fpCount);

	/*
	 * Write Back Process
	 */

	/* Integer Unit */

	if (/* machPtr->iReorderBuffer->entryCount > 0 &&
	  * machPtr->iReorderBuffer->checkOps && */
	    (machPtr->iReorderBuffer->ready == machPtr->cycleCount))
	    WriteBack(machPtr,
		      machPtr->iReorderBuffer,
		      &machPtr->intUnits[0],
		      INT_MEM);

	/* Idem Floating Point unit */
	if (/* machPtr->fpReorderBuffer->entryCount > 0
	     * && machPtr->fpReorderBuffer->checkOps  && */
	    (machPtr->fpReorderBuffer->ready == machPtr->cycleCount))
	    WriteBack(machPtr,
		      machPtr->fpReorderBuffer,
		      &machPtr->fpUnits[0],
		      FP_MEM);


	/*
	 * Execute Stage
	 */

	/*
	 * Operand Forwarding : once the result have been marked valid by the
	 * write back function, they can be forwarded to the instruction
	 * windows and store buffer. Operand forwarding to the instruction is
	 * performed in the function Execute to avoid several passes in  the
	 * instruction windows.
	 */


	/* forward results to the store buffer (for the data to store) */
	store = machPtr->storeBuffer->lastElement;
	while (store != NULL) {
	    if ((store->dataToStore->valid != 1) && (!store->flush))
		ForwardOperand(machPtr, store->dataToStore);
	    store = store->nextPtr;
	}


	/*
	 * Loads and stores are sent to the cache from their respective load
	 * and store buffers. Load bypassing is implemented: loads have a
	 * higher priority than stores: so we first look in the load buffer
	 * and try to send loads to the cache. The number of data accesses
	 * per clock cycle is limited by machPtr->num_mem_access. So if this
	 * maximum number has not been reached after processing the load
	 * buffer, we can then try to send stores to the cache
	 */

	numLoadAccess = 0;	/* number of loads executed in one cycle */
	dataAccess = ISSUED;
	if (COUNT(loadBuffer) != 0) {
	    if ((dataAccess =
		 SendLoads(machPtr,
			   &numLoadAccess)) == STOP_SIMULATION) {
		machPtr->state = STOP_SIMULATION;
		break;
	    }
	}
	if ((COUNT(storeBuffer) != 0) && (dataAccess == ISSUED)) {
	    if ((dataAccess =
		 SendStores(machPtr,
	     machPtr->num_mem_access - numLoadAccess)) == STOP_SIMULATION) {
		machPtr->state = STOP_SIMULATION;
		break;
	    }
	}
	/*
	 * Execute process in the integer operational unit
	 */

	intCount = 0;		/* counts the number of integer operations
				 * issues */

	if ((machPtr->state = Execute(machPtr,
				      machPtr->iWindow,
				      machPtr->iReorderBuffer,
				      &machPtr->intUnits[0],
				      &intCount, &errMsg,
				      &machPtr->counts->numWait[INTEGER][0],
				      ExecuteInteger)) == STOP_SIMULATION) {
	    break;
	}
	/*
	 * Execute process in the Floating Point operational unit
	 */


	fpCount = 0;		/* number of fp issues */

	if ((machPtr->state = Execute(machPtr,
				      machPtr->fpWindow,
				      machPtr->fpReorderBuffer,
				      &machPtr->fpUnits[0],
				      &fpCount,
				      &errMsg,
				      &machPtr->counts->numWait[FLOAT][0],
				      ExecuteFloat)) == STOP_SIMULATION) {
	    break;
	}
	StatsCounts(&machPtr->counts->numIssued[0][0],
		    &machPtr->counts->issued[0],
		    NUM_OPS,
		    intCount, fpCount);

	/*
	 * Decode Stage
	 */

	/* count of the number of renamed operands */
	nbRename[ALL] = nbRename[INTEGER] = nbRename[FP_SIMPLE] =
	    nbRename[FP_DOUBLE] = 0;
	/* counts of the number of needed operands */
	nbSearched[ALL] = nbSearched[INTEGER] = nbSearched[FP_SIMPLE] =
	    nbSearched[FP_DOUBLE] = 0;


	instruction = FIRST(insQueue);

	/*
	 * Decode Loop: the next instruction can be accessed from the
	 * instruction queue if the following conditions are satisfied:  ->
	 * the maximum number of instructions that can be decoded in one
	 * cycle has not been reached; ->there are still instructions in the
	 * instruction queue, -> the instruction windows and the reorder
	 * buffers are not full
	 */
	nbDecode = 0;
	while ((nbDecode < machPtr->numDecode) &&
	       (COUNT(insQueue) > 0) &&
	       (COUNT(iWindow) < MAX(iWindow)) &&
	       (COUNT(fpWindow) < MAX(fpWindow)) &&
	       (COUNT(iReorderBuffer) < MAX(iReorderBuffer)) &&
	       (COUNT(fpReorderBuffer) < MAX(fpReorderBuffer))) {

	    int decode;
	    int classe;

	    InstructionElement *instructionTmp;


	    classe = infOp[instruction->wordPtr->opCode].classe;

	    if (((classe == STORE || classe == FSTORE)
		 && (COUNT(storeBuffer) == MAX(storeBuffer))) ||
		((classe == LOAD || classe == FLOAD)
		 && (COUNT(loadBuffer) == MAX(loadBuffer)))) {
		break;
	    }
	    
	    decode = DecodeInstruction(machPtr, instruction,
				       &nbRename[0], &nbSearched[0]);

	    ++machPtr->counts->decoded;
	    ++nbDecode;

	/* luis removes    nbDecode = 1;*/

	    if (machPtr->insQueue->entryCount != 0) {
		--machPtr->insQueue->entryCount;

		/* the instruction is removed from the instruction */
		/* queue                                           */

		instructionTmp = instruction->prevPtr;

		if (instructionTmp != NULL)
		    instructionTmp->nextPtr = NULL;
		else
		    machPtr->insQueue->lastElement = NULL;

		instruction->nextPtr = machPtr->insQueue->freeList;
		instruction->prevPtr = NULL;
		machPtr->insQueue->freeList = instruction;

		instruction = instructionTmp;
	    } else {
		instruction = NULL;
	    }

	}

	if (!nbDecode)
	    ++machPtr->counts->decodeStalls;


	intCount = nbRename[INTEGER];
	fpCount = nbRename[FP_SIMPLE] + nbRename[FP_DOUBLE];

	StatsCounts(&machPtr->counts->numRename[0][0],
		    &machPtr->counts->renames[0],
		    NUM_RENAME,
		    intCount, fpCount);

	intCount = nbSearched[INTEGER];
	fpCount = nbSearched[FP_SIMPLE] + nbSearched[FP_DOUBLE];

	StatsCounts(&machPtr->counts->numSearched[0][0],
		    &machPtr->counts->searchedOp[0],
		    NUM_RENAME,
		    intCount, fpCount);

	/* the last accessed instruction is now at the bottom of the queue */
	FIRST(insQueue) = instruction;

	/*
	 * Fetch Stage We fetch a fixed number of instructions under the
	 * following    conditions: the instruction queue is not full yet and
	 * the stop fetch flag has  not been raised
	 */

	if (machPtr->stopFetch)
	    ++machPtr->counts->branchFetchStalls;

	nbFetch = 0;
	i = 0;			/* number of nops: voir si on peut l'enlever */

	if (((machPtr->state =
	      FetchInstruction(machPtr,
			       &nbFetch,
			       &errMsg))
	     != CYCLE_PROCESS)) {
	    goto stopSimulation;
	    /* } */

	}
	/* fetch stalls recorded for statistics */
	if (!nbFetch) {
	    ++machPtr->counts->fetchStalls;
	    if (machPtr->insQueue->entryCount >= machPtr->insQueue->numEntries)
		++machPtr->counts->fullBufferStalls;
	}
	machPtr->counts->fetched += nbFetch;

	/*
	 * Make sure R0 stays zero.
	 */

	machPtr->regs[0] = 0;


	/* stats: occupancy rate of the reorder buffer and instruction window */

	occupancy = 10 * COUNT(iWindow) / MAX(iWindow);
	++machPtr->counts->intBufferOcc[INST_WINDOW][occupancy];

	occupancy = 10 * COUNT(iReorderBuffer) / MAX(iReorderBuffer);
	++machPtr->counts->intBufferOcc[REORDER_BUF][occupancy];

	occupancy = 10 * COUNT(fpWindow) / MAX(fpWindow);

	++machPtr->counts->fpBufferOcc[INST_WINDOW][occupancy];

	occupancy = 10 * COUNT(fpReorderBuffer) / MAX(fpReorderBuffer);

	++machPtr->counts->fpBufferOcc[REORDER_BUF][occupancy];

	/*
	 * detects the end of the program: all the queues are empty
	 */
	switch (format) {
	  case CYCLE:
	    if (stepCount == numberOfSteps) {
		machPtr->state = END_OF_STEP;
	    }
	    break;
	  case INSTRUCTION:
	    if ((machPtr->counts->fetched - startFetch) >= numberOfSteps) {
		machPtr->state = END_OF_STEP;
	    }
	    break;
	}

	if (((!machPtr->insQueue->entryCount) && (machPtr->cycleCount)
	     && (!machPtr->iReorderBuffer->entryCount)
	     && (!machPtr->fpReorderBuffer->entryCount))) {
	    machPtr->state = END_OF_SIMULATION;
	    result = TCL_OK;
	    machPtr->stopFetch = FETCH;
	}

    }
stopSimulation:
    switch (machPtr->state) {
      case END_OF_STEP:

	
	printf("--> Clock Cycle %d\n", machPtr->cycleCount);
	tmp = INDEX_TO_ADDR(machPtr->regs[PC_REG]);
	sprintf(interp->result,
		"  stopped at pc = %.100s: %.50s",
		Sym_GetString(machPtr, tmp),
		Asm_Disassemble(machPtr,
		 machPtr->memPtr[machPtr->regs[PC_REG]].value, tmp & ~0x3));
	result = TCL_OK;
	machPtr->state = CYCLE_PROCESS;
	break;

      case END_OF_SIMULATION:
	sprintf(interp->result,"End of Simulation\n");
	result = TCL_OK;
	break;
      case ERROR:
	tmp = INDEX_TO_ADDR(machPtr->regs[PC_REG]);
	sprintf(interp->result, "%s, pc = %.50s: %.60s\n", errMsg,
		Sym_GetString(machPtr, tmp),
		Asm_Disassemble(machPtr,
				machPtr->memPtr[machPtr->regs[PC_REG]].value,
				tmp & ~0x3));
	strcat(interp->result,
	  "The machine has been reset: please (re)load your \".s\" files.");
	result = TCL_ERROR;
	Reset(machPtr, 1);
	break;

      case STOP_SIMULATION:
	printf("--> Clock Cycle %d\n", machPtr->cycleCount);
	strcat(interp->result,
	       "\nCommand Failed: check your starting address or the loaded program ");
	machPtr->state = CYCLE_PROCESS;
	machPtr->cycleCount = 0;
	result = TCL_ERROR;
	break;
      default:
	strcat(interp->result, 
	"Something wrong happened.\nThe machine has been reset: please (re)load your \".s\" files.\n");
	result = TCL_ERROR;
	Reset(machPtr, 1);
	break;
    }
    sprintf(msg, "%d", machPtr->insCount);
    Tcl_SetVar(machPtr->interp, "insCount", msg, 1);

    return result;

}




/*------------------------------------------------------------------------
 * ForwardOperand
 *
 * For each not yet valid operand of a given instruction, the reorder buffer
 * entry, where to find the value of the operand, is examined to see if the
 * value has been computed: if it has, then the value is forwarded to the
 * operand value  field in the instruction window
 *
 * Result: None
 *
 * Side Effects: The operand valid field can be turned to 1 if its value has
 * been computed.
 *
 * Called by: Simulate Execute
 *
 *--------------------------------------------------------------------------
 */

static void
ForwardOperand(machPtr, operand)
    DLX *machPtr;
    Operand *operand;

{
    ReorderElement *reorder;

    if (operand->valid != 1) {
	if (operand->type == INTEGER) {
	    reorder = &machPtr->iReorderBuffer->element[operand->value[0]];
	} else {
	    reorder = &machPtr->fpReorderBuffer->element[operand->value[0]];
	}
	if (reorder->valid == 1) {
	    if (operand->value[1] == 0) {
		operand->value[0] = reorder->result[0];
		operand->valid = 1;
		if (operand->type == FP_DOUBLE)
		    operand->value[1] = reorder->result[1];
	    } else if (operand->value[1] == -1) {
		/*
		 * the operand is a floating point simple, the value of which
		 * is the second part of a double
		 */
		operand->value[0] = reorder->result[1];
		operand->value[1] = 0;
		operand->valid = 1;
	    }
	}
    }
}


/*-------------------------------------------------------------------------
 * ResultCommit --
 *
 *      It writes the validated results of a given reorder buffer in the
 *      registers.
 *      The reorder buffer is accessed in order from head to tail, and the
 *      head is incremented (modulo the buffer size) each time a result is
 *      committed. When a reorder buffer entry with a non yet validated
 *      result is met, the process is stopped.
 * Results:
 *       None
 *
 *Side Effects:
 *      Instructions are removed from the reorder buffer and the register
 *      file is updated
 *
 * Called By:
 *       Simulate
 *------------------------------------------------------------------------
 */
static void
ResultCommit(machPtr, buffer, holdTrap, nbCommit,type)
    DLX *machPtr;
    ReorderBuffer *buffer;
    int *holdTrap;
    int *nbCommit;
    int type;


{
    register ReorderElement *reorder;
    int entry;

    int stopCommit, registerNb;
    DataOp *store;

    stopCommit = 0;

    entry = buffer->head;

    while ((stopCommit == 0) && (buffer->entryCount > 0)) {
	reorder = &buffer->element[entry];
	if (reorder->flush == 0) {


	    if (reorder->valid) {
		/*if (*holdTrap)*/
		    /* This is used when the floating point reorder buffer
 		     * is processed, to synchronize it with the integer reorder
		     * buffer.
		     * hodTrap indicates that the integer reorder buffer 
		     * holds a trap at its head. So we have to 
		     * stop commiting instructions that are
		     * younger than the trap  (note: younger means larger
		     * instruction count)
		     */

		  /*  if (reorder->insCount > *holdTrap) {
			stopCommit = 1;
			break;
		    }*/

		switch (reorder->classe) {
		  case STORE:
		  case FSTORE:
		    /*
		     * when a store arrives at the head of the reorder buffer
		     * we mark it as "released" in the store buffer     so
		     * that it may be sent for memory write in a future
		     * execute stage: this way, we ensure the stores are
		     * executed after all the instructions that preceed them
		     * in the program
		     */
		    store = reorder->dataOp;
		    if (store->dataToStore->valid == 1) {
			store->released = 1;
		    } else {
			stopCommit = 1;
		    }
		    break;
		  case TRAP:{
			if (reorder->valid == 2) {
			    /*
			     * valid set to 2 means the trap has not yet been
			     * synchronized with the store buffer :
			     * it must not bypass any younger instructions in
			     *  other store buffer
			     */
			    DataOp *store;
			    int check1, check2, found;
			    /* we go to the youngest element in the store */
			    /* buffer is marked released: the TRAP must  */
			    /* not bypass it */
			    found = 0;
			    store = FIRST(storeBuffer);
			    while (store != NULL) {
				if (store->flush != 1) {
				    found = 1;
				    break;
				}
				store = store->prevPtr;
			    }

			    /*
			check1 = !machPtr->fpReorderBuffer->entryCount ||
				(COUNT(fpReorderBuffer) &&
				 reorder->insCount < HEAD(fpReorderBuffer).insCount);*/


			    check2 = !found || (found &&
				       reorder->insCount < store->insCount);



			    if (/*check1 && */ check2) {

				/*
				 * then the trap is older than the element at
				 * the head of the other reorder buffer
				 */
				switch (Handle_Trap(machPtr,
				      ((reorder->result[1]) & 0x3ffffff))) {
				  case 1:
				    printf("TRAP #%d failed\n",
					   (reorder->result[1]) & 0x3ffffff);

				    break;
				  case 2:
				    printf("TRAP #%d received\n",
					   (reorder->result[1]) & 0x3ffffff);

				    break;
				}

				/*
				 * the trap can be processed and the its
				 * result is placed in R1 (see trap.c). It is
				 * validated so that the result be forwarded
				 */

				reorder->valid = 1;
				reorder->result[0] = machPtr->regs[1];
				stopCommit = 1;
				++machPtr->numTrap;
			    } else {
				/*
				 * the trap cannot be handled, because it
				 * would bypass instructions from the float
				 * reorder buffer. we thus indicate to the float
				 * commit process that a trap stands at the
				 * head of the integer reorder buffer. The
				 * holdTrap flag records the i-count of the
				 * trap so that no instruction from the float
				 * buffer bypass it
				 */
				/* *holdTrap = reorder->insCount;*/
				stopCommit = 1;
			    }

			} else {
			    if (++*nbCommit == machPtr->numCommit) 
			    	stopCommit = 1;
			}
			break;
		    }

		  default:
		    registerNb = reorder->registerNb;
		    if ((reorder->type != INTEGER) && (reorder->type != NONE) &&
			(reorder->registerNb != FP_STATUS))
			/* floating point result registers */
			registerNb += 32;

		    if (registerNb != 0) {

			machPtr->regs[registerNb] = (int) (reorder->result[0]);
			if (reorder->type == FP_DOUBLE)
			    machPtr->regs[registerNb + 1] = reorder->result[1];

			++machPtr->counts->writes[type];

			if (--machPtr->counts->regWrites[registerNb] > 0) {
			    /*
			     * if the instruction is followed in the reorder
			     * buffer by another one with the same result
			     * register, then the register write is "useless"
			     */
			    ++machPtr->counts->uselessWrites[type];

			}
		    }
		    break;
		}
	    } else
		stopCommit = 1;
	}
	if (stopCommit == 0) {
	    /*
	     * the reorder buffer entry can be cleared out: but the head is
	     * incremented only if it has not yet reached the tail attention:
	     * when the head reaches the tail, this does not mean that the
	     * number of entries has reached 0. There is still one entry left
	     */

	    reorder->type = NONE;
	    reorder->registerNb = -1;
	    reorder->valid = 0;
	    reorder->ready = 0;
	    reorder->insCount = 0;
	    reorder->cycleCount = 0;
	    reorder->otherReorder = NULL;
	    if (!reorder->flush && reorder->unit != NO_UNIT) {
		if (++*nbCommit == machPtr->numCommit) 
			stopCommit = 1;
	    }
	    if (--buffer->entryCount > 0) {
		if (++entry == buffer->numEntries)
		    entry = 0;
		buffer->head = entry;
	    }
	}
    }
}
/*--------------------------------------------------------------------------
 *
 * WriteBack --
 *      It validates the computed results in the integer reorder buffer:
 *      the ready field of each reorder entry is there to simulate the
 *      latency of the pending operation; it says at which cycle count an
 *      operation has computed its value. Thus, when this cyclecount is
 *      reached, the result can be validated: the valid field is set
 *
 *Result :
 *      None
 *
 *Side Effects
 *      valid bit set to one for the operations that have completed.
 *
 *Called By:
 *      Simulate
 -----------------------------------------------------------------------------
*/


static void
WriteBack(machPtr, reorderBuffer, functUnit, memoryAccess)
    DLX *machPtr;
    ReorderBuffer *reorderBuffer;
    FunctionalUnit *functUnit;
    int memoryAccess;

{
    register int entry, i;
    register ReorderElement *reorder;


    entry = reorderBuffer->head;
    reorderBuffer->ready = 0;

    /* we look at each element in the list of pending operations*/

    reorder = reorderBuffer->listOfExec;
    while (reorder != NULL) {
	ReorderElement *reorderTmp;
	reorderTmp = (ReorderElement *) reorder->nextPtr;
	if ((reorder->ready == machPtr->cycleCount) && (!reorder->flush)) {
	   	    /*
	     * the completion time for the operation has been reached: we can
	     * thus validate the reorder buffer entry
	     */

	    switch (reorder->classe) {
	      case FLOAD:
	      case LOAD:
		if ((reorder->unit == FP_DATA) ||
		    (reorder->unit == INT_DATA)) {

		    /*
		     * the reorder unit field is changed to indicate the load
		     * is entering its second processing step: memory access.
		     * The corresponding load buffer entry is "released",
		     * meaning that the load is now candidate for being sent
		     * to the cache
		     */

		    reorder->dataOp->released = 1;
		    reorder->valid = 0;
		    --functUnit[reorder->unit].num_used;
		    reorder->unit = memoryAccess;
		    reorder->ready = 0;
		} else {
		    reorder->valid = 1;
		}

		break;
	      case FSTORE:
	      case STORE:
		if ((reorder->unit == FP_DATA) ||
		    (reorder->unit == INT_DATA)) {
		    /*
		     * same as load: the store enters the "memory" processing
		     * stage. But, the store is not allowed to process to the
		     * cache. It will allowed to be "released" from the store
		     * buffer only when the corresponding reorder buffer
		     * entry reaches the head of the reorder buffer. So only
		     * the ResultCommit process can release stores
		     */
		    --functUnit[reorder->unit].num_used;
		    reorder->unit = memoryAccess;
		}
		reorder->valid = 1;

		break;

	      case TRAP:{
/*
			ReorderElement *other =
			(ReorderElement *) reorder->otherReorder;
*/
		reorder->valid = 2;
/*
			other->valid = 2;
*/
		break;
}
	      case BRANCH:
		/*
		 * if branch prediction is performed , there is a branch
		 * entry in the other reorder buffer for synchronization
		 * purposes: this other entry also has to be validated
		 */
		if (machPtr->branchPred) {
		    if (memoryAccess == INT_MEM) {
			ReorderElement *other =
			(ReorderElement *) reorder->otherReorder;
			other->valid = 1;
			--functUnit[reorder->unit].num_used;
			reorder->valid = 1;
		    }
		    break;
		}
	      case FBRANCH:
		/* idem as for BRANCH */
		if (machPtr->branchPred) {
		    if (memoryAccess == FP_MEM) {
			ReorderElement *other =
			(ReorderElement *) reorder->otherReorder;
			other->valid = 1;
			--functUnit[reorder->unit].num_used;
			reorder->valid = 1;
		    }
		    break;
		}
	      default:
		/*
		 * for the other instructions, symply free the functional
		 * unit
		 */
		--functUnit[reorder->unit].num_used;
		reorder->valid = 1;
		break;

	    }
	    EndOfExec(reorder, reorderBuffer);
	    if ((--reorderBuffer->checkOps) == 0)
		break;

	} else {
	    if (!reorderBuffer->ready ||
		reorder->ready < reorderBuffer->ready) {
		reorderBuffer->ready = reorder->ready;
	    }
	}
	entry = (++entry) % (reorderBuffer->numEntries);
	reorder = reorderTmp;

    }
    /* end for */

}


/*---------------------------------------------------------------------
 * Execute --
 *      Core of the execute stage It looks in the instruction window
 *      from head to tail to see if instructions can be executed; if they
 *       can, they are removed from the instruction central window.
 *
 * Results
 *       None
 *
 * Side Effects
 *      Calls ForwardOperand before testing if an operation is elligible
 *      for issue, so that it may get operand values that have just been
 *      computed
 *      Calls ExecuteInteger or ExecuteFloat which execute operations and
 *      place the results in the reorder buffer It calls RemoveOperation
 *      to remove the instruction from the instruction window if it has been
 *      issued or flushed.
 *
 *
 * Called by:
 *      Simulate
 *--------------------------------------------------------------------------
 */


static
int
Execute(machPtr, window, reorderBuffer, funcUnits, execCount, errMsg, waitTable, execFunction)
    DLX *machPtr;
    InstructionWindow *window;
    ReorderBuffer *reorderBuffer;
    FunctionalUnit *funcUnits;
    int *execCount;
    char *errMsg;
    int *waitTable;
    int (*execFunction) ();

{

    register WindowElement *operation;
    WindowElement *operationTmp;
    ReorderElement *reorder;
    int issued, wait;

    operation = window->firstElement;

    while (operation != NULL) {

	operationTmp = operation->prevPtr;

	reorder = &reorderBuffer->element[operation->reorderEntry];

	/* step one : see if the instruction is eligible for issue */

	if (reorder->flush) {
	    issued = FLUSHED;

	} else {

	    issued = NOT_ISSUED;
	    /*
	     * look if the non yet valid operands  where just computed in the
	     * reorder buffer: in that case forward  them
	     */

	    if (operation->firstOperand->valid != 1) {
		ForwardOperand(machPtr, operation->firstOperand);
	    }
	    if (operation->secondOperand->valid != 1) {
		ForwardOperand(machPtr, operation->secondOperand);
	    }
	    /*
	     * if the operands are available, we still have to check for
	     * structural hazards
	     */


	    if ((operation->firstOperand->valid == 1) &&
		(operation->secondOperand->valid == 1)) {


		FunctionalUnit *functUnit = &funcUnits[operation->unit];
		issued = ISSUED;
		switch (operation->classe) {
		  case TRAP:
		    reorder->ready = machPtr->cycleCount + 1;
		    break;

		  default:
		    if (functUnit->num_used < functUnit->num_units) {
			reorder->ready =
			    machPtr->cycleCount + functUnit->latency;
			++functUnit->num_used;

		    } else {
			issued = NOT_ISSUED;
			break;
		    }

		}
	    }
	}
	/*
	 * the operation is ready for issue: we call ExecuteInteger of
	 * ExecuteFloat to compute the result depending on the opcode
	 */

	if (issued == ISSUED) {
	    ++*execCount;
	    ++reorderBuffer->checkOps;

	    /* the operation is entered in the list of pending ops*/
	    StartOfExec(reorder, reorderBuffer);

	    if ((issued = execFunction(machPtr, operation, reorder, errMsg))
		!= ISSUED)
		return STOP_SIMULATION;

	   /* we calculate how long did the operation stayed in the window*/
	    if ((wait = machPtr->cycleCount - operation->cycleDecode) <
		NUM_OPS) {
		++waitTable[wait];
		++machPtr->counts->numWait[ALL][wait];
	    } else {
		++waitTable[NUM_OPS - 1];
		++machPtr->counts->numWait[ALL][NUM_OPS - 1];
	    }


	}
	if (issued != NOT_ISSUED)
	    RemoveOperation(window, operation);

	operation = operationTmp;
    }

    return CYCLE_PROCESS;

}

/*------------------------------------------------------------------------
 * StatsCounts --
 *
 *      Updates the counts tables with the measurements collected during
 *      simulation
 *Results:
 *      None
 *Side Effects:
 *      machPtr->counts tables updated
 *Called by:
 *      Simulate
 *--------------------------------------------------------------------------
*/

static void
StatsCounts(tableCycles, tableCounts, bound, intCount, fpCount)
    int *tableCycles;
    int *tableCounts;
    int bound;
    int intCount;
    int fpCount;
{

    int allCount = intCount + fpCount;
    tableCounts[INTEGER] += intCount;
    if (intCount < bound) {
	++*(tableCycles + bound * INTEGER + intCount);
    } else {
	++*(tableCycles + bound * INTEGER + NUM_OPS - 1);
    }
    tableCounts[FLOAT] += fpCount;

    if (fpCount < bound) {
	++*(tableCycles + bound * FLOAT + fpCount);
    } else {
	++*(tableCycles + bound * FLOAT + NUM_OPS - 1);
    }
    tableCounts[ALL] += allCount;

    if (allCount < bound) {
	++*(tableCycles + bound * ALL + allCount);
    } else {
	++*(tableCycles + bound * ALL + NUM_OPS - 1);
    }

}
