
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
 * superdecode.c --
 *
 * Functions for the decode stage of the superscalar simulator
 *
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "dlx.h"
#include "superdlx.h"

static char rcsid[] = "$Id: superdecode.c,v 1.15 93/04/14 00:43:35 cecile Exp $";
/* $Log:	superdecode.c,v $
 * Revision 1.15  93/04/14  00:43:35  cecile
 * *** empty log message ***
 * 
 * Revision 1.14  93/04/13  14:05:35  cecile
 * Supersimulator source file
 * 
 * Revision 1.13  93/04/09  19:41:33  cecile
 * *** empty log message ***
 * 
 * Revision 1.12  93/04/01  18:17:24  cecile
 * save version before trap changes
 * 
 * Revision 1.11  93/03/22  19:52:07  cecile
 * New processing for branch prediction: an entry is made for a decoded branch 
 * in each reorder buffer.
 * 
 * Revision 1.10  93/02/16  11:57:23  cecile
 * Collection of statistics: renaming and operand searching distribution
 * 
 * Revision 1.9  93/01/30  19:23:52  cecile
 * revised branch prediction: an alternative would be to have an entry for branches in both reorder buffers..
 * 
 * Revision 1.8  92/11/04  14:12:31  cecile
 * version without branch prediction
 * 
 * Revision 1.7  92/09/28  14:43:23  cecile
 * New structure for load and store buffers: only one load buffer and one store
 * buffer are used instead of one per operational unit (Integer and FP).
 * So now the load and store instructions are entered in the load or store
 * buffer during the decode stage instead of after the execute stage (as this 
 * was done before). So a link is created between a load/store operation
 * in the instruction window and its corresponding entry in the load or 
 * store buffer. The link between the reorder entry and the store/load buffer
 * is also created during decode.
 * 
 * Revision 1.6  92/08/27  14:28:11  cecile
 * Implementation of Branch Prediction
 * 
 * Revision 1.5  92/08/24  14:35:52  cecile
 * changes in the structure of the instruction window, which is now allocated 
 * a constant space: so no malloc is necessary. When an operation is entered
 * in the instruction window, a free slot is taken from the free list and
 * placed in the occupied list of the window.
 * 
 * Revision 1.4  92/08/16  12:50:42  cecile
 * Implementation of traps: they are entered in the integer window and
 * reorder buffer. The trap number is placed in the second operand treated
 * as immediate. The first operand is ignored (NONE). The result in the
 * reorder buffer will receive the trap #.
 * 
 * Revision 1.3  92/08/09  18:19:45  cecile
 * Implementation of the branch instruction. prediction is done at decode 
 * stage if it has failed at fetch
 * 
 * Revision 1.2  92/07/22  17:20:19  cecile
 * Implementation of the store and load buffers. Here it means that the 
 * data to store has to be searched (by SearchOperand) as the other
 * operands.
 * 
 * Revision 1.1  92/06/11  14:56:40  cecile
 * Initial revision
 * 
*/
static int SearchOperand();
static void EnterRBuffer();
static void EnterDBuffer();

int resultType;


/*-------------------------------------------------------------------
 *  SearchOperand  --       					
 *  	This procedure looks for the value of an operand:        	
 * 	 first it looks in the reorder buffer corresponding to the  
 *  	operand type to see if the operand is available there: 	
 *  	if it is available in one reorder buffer entry but has not  
 *  	been computed yet, the reorder buffer entry is taken in
 *	place of the operand value. If it has been computed, the     	
 * 	value is taken. Finally, if the operand is not available 	
 * 	in the reorder buffer, this means we get it from the         
 * 	register file                                  
 *
 * Result:
 *	A flag indicating if the operand is taken from the reorder
 *	buffer (instead of the register file)
 *
 * Called by :
 *	DecodeInstruction              
*------------------------------------------------------------------
*/

