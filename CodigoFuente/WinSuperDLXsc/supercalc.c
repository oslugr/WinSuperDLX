
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
 * supercalc.c
 *
 *
 * This files contains the execution functions for the superscalar simulator
 *
 *
 */


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <values.h>
#include "tcl/tcl.h" //cambiado
#include "dlx.h"
#include "superdlx.h"

static char rcsid[] = "$Id: supercalc.c,v 1.18 1993/12/05 01:02:06 lozano Exp $";
/*
 * $Log: supercalc.c,v $
 * Revision 1.18  1993/12/05  01:02:06  lozano
 * Fix a Bug with MOVF and MOVD. They were not working
 *
 * Revision 1.18  1993/12/05  01:02:06  lozano
 * Fix a Bug with MOVF and MOVD. They were not working
 *
 * Revision 1.17  1993/04/13  23:44:43  cecile
 * *** empty log message ***
 *
 * Revision 1.16  93/04/13  14:05:33  cecile
 * Supersimulator source file
 *
 * Revision 1.15  93/04/09  19:39:21  cecile
 * new mechanisms for load bypassing stores: look for non explicit memory
 * conflits
 *
 * Revision 1.14  93/04/01  18:12:35  cecile
 * executing instructions are now entered in a list of pending operations.
 * 
 * Revision 1.13  93/03/22  19:49:29  cecile
 * New handling of branch prediction recovery: now two entries are provided
 * for branches:one in each reorder buffer. So flushing instructions from the
 * other reorder buffer is made easier.
 * New functions to enter operations in the list of pending operations, and
 * to remove them
 *  Revision 1.12  93/02/16  11:58:13  cecile
 * Collection of stats
 * 
 * Revision 1.11  93/02/03  12:21:20  cecile Implementation of error checking:
 * when overflow, bad read or bad write, the STOP_SIMULATION od ERROR flag is
 * raised to be interpreted by Simulate
 * 
 * Revision 1.10  93/01/30  19:25:28  cecile branch prediction revised .
 * 
 * Revision 1.9  92/09/28  14:48:40  cecile New structure for load/store buffer:
 * a single load buffer and store buffer is used. Note : this version has
 * been tested without branch prediction
 *
 * Revision 1.8  92/08/27  14:28:42  cecile Implementation of the branch target
 * buffer
 * 
 * Revision 1.7  92/08/26  10:21:09  cecile New data structure for instruction
 * queue.
 * 
 * Revision 1.6  92/08/24  14:34:08  cecile Changs in the structure of the
 * instrcution window. This slightly affects RemoveOperation. In particular
 * there is no need to free the elements of the instruction window when
 * executed. They are just added to the free list (no system call "free")
 * 
 * Revision 1.5  92/08/16  12:52:39  cecile Handles traps. The trap number is
 * placed in the result field of the reorder buffer. But the trap is not
 * executed here (will be in result commit). Bug suppressed which blocked
 * loads when they could not bypass stores. Some instructions have been
 * corrected: sll, slli, div , xor, xori, sar, sari.
 * 
 * Revision 1.4  92/08/09  18:18:25  cecile New procedures for branch execution,
 * load/store execution also all the instructions have been implemented works
 * with f3_2.s fdata.s
 * 
 * Revision 1.3  92/07/22  17:09:39  cecile The store and load buffers have been
 * implemented. In terms of load/store execution stage, it means that we have
 * now two steps: an address calculation stage, and then a data access stage.
 * In the first stage, the calculation is performed if the operands are
 * available and a functional unit is availa ble, then the load/store is
 * placed in its appropriate buffers. At each cycle, the load buffer is
 * emptied depending on whether or not the loads can bypass the stores,
 * depending also on the number of data accesses that are permitted on a
 * single cycle. Then once the loads have been processed to the data cache ,
 * and if the number of data accesses has not reached its maximum, then
 * stores can be sent to the cache. For a store to be sent to the cache, two
 * conditions must be fulfilled: the data to store must be valid, and the
 * store must have been released. The validation of the data to store is
 * performed by the ForwardStoreBuffer of supersim.c, whereas the release of
 * the store is done when it reaches the bottom of the reorder buffer (see
 * ResultCommit)
 * 
 * Revision 1.2  92/06/11  14:59:21  cecile
 * 
 * Some simple operations have been implemented for the integer arithmetics
 * mainly. The floating point arithmetic needs to be urgently dealt with.
 * 
 */

static void RemoveData();
static void BranchExecute();

/*-----------------------------------------------------------------------
 * StartOfExecution --
 *
 *	When an operation is issued, its reorder buffer entry is added 
 * 	to the list of currently executing operations.
 * Result:
 *	None
 * Side Effect:
 *	The reorder buffer entry is linked to the rest of the pending 
 *	operation reorder buffer entries. 
 * Called by: 
 *	Execute in sim.c
 *----------------------------------------------------------------------
 */
void
StartOfExec(reorder,reorderBuffer)
ReorderElement *reorder;
ReorderBuffer *reorderBuffer;
{
		
	(ReorderElement *) reorder->nextPtr =reorderBuffer->listOfExec;
	if (reorderBuffer->listOfExec != NULL) {
		(ReorderElement *) reorderBuffer->listOfExec->prevPtr = reorder;
	}
	reorderBuffer->listOfExec = reorder;

	if (!reorderBuffer->ready || reorder->ready < reorderBuffer->ready)
		reorderBuffer->ready = reorder->ready;

} 
/*-----------------------------------------------------------------------
 * EndOfExecution --
 *
 *	When an operation completes, its corresponding reorder buffer 
 * 	entry is removed from the list of pending operations
 * Result: 
 *	None
 * Called by: 
 *	Execute in sim.c
 *----------------------------------------------------------------------
 */

void
EndOfExec(reorder,reorderBuffer)
ReorderElement *reorder;
ReorderBuffer *reorderBuffer;
{
	
	ReorderElement *reorderTmp;

	if (reorder->prevPtr != NULL) {
		reorderTmp = (ReorderElement *) reorder->prevPtr;
		 reorderTmp->nextPtr = ((ReorderElement *)reorder)->nextPtr;
	} else 
		reorderBuffer->listOfExec = (ReorderElement *) reorder->nextPtr;
	
	if (reorder->nextPtr != NULL) {
		reorderTmp = (ReorderElement *) reorder->nextPtr;
		reorderTmp->prevPtr = ((ReorderElement *)reorder)->prevPtr;
	}
	reorder->prevPtr = NULL;
	reorder->nextPtr = NULL;
	
}

