

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dlx.h"
#include "superdlx.h"

static char rcsid[] = "$Id: supertrace.c,v 1.6 92/11/04 14:14:52 cecile Exp Locker: cecile $";
/* $Log:	supertrace.c,v $
 * Revision 1.6  92/11/04  14:14:52  cecile
 * version without branch prediction
 * 
 * Revision 1.5  92/09/28  14:55:01  cecile
 * New load/store buffer structures
 * 
 * Revision 1.4  92/08/27  14:30:49  cecile
 * Implementation of the branch target buffer
 * 
 * Revision 1.3  92/08/24  14:40:21  cecile
 * Changes in the structure of the instruction window
 * 
 * Revision 1.2  92/07/22  17:22:47  cecile
 * Display of load and store buffers
 * 
 * Revision 1.1  92/06/11  15:01:37  cecile
 * Initial revision
 * 
*/

void
DisplayBB(branchBuffer)
BranchInfo *branchBuffer;

{
    int i;
    BranchInfo *branch;

    for (i=0, branch = branchBuffer;i<NUM_BB_ENTRIES;i++, branch++) {
	if (branch->address != 0) {
	    printf("Branch Entry: %d address : %d prediction : %d predicted target: %d \n",
		i, branch->address, branch->predictState,
		branch->predictTarget);
  	}
   }
}

void
FDisplayBB(branchBuffer,fp)
BranchInfo *branchBuffer;
FILE * fp;

{
    int i;
    BranchInfo *branch;

    for (i=0, branch = branchBuffer;i<NUM_BB_ENTRIES;i++, branch++) {
	   if (branch->address != 0) {
	    fprintf(fp,"Branch Entry: %d address : %d prediction : %d predicted target: %d \n",
		         i, branch->address, branch->predictState,branch->predictTarget);
  	   }
   }
}
   
void
DisplayUnits(funcUnits,numUnits,name)
FunctionalUnit *funcUnits;
int numUnits;
char *name[];

{

FunctionalUnit *fu;
int i;

    for (fu = funcUnits, i=0; i<numUnits ;i++, fu++){
	printf(" * FU :	%s 	lat: %d 	Num: %d 	Used: %d\n",
		 name[i],
		 fu->latency, fu->num_units, fu->num_used);
    }
}

void
FDisplayUnits(funcUnits,numUnits,name,fp)
FunctionalUnit *funcUnits;
int numUnits;
char *name[];
FILE * fp;

{
FunctionalUnit *fu;
int i;

    for (fu = funcUnits, i=0; i<numUnits ;i++, fu++){
	fprintf(fp," * FU : %s	lat: %d 	Num: %d 	Used: %d\n", name[i],
		 fu->latency, fu->num_units, fu->num_used);
    }
}
        
void
DisplayDataBuffer(dataBuffer)
DataBuffer *dataBuffer;

{
    DataOp *data;
    
    printf("Max Elements : %d \n", dataBuffer->numEntries);
       
    printf ("Num Elements : %d \n", dataBuffer->entryCount);
      
    for (data = dataBuffer->lastElement; data != NULL; data = data->nextPtr){
	printf(
	" * InsCount:%d	Op: %s	Addr: %d	Rel: %d 	R_entry: %d",
		data->insCount, 
		infOp[data->opCode].opName, 
		data->address,
		data->released, 
		data->reorderEntry);
	if ((infOp[data->opCode].classe == STORE) || 
		(infOp[data->opCode].classe == FSTORE)) {
	    printf(
		 "	-> Data: %d : %d 	Validity: %d\n" , 
			data->dataToStore->value[0],
			data->dataToStore->value[1], 
			data->dataToStore->valid);

	}
	
    }
    
}

         
void
fDisplayDataBuffer(fp,dataBuffer)
FILE *fp;
DataBuffer *dataBuffer;