static int 
SearchOperand(machPtr,operand,registerNb)
   DLX *machPtr;
   Operand *operand;	/* operand to be searched 	*/
   int registerNb;	/* register number that 	*/
     			/* corresponds to the         	*/
			/* operand                      */
   
  
{
    int i,entry,found,found2,start;
   ReorderBuffer *buffer;
    ReorderElement *reorder, *reorderTail;
   int registerNb1 = registerNb;

    if (operand->type == INTEGER) {
	buffer = machPtr->iReorderBuffer;
     	registerNb1 = registerNb;
/*	++machPtr->counts->searchedOp[INTEGER];*/
    }
    else {
	buffer = machPtr->fpReorderBuffer;
/*	++machPtr->counts->searchedOp[FLOAT];*/
	if (registerNb != FP_STATUS) 
            registerNb1 = registerNb + 32;
    }

 
 /* the reorder buffer is selected according to the type of  */
 /* the operand                                              */

 
    found = 0;
    found2 = 0;
    operand->valid = 0;

    /* the reorder buffer is searched from tail to head		*/
    /* if the register number of the operand is found		*/
    /* there we must stop the search and check if the   	*/
    /* reorder buffer is valid or not: if it is valid, we take  */
    /* the result value, otherwise, we take the reorder    	*/
    /* buffer entry number   */

    reorderTail = &buffer->element[buffer->tail];
    if (registerNb1 != 0){
	for(i=0,reorder=reorderTail, entry = buffer->tail ;
		i<buffer->entryCount;
		i++,reorder--,entry--){
	    if ((reorder->flush == 0) && (reorder->type != NONE)) {
		 if (reorder->registerNb == registerNb) {
		    found = 1;
		    break;
	        } else if (reorder->type == FP_DOUBLE &&
			   reorder->registerNb == registerNb -1){
		    found2 = 1;
		    break;
		}
		
	    } 
	    if (entry == 0){
		reorder = &buffer->element[buffer->numEntries];
		entry = buffer->numEntries;
		continue;
	    }
	 }
    }
    if (found) {
	operand->valid = reorder->valid;	    
	if (operand->valid != 1){
            operand->value[0] = entry;
	    operand->value[1] = 0;
	} else{  
	    operand->value[0]=reorder->result[0];
	    if (operand->type == FP_DOUBLE)
	    	operand->value[1]=reorder->result[1];
	    else
		operand->value[1] = 0;
	}
	return found; 
     } else if (found2) {
	operand->valid = reorder->valid;
	if (operand->valid != 1){
	    operand->value[0] = entry;
	    operand->value[1] = -1;
	}else {
	    operand->value[0] = reorder->result[1];
	    operand->value[1] = 0;
	}
	return found2; 
   } else {
    /* If the register is not found in the reorder buffer, the  */
    /* value is taken from the register file   ; also the case */
    /* if the register is R0 (always 0)                        */
    	operand->value[0]=machPtr->regs[registerNb1];
     	operand->valid = 1;
        if (operand->type==FP_DOUBLE) 
            operand->value[1]=machPtr->regs[registerNb1+1];
	else 
	    operand->value[1]=0;  
     }
     return 0 ;
}

/*-----------------------------------------------------------------
 * DecodeInstruction  --					      	
 *    	This procedure takes an instruction which has been    	
 * 	extracted from the instruction queue, and prepares it      	
 * 	for entry in the instruction window: depending on the        
 * 	instruction classe, it searches for one or two operands ;    	
 * 	if it is a store, it searches for a third operand.                       *
 * Result:
 *	NONE  
 * Called by:
 *	Simulate in sim.c                                 
---------------------------------------------------------------------
*/


