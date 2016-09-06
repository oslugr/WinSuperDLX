
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
static char rcsid[] = "$Id: read.c,v 1.3 93/04/22 18:55:03 cecile Exp $";


/*
 * read.c
 * 
 * 
 * Read the machine configuration file which is passed as argument of the
 * program command line.
 * 
 * 
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>    // añadido
#include "tcl/tcl.h"   // cambiado
#include "dlx.h"
#include "superdlx.h"

/*--------------------------------------------------------------------------
 * getParameter --
 * 
 * this procedure reads a figure in the machine configuration file. the figure
 * is always constrained by a lower value, but not necessarily an upper value
 *
 *----------------------------------------------------------------------------
 */

void 
getParameter(configFile, error, upper, lower, dest)
    FILE *configFile;
    int *error;
    int upper;
    int lower;
    int *dest;

{
    int value;

    if (fscanf(configFile, "%d", &value) != 1) {
	*error = 1;
    } else {
	if (((upper) && ((value < lower) || (value > upper))) ||
	    ((!upper) && (value < lower))) {
	    *error = 1;
	} else {
	    *dest = value;

	}
    }

}

/*--------------------------------------------------------------------------
 * getFuParameter --
 * 
 * gets the characteristics of a functional unit (number of units, latency)
 * 
 *---------------------------------------------------------------------------
 */

void 
getFuParameters(interp, configFile, lineNum, nextString, flag, error,
		upperBound, lowerBound, unit)
    Tcl_Interp *interp;
    FILE *configFile;
    char **nextString;
    int *lineNum;
    int *flag;
    int *error;
    int upperBound;
    int lowerBound;
    FunctionalUnit *unit;

{

    *flag = fscanf(configFile, "%s", *nextString);
    *lineNum += 1;


    while (*flag != EOF) {

	if (strcmp(*nextString, "number") == 0) {

	    getParameter(configFile, error, upperBound, lowerBound,
			 &unit->num_units);

	} else if (strcmp(*nextString, "latency") == 0) {
	    getParameter(configFile, error, 0, lowerBound,
			 &unit->latency);

	} else {
	    break;
	}

	if (*error) {
	    sprintf(interp->result, "invalid specification line %d \n", 
				*lineNum);
	    break;
	}
	*flag = fscanf(configFile, "%s", *nextString);
	*lineNum += 1;
    }
}

/*
 *--------------------------------------------------------------------------
 * readConfigFile --
 * 
 * Main function for reading the machine configuration text file
 * 
 * Result: A Tcl result
 * 
 * Side Effects: The machine structure (machPtr) is updated
 * 
 * Called By: main in main.c
 * 
 *--------------------------------------------------------------------------
 */

int 
readConfigFile(configFile, interp, machPtr)
    FILE *configFile;
    Tcl_Interp *interp;
    DLX *machPtr;