/*-----------------------------------------------------------------------
 * RemoveOperation  --
 *
 *      Before handling the next instruction, the current  operation is
 *      removed from the  window , if it has been issued, or flushed
 *
 * Result: 
 *	None
 * Side Effect:
 *      The removed operation frees a window entry which is placed
 *      in the freelist. The freed entry is removed from the occupied
 *      list
 * Called by:
 *	Execute in sim.c
 *----------------------------------------------------------------------
 */


void
RemoveOperation(window, operation)
    InstructionWindow *window;
    WindowElement *operation;

{
    if (window->entryCount > 0) {
	--window->entryCount;

	if (operation->prevPtr != NULL)
	    operation->prevPtr->nextPtr = operation->nextPtr;
	else
	    window->lastElement = operation->nextPtr;
	if (operation->nextPtr != NULL)
	    operation->nextPtr->prevPtr = operation->prevPtr;
	else
	    window->firstElement = operation->prevPtr;

	operation->nextPtr = window->freeList;
	operation->prevPtr = NULL;
	window->freeList = operation;
    }
}

/*------------------------------------------------------------------------
 * RemoveData --
 *
 *      it removes a load/store entry from the load/store buffers.
 *      (we should have used the above procedure, making it generic... later..)
 *
 * Results:
 *	None
 * Side Effects:
 *      add one freed slot to the free list, delete
 *      one from the occupied list that build up the buffer
 * Called by:
 *	SendLoads, SendStores in supercalc.c
 *--------------------------------------------------------------------------
 */

static void
RemoveData(dataBuffer, dataOp, type)
    DataBuffer *dataBuffer;
    DataOp *dataOp;
    int type;

{


    --dataBuffer->entryCount;

    if (dataOp->prevPtr != NULL)
	dataOp->prevPtr->nextPtr = dataOp->nextPtr;
    else
	dataBuffer->lastElement = dataOp->nextPtr;

    if (dataOp->nextPtr != NULL)
	dataOp->nextPtr->prevPtr = dataOp->prevPtr;
    else
	dataBuffer->firstElement = dataOp->prevPtr;
    dataOp->loadBlck = 0;
    dataOp->nextPtr = dataBuffer->freeList;
    dataOp->prevPtr = NULL;
    dataBuffer->freeList = dataOp;
    dataBuffer->freeList->flush = 0;
}
/*-------------------------------------------------------------------------
 * BranchExecute --
 *
 *      This function is called to execute a branch operation:
 *      i.e compute the target address and the outcome of the branch.
 *      There are two cases: the branch prediction option is on or off.
 *      If it is off: the branch target is placed in the program counter
 *      and the fetch process is told to restart
 *      (it was stalling since the branch was first met). If the branch
 *       prediction option is on: what we have to do here is to check the
 *      prediction that was made at fetch for the branch, if it happens to be
 *      good, we just "increase" the trust we had in this prediction in the
 *      branch target buffer. Otherwise, we have to repair the effect of the
 *      bad prediction: flush the instructions that depend on the branch in the
 *      reorder buffers, empty the instruction queue.
 *
 * Result:
 *      none
 * Called by:
 *      ExecuteInteger and ExecuteFloat
 * Side Effect:
 *      modification of the PC, modification of the branch target buffer.if bad
 *      prediction: flush fields are set in the reorder buffers, and the
 *      instruction queue is emptied
 *--------------------------------------------------------------------------*/
static void
BranchExecute(machPtr,
	      ownWindow,
	      ownReorderBuffer,
	      otherReorderBuffer,
	      operation,
	      outcome,
	      ownFuncUnit,
	      otherFuncUnit)
    DLX *machPtr;
    InstructionWindow *ownWindow;
    ReorderBuffer *ownReorderBuffer;
    ReorderBuffer *otherReorderBuffer;
    WindowElement *operation;
    int outcome;
    FunctionalUnit *ownFuncUnit;
    FunctionalUnit *otherFuncUnit;