int
 DecodeInstruction(machPtr,instruction,rename,searched)
    DLX *machPtr;
 		 			/* will be placed in the */
    					 /* appropriate window  */
    InstructionElement *instruction; 
    int *rename;   
    int *searched;
{
    char opcode;
    int rd, real_rd;
    int found;
    int rs1,rs2;
    int opUnit;
    MemWord *wordPtr;
    WindowElement *operation; 
    WindowElement *newFreeList;
    InstructionWindow *window;
    ReorderBuffer *reorder;
    ReorderElement *otherRentry = NULL;
    

    rd = instruction->wordPtr->rd;
    rs1 = instruction->wordPtr->rs1;
    rs2 = instruction->wordPtr->rs2;


    opUnit = infOp[instruction->wordPtr->opCode].opUnit;
    resultType = infOp[instruction->wordPtr->opCode].result;

    if ( opUnit == INTEGER ) {
	window = machPtr->iWindow;
	reorder = machPtr->iReorderBuffer;
	real_rd = rd;
    } else {
	window = machPtr->fpWindow;
	reorder = machPtr->fpReorderBuffer;
	real_rd = rd + 32;
    }
   
    operation = window->freeList;

    operation->opCode = instruction->wordPtr->opCode;
    if (operation->opCode == OP_BFPT || operation->opCode == OP_BFPF){
	rs1 = FP_STATUS;
	}
    operation->classe = infOp[operation->opCode].classe;
    operation->unit = infOp[operation->opCode].funcUnit;

    operation->cycleDecode = machPtr->cycleCount;
    operation->address = instruction->address;
    operation->prediction = instruction->prediction;
    operation->bbEntry = instruction->bbEntry;
    operation->firstOperand->type = infOp[operation->opCode].firstOperand;
    operation->secondOperand->type = infOp[operation->opCode].secondOperand;
    operation->dataOp = NULL;
    /* the first operand is always to be searched: it always exists */
    /* and it cannot be an immediate*/
    if (operation->firstOperand->type != NONE){
	++searched[operation->firstOperand->type];
    	if ((found = SearchOperand(machPtr,operation->firstOperand,rs1)) == 1) {
	    ++*(rename + operation->firstOperand->type);
	   /* ++*(rename + ALL);*/
	}

    } else  /* ex traps */
	operation->firstOperand->valid = 1;

    switch(operation->classe){
    case TRAP:
	operation->secondOperand->value[0] = instruction->wordPtr->extra;
	operation->secondOperand->valid = 1;
	real_rd = rd = 1;
	
 	break;
	
    case BRANCH: {

	    operation->secondOperand->value[0] = 
			instruction->wordPtr->extra;
	    operation->secondOperand->valid = 1;
	    real_rd = rd = 0;
	    if (machPtr->branchPred) {
	    	EnterRBuffer(machPtr,  
			machPtr->fpReorderBuffer,
			operation,
			instruction,
			rd,
			real_rd,
			&otherRentry);
		operation->otherRentry = 
				machPtr->fpReorderBuffer->tail;
	    	otherRentry->type = NONE;
	    	otherRentry->unit = NO_UNIT;
	    }
         break;
     	}

    case FBRANCH:{
	    operation->secondOperand->value[0] = 
			instruction->wordPtr->extra;
	    operation->secondOperand->valid = 1;
	    real_rd = rd = 0;
	    if (machPtr->branchPred) {
	    	EnterRBuffer(machPtr,  
			machPtr->iReorderBuffer,
			operation,
			instruction,
			rd, 
			real_rd,
			&otherRentry);
			operation->otherRentry =
				machPtr->iReorderBuffer->tail;
	    	otherRentry->type = NONE;
	    	otherRentry->unit = NO_UNIT;
	    }
         break;
       }
    case INDIRECT_JUMP:
	
	if (operation->firstOperand->valid == 1) {
	    machPtr->stopFetch = 0;
	    machPtr->regs[PC_REG] = 
    		ADDR_TO_INDEX(operation->firstOperand->value[0]);
	    if (operation->opCode == OP_JALR)
		real_rd = rd = R31;
	    else if (operation->opCode == OP_JR)
	 	return(0);
	} else {    
	   machPtr->stopFetch = STOP_FETCH;
	   operation->secondOperand->valid = 1;
	
           if (operation->opCode == OP_JALR)
	    	real_rd = rd = R31;
	   else if (operation->opCode == OP_JR)
		real_rd = rd = 0;
	}

        break;
  
    case DIRECT_JUMP:
	/* only JAL left */
	real_rd = rd = R31;
	operation->secondOperand->valid = 1;
	break;
	 
    case STORE: 
    case FSTORE:
	operation->secondOperand->value[0] = instruction->wordPtr->extra;
	operation->secondOperand->valid = 1;
       	operation->dataOp = machPtr->storeBuffer->freeList;	
     	operation->dataOp->dataToStore->type = resultType;
	if ((found = SearchOperand(machPtr,
				operation->dataOp->dataToStore,rd)) == 1 ) {
			++*(rename + operation->dataOp->dataToStore->type);
	} 
	rd = real_rd = 0;
	++searched[operation->dataOp->dataToStore->type];
	EnterDBuffer(instruction,operation,machPtr->storeBuffer);
	break;
		
    case LOAD:
    case FLOAD:
	operation->secondOperand->value[0] = instruction->wordPtr->extra;
	operation->secondOperand->valid = 1;
	operation->dataOp = machPtr->loadBuffer->freeList;
	EnterDBuffer(instruction,operation,machPtr->loadBuffer);	
	++machPtr->counts->numLoads;
    	operation->dataOp->loadBlck = 0;
     	break;

    case FSET:
	rd = real_rd = FP_STATUS;
    default:
         switch(operation->secondOperand->type) {
	    case IMMEDIATE:
	       operation-> secondOperand->value[0]=instruction->wordPtr->extra;
	       operation->secondOperand->valid = 1;
	       break;
	    case NONE:
	       operation->secondOperand->value[0]=0;
	       operation->secondOperand->valid = 1;
	       break;
	    default:{
		++searched [operation->secondOperand->type];
		if (SearchOperand(machPtr,operation->secondOperand,
			instruction->wordPtr->rs2)) {
			++*(rename + operation->secondOperand->type);
			++*(rename +ALL);
		}
	       break;
	      }    
	  break;
  
         }
    }

  	
  EnterRBuffer(machPtr, reorder,operation,instruction,rd, real_rd,&otherRentry);
    operation->reorderEntry = reorder->tail;
    if (operation->dataOp != NULL)
    	operation->dataOp->reorderEntry = reorder->tail;
    window->entryCount++;
    newFreeList = window->freeList->nextPtr;
    if (newFreeList != NULL)
	newFreeList->prevPtr = NULL;

    /* entry in the instruction window */
    
    operation->nextPtr= window->lastElement;
    if (window->lastElement != NULL){
	   window->lastElement->prevPtr = operation;
	 }
	 
    operation->prevPtr = NULL;
    window->lastElement = operation;
    if (window->entryCount == 1)
	window->firstElement = operation;

    /* remove operation from free list*/
  
    window->freeList = newFreeList;
    return(0);

}

