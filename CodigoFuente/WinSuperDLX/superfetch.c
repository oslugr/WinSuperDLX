
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

/* indent -i4 -cli0.5 -di1 */

/*
 * superfetch.c
 *
 *
 * This file contains the function that simplements the fetch mechanism of the
 * simulated machine
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "dlx.h"
#include "superdlx.h"
#include "asm.h"

static char rcsid[] = "$Id: superfetch.c,v 1.15 93/04/13 23:44:55 cecile Exp $";
/*
 * $Log:	superfetch.c,v $
 * Revision 1.15  93/04/13  23:44:55  cecile
 * *** empty log message ***
 *
 * Revision 1.14  93/04/13  14:05:36  cecile
 * Supersimulator source file
 *
 * Revision 1.13  93/04/11  21:08:50  cecile
 * Problem of fetching many nops fixed
 *
 * Revision 1.12  93/04/01  18:17:54  cecile
 * save version before trap changes
 *
 * Revision 1.11  93/03/22  19:53:35  cecile
 * Checking for fetch addess errors
 *  Revision 1.10  93/02/16  11:59:31  cecile
 * Error checking revised: now if we encounter the RFE instruction, the
 * simulation has to stop (other cases of wrong starting addresses for the
 * "go" command
 *
 * Revision 1.9  93/02/03  12:22:41  cecile Implementation of error checking: if
 * a lot of nops are fetched in a row (meaning that no program is loaded or
 * the starting address for the program has been badly specified by the
 * user), then the STOP_SIMULATION flag is raised.
 *
 * Revision 1.8  93/01/30  19:08:05  cecile branch prediction revised: there was
 * a problem when branch was predicted in a given reorder buffer and
 * dependent instruction from the other reorder buffer would commit before
 * the outcome of the branch is calculated. also, here now we treat errors
 * such as when someone types the "go" command and no program is loaded.
 * return(fetch) has been changed to return(machPtr->state) so we will be
 * able to handle errors better.
 *
 * Revision 1.7  92/11/04  14:09:34  cecile version without branch bredixtion
 *
 * Revision 1.6  92/09/28  14:53:31  cecile
 *
 * New Structure for load/store buffers. This does not directly affect fetch but
 * we have removed the branch prediction mechanism, to test the new version.
 *
 * Revision 1.5  92/08/27  14:25:46  cecile Implementation of Branch Prediction
 * with a branch target buffer.
 *
 * Revision 1.4  92/08/26  10:20:16  cecile New data structure for instruction
 * queue .
 *
 * Revision 1.3  92/08/16  12:47:55  cecile Now only trap#0 (exit) is executed
 * at fetch stage. The other traps are entered in the instruction queue and
 * treated as the other instructions. The nop are ignored in the counting of
 * fetch and dynamic instruction counts.
 *
 * Revision 1.2  92/08/09  18:20:41  cecile branches have been implemented.
 *
 * Revision 1.1  92/06/11  14:55:06  cecile Initial revision
 *
 */

#define NUM_NOPS 100	/* number of nops fetched in a row before
				declaring there is a problem*/
extern int Handle_Trap();




/*
 * ----------------------------------------------------------------------
 * FetchInstruction  --
 *
 * This procedure reads from the memory array each next instruction to fetch.
 * The address (and index of array) of the instruction is given by the pc.
 * Each fetched instruction is placed at the tail of the instruction queue
 * unless it is unnecessary (nop, direct jumps..)
 *
 * Results:
 *	The machine state which may indicate an error has occurred
 *
 * Called by:
 *	Simulate in supersim.c
 * ----------------------------------------------------------------------
 */
int
FetchInstruction(machPtr, nbFetch, errMsg)
    DLX *machPtr;
    int *nbFetch;
    char *errMsg[];