{
    BranchInfo *branch;
	int collision = 0;

    ++machPtr->counts->branches;

    /* we must check if there is no collision */

    if (machPtr->branchPred == 0) {
	if (outcome == TAKEN) {
	    ++machPtr->counts->branchYes;
	    machPtr->regs[PC_REG] = operation->address + 1 +
		ADDR_TO_INDEX(operation->secondOperand->value[0]);
	} else {
	    ++machPtr->counts->branchNo;
	    machPtr->regs[PC_REG] = operation->address + 1;
	}

	/* fetch can start again */

	machPtr->stopFetch = NEXT_CYCLE_FETCH;

    } else {



	branch = &machPtr->branchBuffer[operation->bbEntry];
	/*
	   if the branch is not any more in the branch target buffer,
	   this means the branch has be replaced by another one in the
	   same BTB entry: this cause problems.. we choose not to
	   change the BTB entry.
	*/
	if (branch->address != operation->address) {
	    printf("Collision in the Branch Target Buffer\n");
		collision = 1;
		++machPtr->counts->branchCollisions;
		}

	/* update the prediction information, based on the new */
	/* outcome  , if there was no collision                */

	/*if (!collision) {
		if (outcome == TAKEN) {
	    	++machPtr->counts->branchYes;
	    	if (branch->predictState != TAKEN) {
				branch->predictState += 1;
	    	}
	    	if (branch->predictState == TAKEN) {
				++machPtr->counts->correctBranchYes;
	    	}
		} else {
	    	++machPtr->counts->branchNo;
	    	if (branch->predictState != NOT_TAKEN) {
				branch->predictState -= 1;
	    	}
	    	if (branch->predictState == NOT_TAKEN) {
				++machPtr->counts->correctBranchNo;
	    	}
		}
	}  */
 //branch es un puntero a un elemento de la btb
  if (!collision) {
                insertOutcome(branch,outcome,machPtr->branchSchema[1]);//insertamos el resultado del salto actual en la estructura
		if (outcome == TAKEN) { //el outcome es un parámetro que se le pasa a la función.Creo que es el resultado de la instrucción actual,
								//y se cambia de estado en función de éste.
								//parece que en función del tipo de instrucción de salto, se le pasa un tipo de predicción

        	    	++machPtr->counts->branchYes;
                        //comprobamos para la secuencia que acabamos de generar si se ha de cambiar de estado
			if (getPred(branch,machPtr->branchSchema)!=TAKEN)//comprobamos que no nos pasemos de 3
			{
				setPred(branch,machPtr->branchSchema,1,outcome);
			}
			if (getPred(branch,machPtr->branchSchema)==TAKEN)//si acierto me quedo en el mismo estado
			{
				++machPtr->counts->correctBranchYes;//tengo que avanzar además el puntero de la instrucción actual, por que no se ha errado la predicción
                                //no tengo que hacer nada puesto que no cambio de estado
			       //	setPredValue(branch,machPtr->branchSchema,TAKEN);
			}

		} else {
        	    	if (getPred(branch,machPtr->branchSchema)!= NOT_TAKEN) {//comprobamos que no sea 0
	  			setPred(branch,machPtr->branchSchema,-1,outcome);
	            	}
	    	        if (getPred(branch,machPtr->branchSchema) == NOT_TAKEN) {
  				++machPtr->counts->correctBranchNo;
                        //no tengo que hacer nada puesto que no cambio de estado
   			//	setPredValue(branch,machPtr->branchSchema,NOT_TAKEN);
	    	        }
		}
    }

	/* if outcome different from prediction: then set the flush */
	/* flag to 1, for the instructions that appear after the    */
	/* the branch in its own reorder buffer, and for the        */
	/* instructions that are older than it in the other reorder */
	/* buffer                                                   */

	if (outcome != operation->prediction) {
	    InstructionElement *instruction;
	    int lostCycle = 0, startCycle = 0;
	    int entry, i, found;
	    ReorderElement *reorder;


	    /* compute the new pc to be used by the fetch process        */
	    /* on the NEXT CYCLE                                         */

	    if (outcome == TAKEN)
		machPtr->regs[PC_REG] = operation->address + 1 +
		    ADDR_TO_INDEX(operation->secondOperand->value[0]);
	    else
		machPtr->regs[PC_REG] = operation->address + 1;


	    /* flush the instructions on the reorder buffer of the  branch                                               */

	    entry = operation->reorderEntry;

	    while (entry != ownReorderBuffer->tail) {
		int useUnit = 1;
		if (++entry == ownReorderBuffer->numEntries)
		    entry = 0;
		reorder = &ownReorderBuffer->element[entry];
		if (!reorder->flush) {

		    reorder->flush = 1;
		    ++machPtr->counts->flushed;

		    if (lostCycle == 0) {
			startCycle = reorder->cycleCount;
			lostCycle = reorder->cycleCount;
		    } else if (lostCycle < reorder->cycleCount) {
			lostCycle = reorder->cycleCount;
		    }
		    if ((reorder->classe == FSTORE ||
			 reorder->classe == STORE ||
			 reorder->classe == LOAD
			 || reorder->classe == FLOAD)) {

			reorder->dataOp->flush = 1;

			if ((reorder->unit == INT_MEM ||
			     reorder->unit == FP_MEM))
			    useUnit = 0;
		    }
		    if ((reorder->classe != TRAP) &&
			(reorder->ready > machPtr->cycleCount)) {
			if (useUnit)
			    --ownFuncUnit[reorder->unit].num_used;
			if ((useUnit) || (reorder->classe == LOAD)
			    || (reorder->classe == FLOAD))
			    --ownReorderBuffer->checkOps;
				EndOfExec(reorder,ownReorderBuffer);
		    }
		}
	    }

	    if (ownReorderBuffer->checkOps){	
		for (reorder = ownReorderBuffer->listOfExec;
			reorder != NULL;
			 reorder = (ReorderElement *)reorder->nextPtr) {
		    if (reorder->ready < ownReorderBuffer->ready)
			ownReorderBuffer->ready = reorder->ready;
		 }
		    
	    } else {
		ownReorderBuffer->ready = 0;
	    }

	    /*
	     * flush the instructions that depend on the branch in the other
	     * reorder buffer
	     */
	    entry = operation->otherRentry;
	    reorder = &otherReorderBuffer->element[entry];

	    while (entry != otherReorderBuffer->tail) {
		if (++entry == (otherReorderBuffer->numEntries)) {
		    entry = 0;
		    reorder = &otherReorderBuffer->element[0];
		} else {
		    reorder++;
		}

		if (!reorder->flush) {
		    int useUnit = 1;
		    reorder->flush = 1;

		    if (lostCycle == 0) {
			lostCycle = reorder->cycleCount;
			startCycle = reorder->cycleCount;
		    } else if (lostCycle < reorder->cycleCount) {
			lostCycle = reorder->cycleCount;
		    }
		    if ((reorder->classe == FSTORE ||
			 reorder->classe == STORE ||
			 reorder->classe == LOAD ||
			 reorder->classe == FLOAD)) {
			reorder->dataOp->flush = 1;
			if ((reorder->unit == INT_MEM ||
			     reorder->unit == FP_MEM)) {
			    useUnit = 0;
			}
		    }
		    if ((reorder->classe != TRAP) &&
			(reorder->ready > machPtr->cycleCount)) {
			if (useUnit)
			    --otherFuncUnit[reorder->unit].num_used;
			if ((useUnit) || (reorder->classe == LOAD) ||
			    (reorder->classe == FLOAD))
			    --otherReorderBuffer->checkOps;
				EndOfExec(reorder,otherReorderBuffer);
		    }
		}
	    }



	    if (otherReorderBuffer->checkOps){	
		for (reorder = otherReorderBuffer->listOfExec;
			reorder != NULL;
			reorder =  (ReorderElement *)reorder->nextPtr) {
		    if (reorder->ready < otherReorderBuffer->ready)
			otherReorderBuffer->ready = reorder->ready;
		    }
		    
	    } else {
		otherReorderBuffer->ready = 0;
	    }


	    machPtr->counts->lostCycles += lostCycle - startCycle;

	    /* the instruction queue is emptied:                 */

	    machPtr->insQueue->freeList = machPtr->insQueue->table;

	    for (i = 0, instruction = machPtr->insQueue->freeList;
		 i < machPtr->insQueue->numEntries;
		 i++, instruction++) {
		instruction->prevPtr = NULL;
		if (i != (machPtr->insQueue->numEntries - 1))
		    instruction->nextPtr = &machPtr->insQueue->freeList[i + 1];
		else
		    instruction->nextPtr = NULL;
	    }
	    machPtr->insQueue->entryCount = 0;
	    machPtr->insQueue->firstElement = NULL;
	    machPtr->insQueue->lastElement = NULL;

	    machPtr->stopFetch = NEXT_CYCLE_FETCH;

	}
    }
}