/*----------------------------------------------------------------------
 *
 * EnterRBuffer  -- 					        
 *  	This procedures enters an operation in  the appropriate	
 * 	reorder buffer
 * Results:
 *	NONE
 * Called by:
 *	DecodeInstruction                                 
 *---------------------------------------------------------------------
 */

static void
EnterRBuffer(machPtr,reorder,operation,instruction,rd, real_rd,otherRentry)
    DLX *machPtr;
    ReorderBuffer *reorder;
    WindowElement *operation;
    InstructionElement *instruction;
    int rd;
    int real_rd;
    ReorderElement **otherRentry;

{
    ReorderElement *reorderElement;
  
    /* the tail is incremented in a circular fashion but only if it */
    /* is not equal to the head with no elements in the reorder buffer */
 
   if (reorder->entryCount != 0) 
	if (++reorder->tail == reorder->numEntries)
		reorder->tail = 0;

  
    reorder->entryCount++;

     
    /* entry in the reorderbuffer*/
    reorderElement=&reorder->element[reorder->tail];
    reorderElement->opCode = operation->opCode;
    reorderElement->classe = operation->classe;
    reorderElement->unit = operation->unit;
    reorderElement->registerNb = rd;
    if (real_rd == 0) {
	reorderElement->type = NONE;
    } else {
    	reorderElement->type = resultType; 
   	++machPtr->counts->regWrites[real_rd];
    }

    reorderElement->insCount = instruction->insCount;
    reorderElement->cycleCount = instruction->cycleCount;
    reorderElement->dataOp = operation->dataOp;
    reorderElement->flush = 0;
    (ReorderElement *) reorderElement->otherReorder = *otherRentry;

    *otherRentry = reorderElement;

	
} 
/*----------------------------------------------------------------------
 *
 * EnterDBuffer  -- 					        
 *  	This procedures enters an operation in  the load or store buffer
 * Results:
 *	NONE
 * Called by:
 *	DecodeInstruction                                 
 *---------------------------------------------------------------------
 */

static void
EnterDBuffer(instruction,operation,dataBuffer)
InstructionElement *instruction;
WindowElement *operation;
DataBuffer *dataBuffer;

{
    DataOp *dataOp;
    DataOp *newFreeList;
    newFreeList = dataBuffer->freeList->nextPtr;
    if (newFreeList != NULL)
	newFreeList->prevPtr = NULL;

    dataOp = operation->dataOp;

    dataOp->opCode = operation->opCode;
	
    dataOp->insCount = instruction->insCount;
    dataOp->address = 0;
    dataOp->pcAddress = instruction->address;
    dataOp->released = 0;
    dataOp->flush = 0;


    dataOp->nextPtr = dataBuffer->lastElement;
		
    if (dataBuffer->lastElement != NULL) {
	    dataBuffer->lastElement->prevPtr = dataOp;
	    }
    dataOp->prevPtr = NULL;
    dataBuffer->lastElement = dataOp;
    ++dataBuffer->entryCount;
    if (dataBuffer->entryCount == 1)
	dataBuffer->firstElement = dataOp;

    dataBuffer->freeList = newFreeList;

}


	
    
  