{

    int insertQueue;
    register InstructionElement *instruction;
    unsigned int pc, current_pc, prediction;
    register MemWord *wordPtr;
    int bb_index;
    int result;
    static int nopNum;

	nopNum = 0;
    while ((*nbFetch < machPtr->numFetch) && (!machPtr->stopFetch) &&
	   (COUNT(insQueue) < MAX(insQueue))) {

	current_pc = machPtr->regs[PC_REG];
	if (current_pc >= machPtr->numWords) {
	    result = BusError(machPtr, INDEX_TO_ADDR(current_pc), 1);
	    if (result != TCL_OK) {
		printf("fetch: end of simulation");
		return END_OF_SIMULATION;

	    } else {
		return END_OF_STEP;
	    }
	}
	wordPtr = &machPtr->memPtr[current_pc];

	pc = machPtr->regs[PC_REG] + 1;

	/* the instruction is read from memory */
	insertQueue = 1;

	/* when encountered the first time, the instruction */
	/* is compiled.                                     */
	if (wordPtr->opCode == OP_NOT_COMPILED)
	    Compile(wordPtr);

	bb_index = 0;
	prediction = NOT_TAKEN;


	switch (infOp[wordPtr->opCode].classe) {
	    case NONE:
		insertQueue = 0;
		/*
		 * if too many nops are fetched, it means that we are
		 * fetching instructions in a wrong part of memory Probably
		 * the user has entered a bad starting address with the go or
		 * step command
		 */
		if (++nopNum >= NUM_NOPS){
		    *errMsg = "Command Failed: check your starting address";
		    return STOP_SIMULATION;
		}
		if (wordPtr->opCode == OP_RFE) {
		    *errMsg = "Non Implemented Instruction ";
		    return STOP_SIMULATION;
		}
		break;

	    case TRAP:
		if ((wordPtr->extra & 0x3ffffff) == 0) {
		    switch (Handle_Trap(machPtr, 0)) {
			case 1:
			    printf("TRAP #0 failed\n");
			    machPtr->stopFetch = STOP_FETCH;
			    break;
			case 2:
			    fflush(stdout);
			    machPtr->stopFetch = STOP_FETCH;
			    break;
		    }
		    insertQueue = 0;
		}
		break;

/*	    case BRANCH:
	    case FBRANCH:
		if (machPtr->branchPred == 1) {
		    BranchInfo *branch;
		    /*
		     * We look in the branch target buffer to see if the
		     * branch is there (i.e if it has already been met
		     *

		    bb_index = current_pc % machPtr->bb_size;
		    branch = &machPtr->branchBuffer[bb_index];

		    /*
		     * if the branch is in the BTB already, we take its
		     * prediction based on past behaviour of the branch
		     *

		    if (branch->address == current_pc) {
			if ((branch->predictState == TAKEN) ||
			    (branch->predictState == TAKEN_)) {
			    pc = branch->predictTarget;
			    prediction = TAKEN;
			} else {
			    prediction = NOT_TAKEN;
			}
		    } else {
			/*
			 * if the branch is not in the BTB, this means that
			 * we have not met it before (or else , there has
			 * been a collision in the BTB and another branch
			 * replaced this one.. So we have to enter the branch
			 * in the BTB. The prediction field is initialized to
			 * TAKEN
			 *

			branch->address = current_pc;
			branch->predictState = TAKEN_;
			branch->predictTarget = machPtr->regs[PC_REG] + 1 +
			    ADDR_TO_INDEX(wordPtr->extra);
			pc = branch->predictTarget;
			prediction = TAKEN;
 		    }*/
        //línea 203-244 del superfetch.c FetchInstruction
	    case BRANCH:
	    case FBRANCH:
    		if (machPtr->branchPred == 1) {
		      BranchInfo *branch;
		    /*
		     * We look in the branch target buffer to see if the
		     * branch is there (i.e if it has already been met
		     */

		      bb_index = current_pc % machPtr->bb_size;
		      branch = &machPtr->branchBuffer[bb_index];

	    /*
		     * if the branch is in the BTB already, we take its
		     * prediction based on past behaviour of the branch
		     */

		      if (branch->address == current_pc) {

		        if ((getPred(branch,machPtr->branchSchema) == TAKEN) ||
                                (getPred(branch,machPtr->branchSchema) == TAKEN_)) {
				      pc = branch->predictTarget;
	  				  prediction = TAKEN;
                        } else {
			  	    prediction = NOT_TAKEN;
			}
		      } else {
			/*
			 * if the branch is not in the BTB, this means that
			 * we have not met it before (or else , there has
			 * been a collision in the BTB and another branch
			 * replaced this one.. So we have to enter the branch
			 * in the BTB. The prediction field is initialized to
			 * TAKEN
			 */

  				branch->address = current_pc;
//				branch->predictState = TAKEN_;
                                brInit(branch,machPtr->branchSchema);//inicializo los valores añadidos por mi
//		  		setPredValue(branch,machPtr->branchSchema,TAKEN_);
			   //	  branch->currentIns=0; //es necesario hacer esta asignación y además en este orden
			 	branch->predictTarget = machPtr->regs[PC_REG] + 1 + ADDR_TO_INDEX(wordPtr->extra);
				pc = branch->predictTarget;
				prediction = TAKEN;
 		    }

		} else {
		    machPtr->stopFetch = STOP_FETCH;
		}
		break;

	    case DIRECT_JUMP:
		/*
		 * in case of a direct jump. We can execute it here and we
		 * don't have to enter it in the instruction queue.. We
		 * continue to fetch instructions on the same cycle, after
		 * the pc has been changed. Alternatively, we could stop the
		 * fetch and continue it in the next cycle
		 */
		pc = machPtr->regs[PC_REG] + 1 + ADDR_TO_INDEX(wordPtr->extra);
		if (wordPtr->opCode == OP_J) {
		    insertQueue = 0;
		    ++machPtr->insCount;
		    ++(*nbFetch);

		}
		break;
	    case INDIRECT_JUMP:
		/*
		 * in case of an indirect jump.. we have to stop fetching
		 * until the jump register 's value has been computed.
		 */

		machPtr->stopFetch = STOP_FETCH;
	    default:
		break;
	}

	/* Insertion of the memory word into the instruction queue */

	if (insertQueue) {
	    InstructionElement *newFreeList;
	    nopNum = 0;
	    ++machPtr->insCount;
	    ++(*nbFetch);
	    instruction = machPtr->insQueue->freeList;
	    instruction->wordPtr = wordPtr;
	    instruction->address = machPtr->regs[PC_REG];
	    instruction->insCount = machPtr->insCount;
	    instruction->cycleCount = machPtr->cycleCount;
	    instruction->prediction = prediction;
	    instruction->bbEntry = bb_index;

	    newFreeList = machPtr->insQueue->freeList->nextPtr;
	    if (newFreeList != NULL)
		newFreeList->prevPtr = NULL;

	    instruction->nextPtr = machPtr->insQueue->lastElement;

	    if (machPtr->insQueue->lastElement != NULL)
		machPtr->insQueue->lastElement->prevPtr = instruction;
	    machPtr->insQueue->lastElement = instruction;
	    instruction->prevPtr = NULL;
	    machPtr->insQueue->entryCount += 1;
	    if (machPtr->insQueue->entryCount == 1)
		machPtr->insQueue->firstElement = instruction;

	    machPtr->insQueue->freeList = newFreeList;


	}
	machPtr->regs[PC_REG] = pc;

    }
    return (CYCLE_PROCESS);
}
