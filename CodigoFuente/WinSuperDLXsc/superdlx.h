/* 
Este fichero es parte de WinSuperDLX, el simulador interactivo de la
    máquina DLX 

    Copyright (C) 2002 Mario David Barragán García (jabade@supercable.com)
                 Antonio Jesús Contreras Sánchez   (anjeconsa@yahoo.es)
                Emilio José Garnica López   (ejgl@hotmail.com)
                Julio Ortega Lopera         (julio@atc.ugr.es)

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.

*/
// A�adido
#ifndef _SUPERDLX_H
#define _SUPERDLX_H 1

#include "dlx.h"
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
 * superdlx.h --
 *
 * Constants and default values for the superscalar machine parameters
 * Declaration of the exported functions of the simulator (non-X version)
 * 
 */

/*
 * $Id: superdlx.h,v 1.13 93/04/26 20:46:15 cecile Exp $ $Log:
 * superdlx.h,v $ Revision 1.7  93/02/08  12:07:58  cecile statistics and
 * display functions declarations
 * 
 * Revision 1.6  93/02/01  10:02:19  cecile new flags added for the machine
 * state.
 * 
 * Revision 1.5  92/08/26  10:22:07  cecile New data structure for instruction
 * queue
 * 
 * Revision 1.4  92/08/16  12:46:49  cecile macros have been added for the
 * execution of traps
 * 
 * Revision 1.3  92/08/09  18:34:57  cecile implementation of the functional
 * units parameters of the FUs have been transferred from dlx.h to here
 * 
 * Revision 1.2  92/06/11  15:02:44  cecile defines have been transfered here
 * from supersim.c: to be used by the other super* files
 * 
 * Revision 1.1  92/06/07  16:42:08  cecile Initial revision
 * 
 */

/* machine flags */
#define CYCLE_PROCESS 0
#define END_OF_STEP 1
#define END_OF_SIMULATION 2
#define ISSUED 3
#define NOT_ISSUED 4
#define STOP_SIMULATION 5
#define STOP_ACCESS 6
#define END_OF_INSTRUCTION 7
#define ERROR 8
#define FLUSHED 9

#define FETCH 0
#define STOP_FETCH 1
#define NEXT_CYCLE_FETCH 2

/* two bit branch prediction scheme */
#define NOT_TAKEN 0
#define NOT_TAKEN_ 1
#define TAKEN_ 2
#define TAKEN 3

#define X 0
#define NON_X 1

#define INSTRUCTION 0
#define CYCLE 1

/* operational units & operand types */
#define ALL 0
#define INTEGER 1
#define FLOAT  2
#define OTHER 3

#define NONE 0
#define FP_SIMPLE 2
#define FP_DOUBLE 3
#define IMMEDIATE 4
#define FSPECIAL 5

/* classees of instructions */

#define ALU 1
#define SHIFT 2
#define LOAD 3
#define STORE 4
#define IMEM 5
#define BRANCH 6
#define FADD 7
#define FCONV 8
#define FMULT 9
#define FDIV 10
#define FLOAD 11
#define FSTORE 12
#define FMEM 13
#define FBRANCH 14
#define SET 15
#define MOVE 16
#define FSET 17
#define FMOVE 18
#define DIRECT_JUMP 19
#define INDIRECT_JUMP 20
#define TRAP 21

/* integer functional units */
#define NO_UNIT 0
#define INT_ALU 1
#define INT_SHIFT 2
#define INT_DATA 3
#define INT_COMP 4
#define INT_BRANCH 5
#define INT_MEM 6


/* floating point functional units */

#define FP_ADD 1
#define FP_DIV 2
#define FP_MUL 3
#define FP_CONV 4
#define FP_DATA 5
#define FP_COMP 6
#define FP_BRANCH 7
#define FP_MEM 8

/* dans opInfo, mettre ca et changer dans decode: */
/* define FLOAD, LOAD, STORE, FSTORE */
#define LOAD_BUFFER 0
#define STORE_BUFFER 1

/* default values for number of functional units and latencies */

#define NUM_INT_UNITS 5/* number of "real" integer functional unit types, without counting INT_MEM and NONE*/
#define NUM_FP_UNITS 7 /* number of "real" fp functional unit types*/
#define NUM_FADD_UNITS 4	/* Number of fp adder units. */
#define NUM_FDIV_UNITS 2	/* Number of fp divider units. */
#define NUM_FMUL_UNITS 2	/* Number of fp multiplier units. */
#define NUM_FCONV_UNITS 2	/* Number of convert units (useful for */
#define NUM_FCOMP_UNITS 1	/* superscalar dlx). */

#define NUM_FDATA_UNITS 4
#define NUM_FBRANCH_UNITS 1

#define FP_ADD_LATENCY 2	/* Latency for an FP add. */
#define FP_DIV_LATENCY 10	/* Latency for an FP divide. */
#define FP_MUL_LATENCY 5	/* Latency for an FP multiply. */
#define FP_CONV_LATENCY 2
#define FP_COMP_LATENCY 1

