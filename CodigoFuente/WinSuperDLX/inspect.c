
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
 * inspect.c
 * 
 *         
 * Procedures that display the content of the different buffers and queues
 * of the machine. They are used by the non-X and X version of the simulator
 * 
 */

#include <stdlib.h>
/* #include <X11/Intrinsic.h> */

#include <stdio.h>
#include <string.h>
#include "dlx.h"
#include "superdlx.h"
#include "extern.h"
static char rcsid[] = "$Id: inspect.c,v 1.4 93/04/14 00:43:14 cecile Exp $";
   
/*------------------------------------------------------------------------- 
 *    displayFuncUnits --
 *   
 *   Displays the functional units information: number of functional units
 *   available, number of them which are used, latencies.
 *
 *   Results: NONE
 *   Called by: Sim_InspectCmd, in sim.c 
 *
 *-------------------------------------------------------------------------
*/

void
displayFuncUnits(dispUnits,funcUnits,numUnits,name, environment)
char **dispUnits;
FunctionalUnit *funcUnits;
int numUnits;
char *name[];
int environment;
{

  FunctionalUnit *fu;
  int i;
  char fp[100];

  *dispUnits = (char *)calloc(1,100*numUnits);

    for (fu = funcUnits, i=1; i<=numUnits ;i++, fu++){
      if (environment == NON_X){
	strcat(*dispUnits,"|");
      }


      sprintf(fp," %-10s   %5d      %5d      %5d    ",
		 name[i],
		 fu->num_units, fu->latency, fu->num_used);
      strcat(*dispUnits,fp);

      if (environment == NON_X){
	strcat(*dispUnits,"|");
      } 

      strcat(*dispUnits,"\n");
    }
}
/*-----------------------------------------------------------------------
 *   displayQueue --
 *
 *   display the content of the machine instruction queue.
 *
 *   Result: none
 *
 *   Called by: Sim_InspectCmd in sim.c
 *-----------------------------------------------------------------------
*/

void displayQueue(dispQueue,insQueue,environment) 
	char **dispQueue;
	InstructionQueue *insQueue;
	int environment;

{
    InstructionElement *instruction;
    unsigned int tmp;
    int insNum = insQueue->entryCount;
    char fp[100];

    *dispQueue = (char *)calloc(1,100*insQueue->numEntries);
    for (instruction= insQueue->lastElement; instruction!=NULL;
		instruction = instruction->nextPtr) {
	
      if (environment == NON_X){
	sprintf(fp,"|");
	strcat(*dispQueue,fp);
      }
    	tmp = INDEX_TO_ADDR(instruction->address);
	sprintf(fp,"%3d  %10d  %-10s  %-7s  %3d  %3d  %3d  %8d ",
		insNum,
		instruction->insCount,	
		Sym_GetString(machPtr,tmp),		
      		infOp[instruction->wordPtr->opCode].opName,
		instruction->wordPtr->rs1,
		instruction->wordPtr->rs2,
		instruction->wordPtr->rd,
		instruction->wordPtr->extra);
	insNum--;
	strcat(*dispQueue,fp);

      if (environment == NON_X){
	sprintf(fp,"|");
	strcat(*dispQueue,fp);
      }

      strcat(*dispQueue,"\n");
    }
  
}



/*-----------------------------------------------------------------------
 *  displayWindow --
 *	Display the content of the instruction window
 * 
 *   Result: 
 *	None
 *   Called By:
 *	Sim_InspectCmd in sim.c (non X version)
 *	IntegerWindowCallback, FloatWindowCallback in xinspect.c
 * 
 *----------------------------------------------------------------------
*/

 
void displayWindow(instWindow,environment)
     InstructionWindow *instWindow;
     int environment;