/*--------------------------------------------------------------------------
 * ExecuteInteger ---
 *
 *      It executes the integer operations from the instruction
 *      window of the integer operational unit. The computed result, if any,
 *      is placed in the integer reorder buffer.
 *
 * Results:
 *      error for the machine state or ok flag (i.e operation has been
 *      correctly computed)
 * Side Effects:
 *      modification of the reorder buffer entries
 *--------------------------------------------------------------------------*/
int ExecuteInteger(machPtr, operation, reorder, errMsg)
    DLX *machPtr;
    WindowElement *operation;
    ReorderElement *reorder;
    char *errMsg;
/* attention this will be changed and handled in fetch: set a classe of */
/* error/unimplemented instructions */


{

    unsigned int tmp;
    int result;

    switch (operation->opCode) {

      case OP_ADD:{
	    long sum;
	    sum = (operation->firstOperand->value[0]) +
		(operation->secondOperand->value[0]);

	    if (!((operation->firstOperand->value[0] ^
		   operation->secondOperand->value[0])
		  & SIGN_BIT) && ((operation->secondOperand->value[0]
				   ^ sum) & SIGN_BIT)) {

		result = Overflow(machPtr, operation);
		if (result != TCL_OK) {
		    machPtr->state = STOP_SIMULATION;
		    return STOP_SIMULATION;
		} else {
		    return END_OF_INSTRUCTION;
		}
	    }
	    reorder->result[0] = sum;
	    break;
	}

      case OP_ADDI:{
	    long sum;
	    sum = (operation->firstOperand->value[0]) +
		(operation->secondOperand->value[0]);

	    if (!((operation->firstOperand->value[0] ^
		   operation->secondOperand->value[0])
		  & SIGN_BIT) && ((operation->secondOperand->value[0]
				   ^ sum) & SIGN_BIT)) {

		result = Overflow(machPtr, operation);
		if (result != TCL_OK) {
		    machPtr->state = STOP_SIMULATION;
		    return STOP_SIMULATION;
		} else {
		    return END_OF_INSTRUCTION;
		}
	    }
	    reorder->result[0] = sum;
	    break;
	}
      case OP_ADDU:
	reorder->result[0] =
	    operation->firstOperand->value[0] +
	    operation->secondOperand->value[0];
	break;
      case OP_ADDUI:
	reorder->result[0] =
	    operation->firstOperand->value[0] +
	    (operation->secondOperand->value[0] & 0xffff);
	break;

      case OP_AND:
	reorder->result[0] =
	    operation->firstOperand->value[0] &
	    operation->secondOperand->value[0];

	break;
      case OP_ANDI:
	reorder->result[0] =
	    operation->firstOperand->value[0] &
	    (operation->secondOperand->value[0] & 0xffff);

	break;
      case OP_BEQZ:{
	    int outcome;
	    /* printf("BEQZ cycle %d \n", machPtr->cycleCount); */

	    outcome = TAKEN;
	    if (operation->firstOperand->value[0] != 0)
		outcome = NOT_TAKEN;

	    BranchExecute(machPtr,
			  machPtr->iWindow,
			  machPtr->iReorderBuffer,
			  machPtr->fpReorderBuffer,
			  operation, outcome,
			  &machPtr->intUnits[0],
			  &machPtr->fpUnits[0]);
	    break;
	}

      case OP_BNEZ:{
	    int outcome;
	    outcome = TAKEN;
	    if (operation->firstOperand->value[0] == 0)
		outcome = NOT_TAKEN;

	    BranchExecute(machPtr,
			  machPtr->iWindow,
			  machPtr->iReorderBuffer,
			  machPtr->fpReorderBuffer,
			  operation,
			  outcome,
			  &machPtr->intUnits[0],
			  &machPtr->fpUnits[0]);
	    break;
	}

      case OP_JAL:

	/* normalement, il y a un nop apres le JAL */
	reorder->result[0] = INDEX_TO_ADDR((operation->address + 1));
	break;

      case OP_JALR:

	/*
	 * normally there is a nop after the jal  if the code is produced by
	 * dlxcc so we could jump it. But we'd rather not, in case another
	 * compiler produces the code
	 */

	reorder->result[0] = INDEX_TO_ADDR((operation->address + 1));

      case OP_JR:

	machPtr->stopFetch = 0;
	machPtr->regs[PC_REG] =
	    ADDR_TO_INDEX(operation->firstOperand->value[0]);
	break;



      case OP_LB:
      case OP_LBU:
      case OP_LH:
      case OP_LHU:
      case OP_LW:
	{
	    /* here, we perform address calculation only.. */

	    operation->dataOp->address =
		operation->firstOperand->value[0] +
		operation->secondOperand->value[0];


	    break;
	}

      case OP_LHI:
	/* implemented as a shift operation: to be checked */
	reorder->result[0] = operation->secondOperand->value[0] << 16;
	break;

      case OP_MOVFP2I:
	reorder->result[0] = operation->firstOperand->value[0];
	/* attention : we did not put in reorder->unit, to be checked */
	break;

      case OP_MOVI2S:
	errMsg = "MOVI2S instruction is unimplemented";
	return ERROR;

      case OP_MOVS2I:
	errMsg = "MOVS2I instruction is unimplemented";
	return ERROR;

      case OP_OR:
	reorder->result[0] = operation->firstOperand->value[0]
	    | operation->secondOperand->value[0];
	break;
      case OP_ORI:
	reorder->result[0] = operation->firstOperand->value[0]
	    | (operation->secondOperand->value[0] & 0xffff);
	break;
      case OP_SB:
      case OP_SH:
      case OP_SW:
	{
	    /* address calculation           */
	    operation->dataOp->address =
		operation->firstOperand->value[0] +
		operation->secondOperand->value[0];

	    break;
	}
      case OP_SEQ:
      case OP_SEQI:
	{
	    if ((int) operation->firstOperand->value[0] ==
		(int) operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	    break;
	}

      case OP_SEQU:
      case OP_SEQUI:
	{
	    if ((unsigned int) operation->firstOperand->value[0] ==
		(unsigned int) operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SGE:
      case OP_SGEI:
	{
	    if (operation->firstOperand->value[0] >=
		operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SGEU:
      case OP_SGEUI:
	{
	    if ((unsigned int) operation->firstOperand->value[0] >=
		(unsigned int) operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SGT:
      case OP_SGTI:
	{
	    if (operation->firstOperand->value[0] >
		operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SGTU:
      case OP_SGTUI:
	{
	    if ((unsigned int) operation->firstOperand->value[0] >
		(unsigned int) operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SLE:
      case OP_SLEI:
	{
	    if (operation->firstOperand->value[0] <=
		operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SLEU:
      case OP_SLEUI:
	if ((unsigned int) operation->firstOperand->value[0] <=
	    (unsigned int) operation->secondOperand->value[0])
	    reorder->result[0] = 1;
	else
	    reorder->result[0] = 0;
	break;

      case OP_SLL:
      case OP_SLLI:
	reorder->result[0] = operation->firstOperand->value[0] <<
	    (operation->secondOperand->value[0] & 0x1f);
	break;

      case OP_SLT:
      case OP_SLTI:
	{
	    if (operation->firstOperand->value[0] <
		operation->secondOperand->value[0])
		reorder->result[0] = 1;
	    else
		reorder->result[0] = 0;
	}
	break;

      case OP_SLTU:
      case OP_SLTUI:
	if ((unsigned int) operation->firstOperand->value[0] <
	    (unsigned int) operation->secondOperand->value[0])
	    reorder->result[0] = 1;
	else
	    reorder->result[0] = 0;
	break;

      case OP_SNE:
      case OP_SNEI:
	if (operation->firstOperand->value[0] !=
	    operation->secondOperand->value[0])
	    reorder->result[0] = 1;
	else
	    reorder->result[0] = 0;
	break;

      case OP_SNEU:
      case OP_SNEUI:

	if ((unsigned int) operation->firstOperand->value[0] !=
	    (unsigned int) operation->secondOperand->value[0])
	    reorder->result[0] = 1;
	else
	    reorder->result[0] = 0;
	break;

      case OP_SRA:
      case OP_SRAI:
	reorder->result[0] = operation->firstOperand->value[0]
	    >> (operation->secondOperand->value[0] & 0x1f);
	break;

      case OP_SRL:
      case OP_SRLI:
	tmp = operation->firstOperand->value[0];
	tmp >>= (operation->secondOperand->value[0] & 0x1f);
	reorder->result[0] = tmp;
	break;

      case OP_SUB:
      case OP_SUBI:
	{
	    int diff;
	    diff = operation->firstOperand->value[0] -
		operation->secondOperand->value[0];
	    if (((operation->firstOperand->value[0] ^
		  operation->secondOperand->value[0]) & SIGN_BIT)
		&& ((operation->firstOperand->value[0] ^ diff) & SIGN_BIT)) {
		result = Overflow(machPtr, operation);
		if (result != TCL_OK) {
		    machPtr->state = STOP_SIMULATION;
		    return STOP_SIMULATION;
		} else {
		    return END_OF_INSTRUCTION;
		}
	    }
	    reorder->result[0] = diff;
	}


	break;

      case OP_SUBU:
	reorder->result[0] = operation->firstOperand->value[0]
	    - operation->secondOperand->value[0];
	break;

      case OP_SUBUI:
	reorder->result[0] = operation->firstOperand->value[0]
	    - (operation->secondOperand->value[0] & 0xffff);
	break;

      case OP_TRAP:
	/* the trap number is placed in the reorder entry, to 
	 used by ResultCommit*/
	reorder->result[1] = operation->secondOperand->value[0];
/*	reorder->valid = 2;*/
	break;

      case OP_XOR:
	reorder->result[0] = operation->firstOperand->value[0]
	    ^ operation->secondOperand->value[0];
	break;

      case OP_XORI:
	reorder->result[0] = operation->firstOperand->value[0]
	    ^ (operation->secondOperand->value[0] & 0xffff);
	break;

    }				/* end switch */


    return ISSUED;

}

/*--------------------------------------------------------------------
 * ExecuteFloat --- 
 *
 *	It executes the fp operations from the instruction window
 * 	of the floating point operational unit. The computed result, 
 *	if any, is placed in the floating point reorder buffer.
 * 
 * Results: 
 *	error for the machine state or ok flag (i.e operation has been
 *	correctly computed) 
 * Side Effects: 
 *	modification of the reorder buffer entries
 *-------------------------------------------------------------------
 */

int
ExecuteFloat(machPtr, operation, reorder, errMsg)
    DLX *machPtr;
    WindowElement *operation;
    ReorderElement *reorder;
    char *errMsg;
{
    int issued;


    int tmp, result;

    int error;
    int sign;

    sign = 0;
    error = 0;

    switch (operation->opCode) {

      case OP_BFPF:{
	    int outcome;
	    outcome = TAKEN;
	    if (operation->firstOperand->value[0] != 0)
		outcome = NOT_TAKEN;
	    BranchExecute(machPtr,
			  machPtr->fpWindow,
			  machPtr->fpReorderBuffer,
			  machPtr->iReorderBuffer,
			  operation, outcome,
			  &machPtr->fpUnits[0],
			  &machPtr->intUnits[0]);

	    break;
	}

      case OP_BFPT:{
	    int outcome;
	    outcome = TAKEN;
	    if (operation->firstOperand->value[0] == 0)
		outcome = NOT_TAKEN;
	    BranchExecute(machPtr,
			  machPtr->fpWindow,
			  machPtr->fpReorderBuffer,
			  machPtr->iReorderBuffer,
			  operation, outcome,
			  &machPtr->fpUnits[0],
			  &machPtr->intUnits[0]);

	    break;
	}

      case OP_LD:
      case OP_LF:{


	    /* the load operation is entered in the load buffer */
	    /* which models the first stage of computation      */
	    /* for the load: i.e the address calculation        */


	    operation->dataOp->address =
		operation->firstOperand->value[0] +
		operation->secondOperand->value[0];


	    break;
	}

      case OP_MOVD:{
	    double *source, *dest;
	    source = (double *) &(operation->firstOperand->value[0]);
	    dest = (double *) &(reorder->result[0]);
	    /* Introduced by LAL, this was missing and it is a big bug! */
	    *dest = *source;	 
	    break;
	}

      case OP_MOVF:{
	    float *source, *dest;
	    source = (float *) &(operation->firstOperand->value[0]);
	    dest = (float *) &(reorder->result[0]);
	    /* Introduced by LAL, this was missing and it is a big bug! */
	    *dest = *source;	 
	    break;
	}

      case OP_MOVI2FP:
	reorder->result[0] = operation->firstOperand->value[0];
	break;

      case OP_SD:
      case OP_SF:
	{

	    /* the store must be entered in the store buffer to      */
	    /* be potentially bypassed by loads of the same or       */
	    /* later cycle                                           */
	    operation->dataOp->address =
		operation->firstOperand->value[0] +
		operation->secondOperand->value[0];
	    break;
	}

	/* floating point arithmetics */

      case OP_ADDD:{
	    double *ds1, *ds2, *dd;
	    ds1 = (double *) &(operation->firstOperand->value[0]);
	    ds2 = (double *) &(operation->secondOperand->value[0]);
	    dd = (double *) &(reorder->result[0]);
	    *dd = *ds1 + *ds2;
	    break;

	}
      case OP_ADDF:{
	    float *fs1, *fs2, *fd;
	    fs1 = (float *) &(operation->firstOperand->value[0]);
	    fs2 = (float *) &(operation->secondOperand->value[0]);
	    fd = (float *) &(reorder->result[0]);
	    *fd = *fs1 + *fs2;
	    break;
	}
      case OP_CVTD2F:{
	    double *source;
	    float *dest;
	    source = (double *) &(operation->firstOperand->value[0]);
	    dest = (float *) &(reorder->result[0]);
	    *dest = (float) *source;
	    break;
	}

      case OP_CVTD2I:{
	    double *source;
	    int *dest;
	    source = (double *) &(operation->firstOperand->value[0]);
	    dest = (int *) &(reorder->result[0]);
	    *dest = (int) *source;
	    break;
	}

      case OP_CVTF2D:{
	    float *source;
	    double *dest;
	    source = (float *) &(operation->firstOperand->value[0]);
	    dest = (double *) &(reorder->result[0]);
	    *dest = (double) *source;
	    break;
	}

      case OP_CVTF2I:{
	    float *source;
	    int *dest;
	    source = (float *) &(operation->firstOperand->value[0]);
	    dest = (int *) &(reorder->result[0]);
	    *dest = (int) *source;
	    break;
	}


      case OP_CVTI2D:{
	    int *source;
	    double *dest;
	    source = (int *) &(operation->firstOperand->value[0]);
	    dest = (double *) &(reorder->result[0]);
	    *dest = (double) *source;
	    break;
	}

      case OP_CVTI2F:{
	    int *source;
	    float *dest;
	    source = (int *) &(operation->firstOperand->value[0]);
	    dest = (float *) &(reorder->result[0]);
	    *dest = (float) *source;
	    break;
	}

      case OP_DIV:{
	    int *source1, *source2, *result;
	    source1 = (int *) &(operation->firstOperand->value[0]);
	    source2 = (int *) &(operation->secondOperand->value[0]);
	    result = (int *) &(reorder->result[0]);
	    if (operation->secondOperand->value[0] == 0) {
		/* divide by 0 */
		*result = 0;
		error = 1;
	    } else
		*result = *source1 / *source2;
	    break;
	}
      case OP_DIVD:{

	    double *ds1, *ds2, *dd;
	    ds1 = (double *) &(operation->firstOperand->value[0]);
	    ds2 = (double *) &(operation->secondOperand->value[0]);
	    dd = (double *) &(reorder->result[0]);
	    *dd = *ds1 / *ds2;
	    break;
	}
      case OP_DIVF:{

	    float *fs1, *fs2, *fd;
	    fs1 = (float *) &(operation->firstOperand->value[0]);
	    fs2 = (float *) &(operation->secondOperand->value[0]);
	    fd = (float *) &(reorder->result[0]);
	    *fd = *fs1 / *fs2;
	    break;
	}

      case OP_DIVU:{
	    unsigned int source1, source2, tmp;
	    source1 = operation->firstOperand->value[0];
	    source2 = operation->secondOperand->value[0];
	    if (source2 == 0) {
		/* divide by 0 */
		*((unsigned int *) &(reorder->result[0])) = 0;
		error = 1;
	    } else {
		tmp = source1 / source2;
		*((unsigned int *) &(reorder->result[0])) = tmp;
	    }
	    break;
	}

      case OP_EQD:{
	    double *source1, *source2;
	    source1 = (double *) &(operation->firstOperand->value[0]);
	    source2 = (double *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 == *source2);
	    break;
	}

      case OP_EQF:{
	    float *source1, *source2;
	    source1 = (float *) &(operation->firstOperand->value[0]);
	    source2 = (float *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 == *source2);
	    break;
	}

      case OP_GED:{
	    double *source1, *source2;
	    source1 = (double *) &(operation->firstOperand->value[0]);
	    source2 = (double *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 >= *source2);
	    break;
	}

      case OP_GEF:{
	    float *source1, *source2;
	    source1 = (float *) &(operation->firstOperand->value[0]);
	    source2 = (float *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 >= *source2);
	    break;
	}

      case OP_GTD:{
	    double *source1, *source2;
	    source1 = (double *) &(operation->firstOperand->value[0]);
	    source2 = (double *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 > *source2);
	    break;
	}


      case OP_GTF:{
	    float *source1, *source2;
	    source1 = (float *) &(operation->firstOperand->value[0]);
	    source2 = (float *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 > *source2);
	    break;
	}

      case OP_LED:{
	    double *source1, *source2;
	    source1 = (double *) &(operation->firstOperand->value[0]);
	    source2 = (double *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 <= *source2);
	    break;
	}


      case OP_LEF:{
	    float *source1, *source2;
	    source1 = (float *) &(operation->firstOperand->value[0]);
	    source2 = (float *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 <= *source2);
	    break;
	}

      case OP_LTD:{
	    double *source1, *source2;
	    source1 = (double *) &(operation->firstOperand->value[0]);
	    source2 = (double *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */

	    reorder->result[0] = (*source1 < *source2);
	    if (*source1 == *source2)
		reorder->result[0] = 0;
	    if ((operation->firstOperand->value[0] ==
		 operation->secondOperand->value[0]) &&
		(operation->firstOperand->value[1] ==
		 operation->secondOperand->value[1]))
		reorder->result[0] = 0;

	    break;
	}

      case OP_LTF:{
	    float *source1, *source2;
	    source1 = (float *) &(operation->firstOperand->value[0]);
	    source2 = (float *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 < *source2);
	    break;
	}


      case OP_MULT:
	sign = 1;
      case OP_MULTU:

	error = Mult(operation->firstOperand->value[0],
		     operation->secondOperand->value[0], sign,
		     (int *) &(reorder->result[0]));
	if (error) {
	    printf("mult error ==> %d", error);
	    errMsg = "integer multiply overflow";
	    return ERROR;
	}
	break;

      case OP_MULTD:{
	    double *ds1, *ds2, *dd;
	    ds1 = (double *) &(operation->firstOperand->value[0]);
	    ds2 = (double *) &(operation->secondOperand->value[0]);
	    dd = (double *) &(reorder->result[0]);
	    *dd = *ds1 * *ds2;
	    break;
	}


      case OP_MULTF:{

	    float *fs1, *fs2, *fd;
	    fs1 = (float *) &(operation->firstOperand->value[0]);
	    fs2 = (float *) &(operation->secondOperand->value[0]);
	    fd = (float *) &(reorder->result[0]);
	    *fd = *fs1 * *fs2;
	    break;
	}

      case OP_NED:{
	    double *source1, *source2;
	    source1 = (double *) &(operation->firstOperand->value[0]);
	    source2 = (double *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 != *source2);
	    break;
	}

	break;
      case OP_NEF:{
	    float *source1, *source2;
	    source1 = (float *) &(operation->firstOperand->value[0]);
	    source2 = (float *) &(operation->secondOperand->value[0]);
	    /* to put in FPstatusReg */
	    reorder->result[0] = (*source1 != *source2);
	    break;
	}


      case OP_SUBD:{
	    double *ds1, *ds2, *dd;
	    ds1 = (double *) &(operation->firstOperand->value[0]);
	    ds2 = (double *) &(operation->secondOperand->value[0]);
	    dd = (double *) &(reorder->result[0]);
	    *dd = *ds1 - *ds2;
	    break;
	}

      case OP_SUBF:{

	    float *fs1, *fs2, *fd;
	    fs1 = (float *) &(operation->firstOperand->value[0]);
	    fs2 = (float *) &(operation->secondOperand->value[0]);
	    fd = (float *) &(reorder->result[0]);
	    *fd = *fs1 - *fs2;
	    break;
	}


    }				/* switch stat */
    return ISSUED;

}

/*------------------------------------------------------------------------
 * SendLoads --
 *
 *      It traverses the load buffer, to check which load can be sent
 *      to memory.
 *
 * Result: 
 *	ok or error flag for the machine state
 * Side Effect:
 *      places the result of the load in the reorder buffer. empties the
 *      load buffer by calling RemoveData.
 * Called by:
 *      Simulate in supersim.c
 *-------------------------------------------------------------------------
 */
int
SendLoads(machPtr, numLoadAccess)
    DLX *machPtr;
    int *numLoadAccess;		/* number of loads that are allowed on this
				 * cc */
{
    DataOp *load, *store;
    int stopLoadAccess;

    stopLoadAccess = 0;

    load = machPtr->loadBuffer->firstElement;
    while (load != NULL) {

	DataOp *loadTmp;
	loadTmp = load->prevPtr;

	if (load->flush == 1) {
	    RemoveData(machPtr->loadBuffer, load);
	} else if (load->released == 1) {
	    int numData;
	    ReorderElement *reorder;
	    int value;
	    int result;
	    int storeBypass;

	    /* the load cannot bypass a store which affects the same 
	       memory location   
		       sd -32(r30),f4
              	       ld f6,-32(r30)
                 
	       attention: sometimes the memory location conflict 
	       is not explicit, when manipulating doubles which also
	       go through 2 integer registers (dlxcc code)
	       example:
			sd -32(r30),f4 should not be bypassed by:
			lw r5,-28(r30)
		or :
			sw -28(r30),r5 should not be bypassed by:
			ld f6,-32(r30)

		in theory, it there are other cases , like that

			sd -32(r30),f4
			lf f5,-28(r30)

		but I don't think this could happen with dlxcc..
		but in case we treat, you never know..
		However, instead of lw above, I don't think we should
		have lh, or whatever byte manipulation, conflicting 
		with a double for memory.
		Anyway, be careful..
		*/


	    store = machPtr->storeBuffer->firstElement;
	    storeBypass = 1;
	    while (store != NULL) {
		if ((load->insCount >= store->insCount) && 
			(!store->flush)) {
		    result = store->address;

		    switch (load->opCode) {
		    	case (OP_LW): 
			case (OP_LF):
			    if (store->opCode == OP_SD)
				result = store->address + 4;
			     break;
			case (OP_LD) :
			    if (store->opCode == OP_SW)
				result =  store->address - 4;
			    break;
		     }

		    if (store->address == load->address ||
				load->address == result) {
		    	storeBypass = 0;
		    	if (!load->loadBlck) {
		    	    load->loadBlck = 1;
		    	    ++machPtr->counts->loadBlck;
		        }
		        break;
		    }
		}
	
		store = store->prevPtr;

   	    }
		
	    /*
	     * if there is no bypassing problem, the load can be sent to
	     * cache
	     */
	    if (storeBypass) {
		numData = 1;

		++*numLoadAccess;
		switch (load->opCode) {


		    /* Integer Loads */
		  case OP_LB:
		  case OP_LBU:{
			reorder =
			    &machPtr->iReorderBuffer->element[load->reorderEntry];
			++machPtr->iReorderBuffer->checkOps;
			
			if (ReadMem(machPtr, load->address, 
				&value,load->pcAddress) == 0) {
			    return STOP_SIMULATION;
			}
			switch ((load->address) & 0x3) {
			  case 0:
			    value >>= 24;
			    break;
			  case 1:
			    value >>= 16;
			    break;
			  case 2:
			    value >>= 8;
			}
			if ((value & 0x80) && (load->opCode == OP_LB)) {
			    value |= 0xffffff00;
			} else {
			    value &= 0xff;
			}
			reorder->result[0] = value;
			reorder->ready = machPtr->cycleCount +
			    machPtr->mem_latency;
			StartOfExec(reorder,machPtr->iReorderBuffer);
			break;
		    }

		  case OP_LH:
		  case OP_LHU:{
			reorder =
			    &machPtr->iReorderBuffer->element[load->reorderEntry];
			++machPtr->iReorderBuffer->checkOps;

			if (load->address & 0x1) {
			    result = AddressError(machPtr, 
					load->address, 1,
					load->pcAddress);
			    if (result != TCL_OK) {
				return STOP_SIMULATION;
			    } else {
				return END_OF_INSTRUCTION;
			    }
			}
			if (ReadMem(machPtr, load->address, &value,
				load->pcAddress) == 0) {
			    return STOP_SIMULATION;
			}
			if (!(load->address & 0x2)) {
			    value >>= 16;
			}
			if ((value & 0x8000) && (load->opCode == OP_LH)) {
			    value |= 0xffff0000;
			} else {
			    value &= 0xffff;
			}
			reorder->result[0] = value;
			reorder->ready = machPtr->cycleCount +
			    machPtr->mem_latency;
			StartOfExec(reorder,machPtr->iReorderBuffer);

			break;
		    }

		  case OP_LW:{
			reorder =
			    &machPtr->iReorderBuffer->element[load->reorderEntry];
			++machPtr->iReorderBuffer->checkOps;


			if (load->address & 0x3) {
			    result = AddressError(machPtr, 
					load->address, 1,
					load->pcAddress);
			    if (result != TCL_OK) {
				return STOP_SIMULATION;
			    } else {
				return END_OF_INSTRUCTION;
			    }
			}
			if (ReadMem(machPtr, load->address, &value,
				load->pcAddress) == 0) {
			    return STOP_SIMULATION;
			}
			reorder->result[0] = value;
			reorder->ready = machPtr->cycleCount +
			    machPtr->mem_latency;
			StartOfExec(reorder,machPtr->iReorderBuffer);
			break;
		    }

		    /* fp loads */

		  case OP_LD:
		    numData = 2;
		  case OP_LF:{
			int valueHi, valueLo;

			reorder =
			    &machPtr->fpReorderBuffer->element[load->reorderEntry];
			++machPtr->fpReorderBuffer->checkOps;

			valueLo = 0;
			if (load->address & 0x3) {
			    result = AddressError(machPtr, load->address,
						 1,load->pcAddress);
			    if (result != TCL_OK) {
				return STOP_SIMULATION;
			    } else {
				return END_OF_INSTRUCTION;
			    }
			}
			if (ReadMem(machPtr, load->address,
				 &valueHi,load->pcAddress) == 0) {
			    return STOP_SIMULATION;
			}
			if (numData == 2) {
			    if (ReadMem(machPtr, load->address + 4, &valueLo,
				load->pcAddress) == 0) {
				return STOP_SIMULATION;
			    }
			}
			reorder->result[0] = valueHi;
			reorder->result[1] = valueLo;
			reorder->ready = machPtr->cycleCount +
			    machPtr->mem_latency;
			StartOfExec(reorder,machPtr->fpReorderBuffer);

			break;
		    }
		}		/* end switch: enlever le load */

		RemoveData(machPtr->loadBuffer, load);

	    }			/* end if storeBypass just get the next load */
	} else
	    /* return STOP_ACCESS; */
	    break;
	load = loadTmp;
	/* the number of data accesses is limited */
	if (*numLoadAccess == machPtr->num_mem_access) {
	    return STOP_ACCESS;
	}
    }
    return ISSUED;

}
/*------------------------------------------------------------------------
 * SendStores --
 *      It traverses the store buffer, to check which store can be
 *      sent to memory.
 * Result: ok or error flag for the machine state
 * Side Effect:
 *      empties the store buffer by calling RemoveData. call WriteMem (in
 *      sim.c) to write to memory. call RemoveData to remove a flushed or
 *      sent store
 * Called by: Simulate in supersim.c
 *--------------------------------------------------------------------------
 */

int
SendStores(machPtr, numStore)
    DLX *machPtr;
    int numStore;		/* the number of stores that can be performed */
{
    DataOp *store;
    int numStoreAccess;

    numStoreAccess = 0;

    store = machPtr->storeBuffer->firstElement;
    /*
     * the number of store accesses is limited : furthermore, loads have been
     * sent on this clock cycle already
     */
    while ((store != NULL) && (numStoreAccess <= numStore)) {
	DataOp *storeTmp;
	storeTmp = store->prevPtr;

	if (store->flush) {
	    RemoveData(machPtr->storeBuffer, store);
	} else if (store->released) {
	    int numData;

	    numData = 1;

	    ++numStoreAccess;
	    numData = 1;
	    switch (store->opCode) {
		/* integer stores */
	      case OP_SB:{
		    if (WriteMem(machPtr, store->address, 1,
				 store->dataToStore->value[0],
				store->pcAddress) == 0)
			return STOP_SIMULATION;
		    break;
		}
	      case OP_SH:{
		    if (WriteMem(machPtr, store->address, 2,
				 store->dataToStore->value[0],
				 store->pcAddress) == 0)
			return STOP_SIMULATION;
		    break;
		}
	      case OP_SW:{
		    if (WriteMem(machPtr, store->address, 4,
				 store->dataToStore->value[0],
				store->pcAddress) == 0) {
			WriteMem(machPtr, store->address, 4,
				 store->dataToStore->value[0],
				store->pcAddress);
			return STOP_SIMULATION;

		    }
		    break;
		}
		/* floating point stores */
	      case OP_SD:
		numData = 2;
	      case OP_SF:{
		    if (WriteMem(machPtr, store->address, 4,
				 store->dataToStore->value[0],
				store->pcAddress) == 0)
			return STOP_SIMULATION;
		    if (numData == 2) {
			if (WriteMem(machPtr, store->address + 4, 4,
				     store->dataToStore->value[1],
				     store->pcAddress) == 0)
			    return STOP_SIMULATION;
		    }
		    break;
		}
	    }			/* end switch */
	    /* remove the store is it has been sent */
	    RemoveData(machPtr->storeBuffer, store);
	} else
	    return STOP_ACCESS;
	store = storeTmp;

    }				/* end while */

    return ISSUED;

}