{
    DataOp *data;
    
  /*  fprintf(fp,"Max Elements : %d \n", dataBuffer->numEntries);
       
    fprintf (fp,"Num Elements : %d \n", dataBuffer->entryCount);*/
      
    for (data = dataBuffer->lastElement; data != NULL; data = data->nextPtr){
	fprintf(fp," * InsCount:%d	Op: %s	Addr: %d	Rel: %d 	R_entry: %d",
		data->insCount, infOp[data->opCode].opName, data->address,
		 data->released, data->reorderEntry);
	if ((infOp[data->opCode].classe == STORE) || 
		(infOp[data->opCode].classe == FSTORE)) {
	    fprintf(fp,"	-> Data: %d : %d 	Validity: %d\n" , data->dataToStore->value[0],
				   data->dataToStore->value[1], data->dataToStore->valid);

	}
	
    }
    
}


		
 


 void 
Affichage(fp,machPtr)
    FILE *fp;
    DLX *machPtr;
 
{
    InstructionElement *instruction;
    WindowElement *operation;
    DataOp *data;
    int i,entry;
   /*printf("\n\n  ********** Nouveau Cycle:%d *************\n", machPtr->cycleCount);*/
   
    /*printf*/
/*
    printf("\n Affichage\n");
*/ 
    printf("\n\n ---> clock cycle %d\n", machPtr->cycleCount);

    printf("\n ** Instruction Queue:\n");
    printf("Num Elements : %d \n", machPtr->insQueue->entryCount);
    for (instruction=machPtr->insQueue->lastElement; instruction!=NULL;  
                        instruction=instruction->nextPtr) {
	unsigned int tmp;
	char *instrStr;
	tmp = INDEX_TO_ADDR(instruction->address);
     	printf("Instruction %d pc = %.100s opcode %s :	rs1: %d rs2: %d rd: %d extra: %d\n", 
		instruction->insCount,	
		Sym_GetString(machPtr,tmp),		
      		infOp[instruction->wordPtr->opCode].opName,
		instruction->wordPtr->rs1,
		instruction->wordPtr->rs2,
		instruction->wordPtr->rd,
		instruction->wordPtr->extra);
    }
/*
    printf("\n ** Integer Instruction Window:\n");
 
  
    printf(" Num elements: %d\n", machPtr->iWindow->entryCount);
 
    for (operation = machPtr->iWindow->lastElement; operation!=NULL; operation =operation->nextPtr){

    	printf("Operation no %d opcode %s:\n" , operation->insCount,  
              infOp[operation->opCode].opName);

     	printf("  Reorder Type: %d  Reorder Entry: %d\n", 
				 operation->reorderType,
				 operation->reorderEntry);
     	printf("  First Operand:	value: %d : %d 	type: %s 	valid: %d\n",
					operation->firstOperand->value[0],
                		    	operation->firstOperand->value[1],
					type[operation->firstOperand->type],
					 operation->firstOperand->valid);
     			  
     	printf("  Second Operand: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->secondOperand->value[0],
                		    	operation->secondOperand->value[1],
				 	type[operation->secondOperand->type],
					operation->secondOperand->valid);
       if (operation->classe == STORE)
 	    printf("  Data to Store: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->dataOp->dataToStore->value[0],
                		    	operation->dataOp->dataToStore->value[1],
				 	type[operation->dataOp->dataToStore->type],
					operation->dataOp->dataToStore->valid);
    }
   
     printf("\n ** FP Instruction Window:\n");
    printf(" Num elements: %d\n", machPtr->fpWindow->entryCount);
    for (operation = machPtr->fpWindow->lastElement; operation!=NULL;
              operation =operation->nextPtr){
    	printf("Operation no %d opcode %s:\n" , operation->insCount,  
              infOp[operation->opCode].opName);

     	printf("  Reorder Type: %d  Reorder Entry: %d\n", 
				 operation->reorderType,
				 operation->reorderEntry);
     	printf("  First Operand:	value: %d : %d 	type: %s 	valid: %d\n",
					operation->firstOperand->value[0],
                		    	operation->firstOperand->value[1],
					type[operation->firstOperand->type],
					 operation->firstOperand->valid);
     			  
     	printf("  Second Operand: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->secondOperand->value[0],
                		    	operation->secondOperand->value[1],
				 	type[operation->secondOperand->type],
					operation->secondOperand->valid);
       if (operation->classe == FSTORE)
 	    printf("  Data to Store: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->dataOp->dataToStore->value[0],
                		    	operation->dataOp->dataToStore->value[1],
				 	type[operation->dataOp->dataToStore->type],
					operation->dataOp->dataToStore->valid);
     }
   
    printf("\n ** Integer Reorder Buffer:\n");
    printf("  Tail:%d Head:%d Num elements: %d \n", 
                   machPtr->iReorderBuffer->tail,
                   machPtr->iReorderBuffer->head,
		    machPtr->iReorderBuffer->entryCount);
    entry = machPtr->iReorderBuffer->head;
    for (i=0;i < machPtr->iReorderBuffer->entryCount; i++){
      	printf("* Entry number: %d Instruction %d opCode %s\n",
	         entry,
	         machPtr->iReorderBuffer->element[entry].insCount,
		 infOp[machPtr->iReorderBuffer->element[entry].opCode].opName);
      	printf("    Register Number: %d type:%s Validity: %d Ready: %d Unit: %s Flush:%d\n",
             machPtr->iReorderBuffer->element[entry].registerNb,
	     type[machPtr->iReorderBuffer->element[entry].type],
	     machPtr->iReorderBuffer->element[entry].valid,
	     machPtr->iReorderBuffer->element[entry].ready,
	     intUnit[machPtr->iReorderBuffer->element[entry].unit],
	     machPtr->iReorderBuffer->element[entry].flush);
      	printf("    Result %d : %d \n", 
             machPtr->iReorderBuffer->element[entry].result[0],
             machPtr->iReorderBuffer->element[entry].result[1]);
       	entry =(entry + 1)%(machPtr->iReorderBuffer->numEntries);
   }        
       
    printf("\n ** FP Reorder Buffer:\n");
    printf("  Tail:%d Head:%d Num elements: %d \n", 
                   machPtr->fpReorderBuffer->tail,
                   machPtr->fpReorderBuffer->head, 
		   machPtr->fpReorderBuffer->entryCount);
    entry = machPtr->fpReorderBuffer->head;
    for (i=0;i < machPtr->fpReorderBuffer->entryCount; i++){
     	printf("* Entry number: %d Instruction %d opCode %s \n",entry, 
	        machPtr->fpReorderBuffer->element[entry].insCount,
		 infOp[machPtr->fpReorderBuffer->element[entry].opCode].opName);
      	printf("    Register Number: %d type:%s Validity: %d Ready: %d Unit: %s Flush:%d\n",
             machPtr->fpReorderBuffer->element[entry].registerNb,
	     type[machPtr->fpReorderBuffer->element[entry].type],
	     machPtr->fpReorderBuffer->element[entry].valid,
	     machPtr->fpReorderBuffer->element[entry].ready,
	     fpUnit[machPtr->fpReorderBuffer->element[entry].unit],
	     machPtr->fpReorderBuffer->element[entry].flush);
      	printf("    Result %d : %d \n",
             machPtr->fpReorderBuffer->element[entry].result[0],
             machPtr->fpReorderBuffer->element[entry].result[1]);
      	entry =(entry + 1)%(machPtr->fpReorderBuffer->numEntries);
    }
     
    printf("\n Load Buffer \n");
    DisplayDataBuffer(machPtr->loadBuffer);
    printf("\n Store Buffer\n");
    DisplayDataBuffer(machPtr->storeBuffer);
   
    printf("\n Integer Units \n");
    DisplayUnits(&machPtr->intUnits[0], 6, &intUnit[0]);
    printf("\n FP Units \n");
    DisplayUnits(&machPtr->fpUnits[0], 8, &fpUnit[0]);
*/

 /*   printf("\n Branch Target Buffer \n");
 /*
   DisplayBB(machPtr->branchBuffer);*/


    /* end printf*/

/*
fprintf(fp,"\n\n  ********** Nouveau Cycle:%d *************\n", machPtr->cycleCount);

    fprintf(fp,"\n ** Instruction Queue:\n");
    fprintf(fp,"Num Elements : %d \n", machPtr->insQueue->entryCount);
    for (instruction=machPtr->insQueue->lastElement; instruction!=NULL;  
                        instruction=instruction->nextPtr) {
   	fprintf(fp, "Instruction %d 	opcode %s :	rs1: %d rs2: %d rd: %d extra: %d\n", 
		instruction->insCount,			
      		infOp[instruction->wordPtr->opCode].opName,
		instruction->wordPtr->rs1,
		instruction->wordPtr->rs2,
		instruction->wordPtr->rd,
		instruction->wordPtr->extra);
  
  }
*/
/*     
    fprintf(fp,"\n ** Integer Instruction Window:\n");
 
  
    fprintf(fp," Num elements: %d\n", machPtr->iWindow->entryCount);
 
    for (operation = machPtr->iWindow->lastElement; operation!=NULL; operation =operation->nextPtr){
    	fprintf(fp,"Operation no %d opcode %s:\n" , operation->insCount,
              infOp[operation->opCode].opName);
     	fprintf(fp,"  Reorder Type: %d  Reorder Entry: %d\n", 
				 operation->reorderType,
				 operation->reorderEntry);
     	fprintf(fp,"  First Operand:	value: %d : %d 	type: %s 	valid: %d\n",
					operation->firstOperand->value[0],
                		    	operation->firstOperand->value[1],
					type[operation->firstOperand->type],
					 operation->firstOperand->valid);
     			  
     	fprintf(fp,"  Second Operand: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->secondOperand->value[0],
                		    	operation->secondOperand->value[1],
				 	type[operation->secondOperand->type],
					operation->secondOperand->valid);
	if (operation->classe == STORE)
     	    fprintf(fp,"  Data: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->dataOp->dataToStore->value[0],
                		    	operation->dataOp->dataToStore->value[1],
				 	type[operation->dataOp->dataToStore->type],
					operation->dataOp->dataToStore->valid);
     }
   
     fprintf(fp,"\n ** FP Instruction Window:\n");
    fprintf(fp," Num elements: %d\n", machPtr->fpWindow->entryCount);
    for (operation = machPtr->fpWindow->lastElement; operation!=NULL;
              operation =operation->nextPtr){
    	fprintf(fp,"Operation no %d opcode %s:\n" , operation->insCount,  
              infOp[operation->opCode].opName);

     	fprintf(fp,"  Reorder Type: %d  Reorder Entry: %d\n", 
				 operation->reorderType,
				 operation->reorderEntry);
     	fprintf(fp,"  First Operand:	value: %d : %d 	type: %s 	valid: %d\n",
					operation->firstOperand->value[0],
                		    	operation->firstOperand->value[1],
					type[operation->firstOperand->type],
					 operation->firstOperand->valid);
     			  
     	fprintf(fp,"  Second Operand: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->secondOperand->value[0],
                		    	operation->secondOperand->value[1],
				 	type[operation->secondOperand->type],
					operation->secondOperand->valid);
	if (operation->classe == FSTORE)
     	    fprintf(fp,"  Data: 	value: %d : %d 	type: %s 	valid %d \n",
					operation->dataOp->dataToStore->value[0],
                		    	operation->dataOp->dataToStore->value[1],
				 	type[operation->dataOp->dataToStore->type],
					operation->dataOp->dataToStore->valid);
	    
    }
   
    fprintf(fp,"\n ** Integer Reorder Buffer:\n");
    fprintf(fp,"  Tail:%d Head:%d Num elements: %d \n", 
                   machPtr->iReorderBuffer->tail,
                   machPtr->iReorderBuffer->head,
		    machPtr->iReorderBuffer->entryCount);
    entry = machPtr->iReorderBuffer->head;
    for (i=0;i < machPtr->iReorderBuffer->entryCount; i++){
      	fprintf(fp,"* Entry number: %d Instruction %d Opcode %s\n",
	         entry,
	         machPtr->iReorderBuffer->element[entry].insCount,
		 infOp[machPtr->iReorderBuffer->element[entry].opCode].opName);
 
      	fprintf(fp,"    Register Number: %d type:%s Validity: %d Ready: %d Unit: %s Flush:%d\n",
             machPtr->iReorderBuffer->element[entry].registerNb,
	     type[machPtr->iReorderBuffer->element[entry].type],
	     machPtr->iReorderBuffer->element[entry].valid,
	     machPtr->iReorderBuffer->element[entry].ready,
	     intUnit[machPtr->iReorderBuffer->element[entry].unit],
	     machPtr->iReorderBuffer->element[entry].flush);
      	fprintf(fp,"    Result %d : %d \n", 
             machPtr->iReorderBuffer->element[entry].result[0],
             machPtr->iReorderBuffer->element[entry].result[1]);
       	entry =(entry + 1)%(machPtr->iReorderBuffer->numEntries);
   }        
       
    fprintf(fp,"\n ** FP Reorder Buffer:\n");
    fprintf(fp,"  Tail:%d Head:%d Num elements: %d \n",
                   machPtr->fpReorderBuffer->tail,
                   machPtr->fpReorderBuffer->head, 
		   machPtr->fpReorderBuffer->entryCount);
    entry = machPtr->fpReorderBuffer->head;
    for (i=0;i < machPtr->fpReorderBuffer->entryCount; i++){
     	fprintf(fp,"* Entry number: %d Instruction %d Opcode %s\n",
	        entry, 
	        machPtr->fpReorderBuffer->element[entry].insCount,
		infOp[machPtr->fpReorderBuffer->element[entry].opCode].opName);
      	fprintf(fp,"    Register Number: %d type:%s Validity: %d Ready: %d Unit: %s Flush:%d\n",
             machPtr->fpReorderBuffer->element[entry].registerNb,
	     type[machPtr->fpReorderBuffer->element[entry].type],
	     machPtr->fpReorderBuffer->element[entry].valid,
	     machPtr->fpReorderBuffer->element[entry].ready,
	     fpUnit[machPtr->fpReorderBuffer->element[entry].unit],
	     machPtr->fpReorderBuffer->element[entry].flush);
      	fprintf(fp,"    Result %d : %d \n", 
             machPtr->fpReorderBuffer->element[entry].result[0],
             machPtr->fpReorderBuffer->element[entry].result[1]);
      	entry =(entry + 1)%(machPtr->fpReorderBuffer->numEntries);
     }
   
    fprintf(fp,"\n Load Buffer \n");
    fDisplayDataBuffer(fp,machPtr->loadBuffer);
    fprintf(fp,"\nStore Buffer\n");
    fDisplayDataBuffer(fp,machPtr->storeBuffer);

    fprintf(fp,"\n Integer Units \n");
    FDisplayUnits(&machPtr->intUnits[0], 6, &intUnit[0]);
    fprintf(fp,"\n FP Units \n");
    FDisplayUnits(&machPtr->fpUnits[0], 8, &fpUnit[0]);
    fprintf(fp,"\n Branch Target Buffer \n");
    FDisplayBB(machPtr->branchBuffer);
*/

 }

 void
 MachineState(machPtr,fp)
     DLX *machPtr;
     FILE *fp;

{
    int i;
    register MemWord *wordPtr;

    fprintf(fp,"\n        ******************* Registers **************************\n");
    for (i = 0; i < NUM_GPRS; i++) {
	if (machPtr->regs[i] != 0) {
        if (i<32)
	    fprintf(fp,"Register R%d : %d\n",i,machPtr->regs[i]);
	if ((i>=32)&&(i< 64))
	    fprintf(fp,"Register F%d  : %d\n",i - 32,machPtr->regs[i]);
	if (i>=64)
	   fprintf(fp,"Register no %d  : %d\n",i,machPtr->regs[i]);}
    }

    fprintf (fp,"\n       ******************** Memory ***************************\n");
    for (i = machPtr->numWords, wordPtr = machPtr->memPtr; i > 0;
	    i--, wordPtr++) {
	if (wordPtr->value != 0)
	    fprintf(fp,"Memory no %d : %d\n",i,wordPtr->value);
    }
    fclose(fp);
}