{
    int lineNum;
    int flag;
    int value, error = 0;
    char *nextString;



    nextString = (char *) malloc(30*sizeof(char));
    flag = fscanf(configFile, "%s", nextString);

    lineNum = 1;


    while ((flag != EOF) && (!error)) {

	if (strcmp(nextString, "Instructions_process_per_cycle") == 0) {

	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;
	    while (flag != EOF) {

		if (strcmp(nextString, "fetch") == 0) {

		    getParameter(configFile, &error, 0, 1, &machPtr->numFetch);

		} else if (strcmp(nextString, "decode") == 0) {
		    getParameter(configFile, &error, 0, 1, &machPtr->numDecode);
		} else if (strcmp(nextString, "commit") == 0) {

		    getParameter(configFile, &error, 0, 1, &machPtr->numCommit);

		} else {
		    break;
		}
		if (error) {
		    sprintf(interp->result, "invalid specification line %d \n", 
				lineNum);
		    return TCL_ERROR;
		}
		flag = fscanf(configFile, "%s", nextString);
		lineNum += 1;
	    }

	    continue;

	} else if (strcmp(nextString, "Memory") == 0) {

	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    while ((flag != EOF) && (!error)) {
		if (strcmp(nextString, "size") == 0) {

		    if (fscanf(configFile, "%d", &value) != 1) {
			error = 1;

		    } else if (!memArg) {

			/*
			 * if a memory size has already been passed as
			 * argument of the command line, the memory size
			 * specified by the machine configuration file is
			 * ignored
			 */
			mem_size = value ;
			value = (value + 3) >> 2;
			if (value < 1) {
			    error = 1;
			} else {
			    machPtr->numWords = (value + 3) & ~0x3;
			    machPtr->numChars = machPtr->numWords * 4;
			}
		    }
		} else if (strcmp(nextString, "latency") == 0) {

		    getParameter(configFile, &error, 0, 1, 
				&machPtr->mem_latency);
		} else if (strcmp(nextString, "accesses") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->num_mem_access);

		} else {
		    break;
		}

		if (error) {
		    sprintf(interp->result,
			    "invalid specification line %d \n", lineNum);
		    return TCL_ERROR;
		}
		flag = fscanf(configFile, "%s", nextString);
		lineNum += 1;
	    }

	    continue;

	} else if (strcmp(nextString, "Reorder_Buffer_sizes") == 0) {

	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    while (flag != EOF) {
		if (strcmp(nextString, "integer") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->iReorderBuffer->numEntries);

		} else if (strcmp(nextString, "float") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->fpReorderBuffer->numEntries);

		} else {
		    break;
		}

		if (error) {
		    sprintf(interp->result, "invalid specification line %d \n", 
			lineNum);
		    return TCL_ERROR;
		}
		flag = fscanf(configFile, "%s", nextString);
		lineNum += 1;
	    }

	    continue;

	} else if (strcmp(nextString, "Instruction_Window_sizes") == 0) {

	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    while (flag != EOF) {
		if (strcmp(nextString, "integer") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->iWindow->numEntries);

		} else if (strcmp(nextString, "float") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->fpWindow->numEntries);

		} else {
		    break;
		}

		if (error) {
		    sprintf(interp->result, "invalid specification line %d \n", 
				lineNum);
		    return TCL_ERROR;
		}
		flag = fscanf(configFile, "%s", nextString);
		lineNum += 1;
	    }

	    continue;



	} else if (strcmp(nextString, "Data_Buffer_sizes") == 0) {

	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    while (flag != EOF) {
		if (strcmp(nextString, "load") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->loadBuffer->numEntries);

		} else if (strcmp(nextString, "store") == 0) {
		    getParameter(configFile, &error, 0, 1,
				 &machPtr->storeBuffer->numEntries);
		} else {
		    break;
		}

		if (error) {
		    sprintf(interp->result,
			    "invalid specification line %d \n", lineNum);
		    return TCL_ERROR;
		}
		flag = fscanf(configFile, "%s", nextString);
		lineNum += 1;
	    }



	    continue;

	} else if (strcmp(nextString, "Instruction_Queue_size") == 0) {

	    getParameter(configFile, &error, 0, 1, 
			&machPtr->insQueue->numEntries);
	    if (error) {
		sprintf(interp->result, "invalid specification line %d \n", 
			lineNum);
		return TCL_ERROR;
	    }
	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    continue;

	} else if (strcmp(nextString, "Branch_Buffer_size") == 0) {


	    getParameter(configFile, &error, 0, 1, &machPtr->bb_size);
	    if (error) {
		sprintf(interp->result, "invalid specification line %d \n", 
					lineNum);
		return TCL_ERROR;
	    }
	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    continue;

	} else if (strcmp(nextString, "Integer_Functional_Units") == 0) {

	    int fuUnit;
	    FunctionalUnit *unit;


	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    while (flag != EOF) {

		if (strcmp(nextString, "alu") == 0) {
		    fuUnit = INT_ALU;
		} else if (strcmp(nextString, "shift") == 0) {
		    fuUnit = INT_SHIFT;
		} else if (strcmp(nextString, "comp") == 0) {
		    fuUnit = INT_COMP;
		} else if (strcmp(nextString, "address") == 0) {
		    fuUnit = INT_DATA;
		} else if (strcmp(nextString, "branch") == 0) {
		    fuUnit = INT_BRANCH;
		} else {
		    break;
		}

		if (error) {
		    sprintf(interp->result,
			    "invalid specification line %d \n", lineNum);
		    return TCL_ERROR;
		}
		unit = &machPtr->intUnits[fuUnit];
		getFuParameters(interp, configFile, &lineNum, &nextString,
				&flag, &error,
				100, 1, unit);
	    }
	    continue;

	} else if (strcmp(nextString, "Floating_Point_Units") == 0) {
	    int fuUnit;
	    FunctionalUnit *unit;

	    flag = fscanf(configFile, "%s", nextString);
	    lineNum += 1;

	    while (flag != EOF) {
		if (strcmp(nextString, "add") == 0) {
		    fuUnit = FP_ADD;
		} else if (strcmp(nextString, "mult") == 0) {
		    fuUnit = FP_MUL;
		} else if (strcmp(nextString, "div") == 0) {
		    fuUnit = FP_DIV;
		} else if (strcmp(nextString, "convt") == 0) {
		    fuUnit = FP_CONV;
		} else if (strcmp(nextString, "comp") == 0) {
		    fuUnit = FP_COMP;
		} else if (strcmp(nextString, "address") == 0) {
		    fuUnit = FP_DATA;
		} else if (strcmp(nextString, "branch") == 0) {
		    fuUnit = FP_BRANCH;
		} else {
		    break;
		}
		unit = &machPtr->fpUnits[fuUnit];
		getFuParameters(interp, configFile, &lineNum, &nextString,
				&flag, &error,
				100, 1, unit);
		if (error) {
		    sprintf(interp->result,
			    "invalid specification line %d \n", lineNum);
		    return TCL_ERROR;
		}
	    }



	    continue;
	} else {
	    sprintf(interp->result,
		    "Unrecognized field \"%s\" at line %d \n",
		    nextString,
		    lineNum);
	    break;
	}

    }
    fclose(configFile);
    return TCL_OK;


}