{

  WindowElement *operation;
  int instNum;
  unsigned int tmp;
  char fp[100];
  

  dispWindow = (char *)calloc(1,100*instWindow->numEntries);
  //dispWindow = (char *)malloc(100 * instWindow->numEntries);

    /* = fopen("window","wa");*/


  instNum = instWindow->entryCount;
  
  for (operation = instWindow->lastElement; operation != NULL;
  
       operation = operation->nextPtr){
    
    tmp = INDEX_TO_ADDR(operation->address);
      

    if (environment == NON_X) {
      strcat(dispWindow,"|");
    }

    sprintf(fp,"%3d  %-7s %-10.10s %9d ",
	   instNum,
	   infOp[operation->opCode].opName,
	   Sym_GetString(machPtr,tmp),
	    operation->reorderEntry);
    strcat(dispWindow,fp);
    
    if ((!operation->firstOperand->valid)||
	(operation->firstOperand->type == INTEGER) ||
	(operation->firstOperand->type == IMMEDIATE)){
      
      sprintf(fp,"%9d", operation->firstOperand->value[0]);
      
    } else if (operation->firstOperand->type == FP_SIMPLE){
	
      float *operand;
      
      operand = (float *) & (operation->firstOperand->value[0]);
       
      sprintf(fp,"%9.3e",*operand);

    } else if (operation->firstOperand->type == FP_DOUBLE) {
      double *operand;
      
      operand = (double *) & (operation->firstOperand->value[0]);
      sprintf(fp,"%9.3e",*operand);
      
    } else if (operation->firstOperand->type == NONE) {
	sprintf(fp,"         ");
    }

      strcat(dispWindow,fp);

      
    sprintf(fp," %3d  %3s ",  
	   operation->firstOperand->valid,
	   type[operation->firstOperand->type]);
    strcat(dispWindow,fp);

    if ((!operation->secondOperand->valid)||
	(operation->secondOperand->type == INTEGER)||
	(operation->secondOperand->type == IMMEDIATE)){
      
      sprintf(fp,"%9d", operation->secondOperand->value[0]);
      
    } else if (operation->secondOperand->type == FP_SIMPLE){
      
      float *operand;
      
      operand = (float *) & (operation->secondOperand->value[0]);
      
      sprintf(fp,"%9.3e",*operand);
     
    } else if (operation->secondOperand->type == FP_DOUBLE) {
      double *operand;
      
      operand = (double *) & (operation->secondOperand->value[0]);
      sprintf(fp,"%9.3e",*operand);
      
    } else if (operation->secondOperand->type == NONE) {
	sprintf(fp,"         ");

    }
    
          strcat(dispWindow,fp);

    sprintf(fp," %3d  %-3s",  
	   operation->secondOperand->valid,
	   type[operation->secondOperand->type]);
    strcat(dispWindow,fp);
    


    if ((operation->classe == BRANCH) || (operation->classe == FBRANCH)){
      char *pred;
      pred = "PNT";
      if (operation->prediction == TAKEN)
	pred = "PT";
      sprintf(fp," %4s ",pred);
      strcat(dispWindow,fp);
      
    } else {
	strcat(dispWindow,"   -  ");
    }
    if (environment == NON_X) {
      strcat(dispWindow,"|");
    }
    strcat(dispWindow,"\n");
    --instNum;
    
  }

 /* fclose(fp);*/
  strcat(dispWindow,"\0");  

}


/*-------------------------------------------------------------------------
 * diplayReorderBuffer --
 *
 *  	Formats and displays the content of the integer and floating point
 *	reorder buffer.
 *
 * Result:
 *	NONE
 * Called by:
 *	Sim_InspectCmd in sim.c (non-X version)
 *	IntegerBufferCallback, FloatBufferCallback in xinspect.c (X-Version)
 *--------------------------------------------------------------------------
*/



void
displayReorderBuffer(buffer,environment,funcUnit)
     ReorderBuffer *buffer;
     int environment;
    char **funcUnit;