#define INT_ALU_LATENCY 1
#define INT_SHIFT_LATENCY 1
#define MEM_LATENCY 1
#define INT_COMP_LATENCY 1

#define NUM_ALU_UNITS 4		/* Number of integer ALU units */
#define NUM_SHIFT_UNITS 2	/* Number of shift integer units */
#define NUM_DATA_UNITS 4	/* address calculation units */
#define NUM_BRANCH_UNITS 1
#define NUM_COMP_UNITS 1

#define MAX_NUM_UNITS 20	/* max number of functional units of a given
				 * type */
/*
 * Superscalar dlx parameters (default values) for the added queues and
 * buffers
 */

#define NUM_FETCH 4		/* Number of instructions fetched on 1 cc */
#define NUM_DECODE 2		/* Number of instructions decoded on 1 cc */
#define NUM_COMMIT 4
#define NUM_INS_QUEUE 17	/* Number of instruction queue entries	  */

#define NUM_I_WINDOW 20		/* Number of entries in the integer
				 * instruction window */

#define NUM_FP_WINDOW 20	/* idem for the FP instruction window */

#define NUM_I_REORDER 20	/* Number of entries in the integer reorder
				 * buffer */
#define NUM_FP_REORDER 20	/* Number of entries in the floating point
				 * reorder buffer */
#define NUM_STORE_BUFF	5	/* Number of entries in the  store
				 * buffer */
#define NUM_LOAD_BUFF 	5	/* Number of entries in the  load
				 * buffer */

#define NUM_MEM_ACCESS 1	/* Number of data accesses per clock cycle */


#define NUM_BB_ENTRIES 419	/* default size for the branch target buffer,
				 * may be changed in the machine
				 * configuration file in case too many
				 * collisions */

#define HEAD(X)  (machPtr->X->element[machPtr->X->head])
#define TAIL(X)  (machPtr->X->element[machPtr->X->head])
#define FIRST(X) (machPtr->X->firstElement)
#define LAST(X)  (machPtr->X->lastElement)
#define COUNT(X) (machPtr->X->entryCount)
#define MAX(X)   (machPtr->X->numEntries)
#define WINDOW(X) ((X==INTEGER) ? (machPtr->iWindow):(machPtr->fpWindow))
#define REORDER(X) ((X== INTEGER) ? (machPtr->iReorderBuffer):(machPtr->fpReorderBuffer))
#define OTHER_REORDER ((X== INTEGER) ? (machPtr->fpReorderBuffer):(machPtr->iReorderBuffer))
#ifndef HELP_PATH
#define HELP_PATH "/usr/local/lib/superdlx/helpfiles/"

#endif

#define INST_WINDOW 0
#define REORDER_BUF 1

typedef struct {
    char *opName;
    int opUnit;
    int classe;
    int funcUnit;
    int firstOperand;
    int secondOperand;
    int result;
} OpBehave;

/* main.c*/
/*extern*/ int memArg;//quito la inicializacion a ver que pasa=0;
int mem_size;            // Se ha eliminado lo de extern 
extern int x_version;
//char *HelpPath= ".\\";          // se ha eliminado el extern y se inicializa al
                         // directorio actual

/* supersim.c*/
extern OpBehave infOp[];
extern char *type[];
extern char *intUnit[];
extern char *fpUnit[];


/* supersim.c */
int Simulate();

/* superfetch.c */
int FetchInstruction();

/* superdecode.c */
int DecodeInstruction();


/* supercalc.c */
int ExecuteInteger();
int ExecuteFloat();
void StartOfExec();
void EndOfExec();
int SendLoads();
int SendStores();
void RemoveOperation();

/* declarations of functions that are now exported by sim.c */
void *mymalloc();
DLX * Sim_Init(int memSize, int branckPred);
void Create_Interp(DLX * machPtr,Tcl_Interp *interp);
void Sim_Create(DLX *machPtr, int memory);
int  Main_QuitCmd(DLX * machPtr, Tcl_Interp * interp, int argc,char ** argv);
int Sim_GoCmd(), Sim_StepCmd();
int Sim_InspectCmd();
int Sim_DumpStats();
int Sim_NextCmd(), Sim_HelpCmd();
int Sim_ResetCmd();
void Reset();
//void Create_Interp();
int ReadMem();
void Compile();
int AddressError();
int WriteMem();
int Mult();
int BusError();
int Overflow();


/* inspect.c */
void displayWindow();
void displayReorderBuffer();
void displayDataBuffer();
void displayFuncUnits();
void displayDistrib();


void liberaStruct(DLX* machPtr); // Esta en sin.c
int readConfigFile(FILE * fichero, Tcl_Interp * interp, DLX * machPtr); // Esta en read.c

//a�adidos por antonio para predicci�n de saltos  : estan en branchpred.c
void insertOutcome(BranchInfo * branch,int outcome,int numCount);
void setPred(BranchInfo* branch, int* branchSchema,int value,int outcome);
void setPredValue(BranchInfo *branch,unsigned int branchSchema[],int value);
int getPred(BranchInfo *branch,int* branchSchema);
void brInit(BranchInfo * branch,int *branchSchema);
#endif