{
  ReorderElement *reorder;
  int i,entry;
  char s[100];

  dispRBuffer = (char *)calloc(1,100*buffer->numEntries);
  
  sprintf(dispRBuffer,"\0"); 
  entry = buffer->head;
  for (i = 0; i< buffer->entryCount;i++){
    reorder =&buffer->element[entry];
    if (environment == NON_X){
      strcat(dispRBuffer,"|");
    }
    sprintf(s,"%3d  %-7s %9d %4d   %-10.10s",
	    entry,
	    infOp[reorder->opCode].opName,
	    reorder->insCount,
	    reorder->registerNb,
	    funcUnit[reorder->unit]);
    strcat(dispRBuffer,s);
    if (reorder->ready !=0){
      switch (reorder->type){
      case INTEGER:
	if (reorder->opCode == OP_TRAP) {
		if (reorder->valid == 2) {
			sprintf(s,"%10d ",reorder->result[1]);
			strcat(dispRBuffer,s);
			break;
		}
	}		
	sprintf(s,"%10d ",reorder->result[0]);
	strcat(dispRBuffer,s);
	break;
      case FP_SIMPLE:{
	float *result;
	result = (float *) & (reorder->result[0]);
	sprintf(s,"%10.3e ",*result);
	strcat(dispRBuffer,s);
	break;}
      case FP_DOUBLE:{
	double *result;
	result = (double *) & (reorder->result[0]);
	sprintf(s,"%10.3e ",*result);
	strcat(dispRBuffer,s);
	break;}
      default:
	sprintf(s,"     -     ");
	strcat(dispRBuffer,s);
	break;
      }
    }else {
      sprintf(s,"     -     ");
      strcat(dispRBuffer,s);
    }
	
    sprintf(s," %4d %10d  %4d ",
	    reorder->valid,
	    reorder->ready,
	    reorder->flush);
    strcat(dispRBuffer,s);
    if (environment == NON_X) {
      strcat(dispRBuffer,"|");
    }
    strcat(dispRBuffer,"\n");
    entry = (entry + 1)%(buffer->numEntries);
 }

  strcat(dispRBuffer,"\0");
}


/*------------------------------------------------------------------------
 *    displayDataBuffer --
 *
 *   	Formats and displays the content of the integer and floating point*
 *	store and load buffers.
 *
 *  Result: NONE
 *  Called by:
 *	Sim_InspectCmd in sim.c (non-X version)
 *	IntegerBufferCallback, FloatBufferCallback in xinspect.c (X-Version)
 *-------------------------------------------------------------------------
*/


void 
displayDataBuffer(dataBuffer,dataBufferType,environment)
     DataBuffer *dataBuffer;
     int dataBufferType;
     int environment;

{
  DataOp *data;
  int entry;
  char s[2000];
  unsigned int tmp;

  free(dispData);

  dispData = (char *)calloc(1,100*dataBuffer->numEntries);

  entry = dataBuffer->entryCount;
  
  for (data = dataBuffer->lastElement; data != NULL; data = data->nextPtr){

    if (environment == NON_X) {
	  strcat(dispData,"|");
    }

    sprintf(s,"%4d %6s %10d ",
	    entry,
	    infOp[data->opCode].opName,
	    data->insCount);

    strcat(dispData,s);


    if (data->address){     
      sprintf(s,"  %7x ",data->address);
      strcat(dispData,s);
    }
     else {
      strcat(dispData,"       0  ");
    }

    if (dataBufferType == STORE_BUFFER){
      if (data->dataToStore->valid){
	switch (data->dataToStore->type){
	case INTEGER:
	  sprintf(s,"%14d ",data->dataToStore->value[0]);
	  strcat(dispData,s);
	  break;
	case FP_SIMPLE:{
	  float *result;
	  result = (float *) & (data->dataToStore->value[0]);
	  sprintf(s,"%14.3f ",*result);
	  strcat(dispData,s);
	  break;}
	case FP_DOUBLE:{
	  double *result;
	  result = (double *) & (data->dataToStore->value[0]);
	  
	  sprintf(s,"%14.3f ",*result);
	  strcat(dispData,s);

	  break;}
	default:
	
	  break;
	} 
  
     } else {
	  sprintf(s,"%14d ",data->dataToStore->value[0]);
	  strcat(dispData,s);
	
	/*strcat(dispData,"         -         -   -");*/
     }
	
	sprintf(s," %4s %2d",
		type[data->dataToStore->type],
		data->dataToStore->valid); 
	strcat(dispData,s);


    }
   
    sprintf(s,"%4d  %4d ", data->released,data->flush);
    
    strcat(dispData,s);

    if (environment == NON_X) {
      strcat(dispData,"|");
    }

    strcat(dispData,"\n");

    --entry;
    

  }
    
}
/*-------------------------------------------------------------------------
 *   displayDistrib --
 *
 *	formats and displays distribution tables requested by the user (with
 *	the stats command)
 *   
 * Result:
 *	NONE
 * Called by:
 *	Sim_DumpStats in sim.c
 *--------------------------------------------------------------------------
*/

void displayDistrib(fp,machPtr, table, bound, max, base,floatYes)
    FILE *fp;
    DLX *machPtr;
    int *table;
    float bound;
    int max;
    int base;
    int floatYes;
{
    int i , numAll = 0, numInt = 0, numFp = 0;
    fprintf(fp," _____________________________________________________________________________ \n");
    fprintf(fp,"|     |      BOTH  UNITS      |      INTEGER UNIT     |  FLOATING POINT UNIT  |\n"); 
    fprintf(fp,"| NUM |     total    |    %%   |     total    |    %%   |     total    |    %%   |\n");  
    fprintf(fp,"|-----|--------------|--------|--------------|--------|--------------|--------|\n");

    for (i = 0; i <max; i++) {
	numAll = *(table + max * ALL +i);
	numInt = *(table + max * INTEGER + i);
	if (floatYes)
	    numFp = *(table + max * FLOAT + i);
       if (numAll > bound || numInt > bound || numFp > bound ) {	
	   fprintf(fp,"| %3d | %12d | %5.2f%% | %12d | %5.2f%% | %12d | %5.2f%% |\n", 
			i, numAll, 
			100.0 * numAll / base,
			numInt,
			100.0 * numInt / base,		
			numFp,
			100.0 * numFp / base); 
	}
    }
    fprintf(fp,"!-----!--------------!--------!--------------!--------!--------------!--------!\n");

}

/*--------------------------------------------------------------------------
 *
 *   displayBufferOcc --
 *
 *   	format and display the buffer (instruction and reorder buffer) 
 *	occupancy tables.
 *
 * Result:
 * 	 NONE
 * Called by: 
 *	Sim_DumpStats in sim.c
 *--------------------------------------------------------------------------
*/

void displayBufferOcc(fp,machPtr,table, bound, max)
    FILE *fp;
    DLX *machPtr;
    int *table;
    float bound;
    int max;
    
{
    int i;

    int numReorder=0, numWindow = 0;
    fprintf(fp," ________________________________________________________\n");
    fprintf(fp,"|      |   INSTRUCTION WINDOW   |     REORDER BUFFER     |\n");
    fprintf(fp,"| %%OCC |   total cc   |    %%cc  |    total cc  |   %%cc   |\n");
    fprintf(fp,"|------|--------------|---------|--------------|---------|\n");

    for (i = 0; i < max ; i++) {

        numReorder = *(table + max*REORDER_BUF + i);
    	numWindow = *(table + max *INST_WINDOW + i);

       if (numReorder> bound || numWindow > bound) {	
	    if (i < max - 1) {
	    	fprintf(fp,"| +%2d%% | %12d | %6.2f%% | %12d | %6.2f%% |\n",
		i * 10, 
		numWindow,
		100.0 * numWindow / machPtr->cycleCount,
		numReorder,
		100.0 * numReorder / machPtr->cycleCount);
	    } else {
	    	fprintf(fp,"| %3d%% | %12d | %6.2f%% | %12d | %6.2f%% |\n",
		i * 10, 
		numWindow,
		100.0 * numWindow / machPtr->cycleCount,
		numReorder,
		100.0 * numReorder / machPtr->cycleCount);
	    }
	}
    }
    fprintf(fp,"!------!--------------!---------!--------------!---------!\n");

}



