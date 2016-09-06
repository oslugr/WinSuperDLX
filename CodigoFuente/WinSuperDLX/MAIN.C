/*
    Copyright 1989 Regents of the University of California.
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
    
    
    Última modificacion :  6-10-2001
	                      14-10-2001
*/

 /*
 * main.c --
 * 
 * Main program for the superscalar simulator for DLX architecture.
 * 
 * This is a modified version of main
 * procedure of dlxsim . The way to parse the command line under the
 * simulator's prompt has been kept. As in dlxsim, the tcl-interpretor
 * package is used to interpret the users' commands
 */

#ifndef lint
static char rcsid[] = "$Header: /auto/acaps/export/compiler/dlx/superdlx/RCS/main.c,v 1.12 1993/12/05 01:11:04 lozano Exp lozano $ SPRITE (Berkeley)";
#endif				/* not lint */




//#include <bstring.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
//#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tcl.h"
#include "tclutil.h"
#include "dlx.h"
/* #include "xdlx.h" */
#include "version.h"
#include "superdlx.h"

Tcl_Interp *interp;

/* Variable to keep the name of the help path */
/* Luis Lozano, August 25/93 */

char *HelpPath;   /* AÑADIDO  realmente no lo es pero hay que ver si se puede quitar ya que puede que no tenga mucho sentido */

/*
 * Forward references to procedures declared later in this file:
 */
/*
 * static void Interrupt();
 */
/* information on command arguments 
   (declared as globals, because easier for the X functions to use them */
int branchPred = 0;		/* indicates if branch prediction is used */
int memArg = 0;			/* indicates if the memory size is entered as
				         * argument of the command line */
int mem_size = MEMSIZE;
 




/*
 * ----------------------------------------------------------------------
 * 
 * main --
 * 
 * Top-level procedure for the superscalar simulator.
 * 
 * Results: None.
 * 
 * ----------------------------------------------------------------------
 */

void usageError(char * cmd);    /* AÑADIDO  se añade porque es un código que se repite hasta 4 veces*/

main(argc, argv)
    int argc;
    char **argv;

{
    
    char cmd[1000], *p, *command = *argv;
    char *fileName;
    int c, result;
    int specFile = 0;		/* indicates if a machine configuration file
				 * has been specified by the command line */


    FILE *configFile;           /* puntero al fichero de configuracion */


    interp = Tcl_CreateInterp();

    /* Get the enviroment variable for the help files path */

    if ((HelpPath = getenv ("SUPERDLXHELP")) == NULL){
      /*	fprintf(stderr, "\nNo help files specified in SUPERDLXHELP, using default path\n\n"); */
	        HelpPath = HELP_PATH;
    }
      
    /* parse the command line */

    while (argv++, --argc) {
	if (*(p = *argv) != '-') {
          usageError(command);
	}
	switch (*(p + 1)) {
	  case 'm':
	    switch (*(p + 2)) {
	      case 's':
		        /*
		         * specifies the size of memory.it can also be specified from
		         * the machine configuration file . If specified as a command
		         * argument, the specification in the machine configuration
		         * file is ignored
		        */
		        /*mem_size = (atoi(p + 3) + 3) >> 2;*/
		        /* mem_size is the number of BYTES specified by the user */
		        mem_size = atoi(p + 3);
		        memArg = 1;
		        break;
	      default:
		        usageError(command);
		        break;
	    }
	    break;
	  case 'b':		/* branch prediction flag */
	    switch (*(p + 2)) {
	      case 'p': branchPred = 1;
              		break;
	      default :
		        usageError(command);
		        break;
	    }
	    break;
	  case 'f':{		/* specifies the machine */
		         --argc;
		         fileName = *(++argv);
		         specFile = 1;
		         break;
	           }
	  
      case 'v': fprintf(stdout,"SuperDLX Version %s\n",VERSION_STRING);
                exit(0);

	
	  default:  usageError(command);
	            break;
	}
    }

    // llegados a este punto se han capturado los parametros de entrada
    if (mem_size < 4) {
	fprintf(stderr, "invalid memory size (>=1)\n");
	exit(0);
    }

    /*
     * initialization of the machine structure with  the existing default
     * parameters  of superdlx.h
     */
    
    machPtr = (DLX *) Sim_Init(mem_size, branchPred);
    Create_Interp(machPtr, interp);                      /* AÑADIDO ¿por qué no lo asigna a algo? */

    printf("    SuperDLX Version %s\n\n",VERSION_STRING);

    printf("(C) copyright 1990, University of California\n");
    printf("(C) copyright 1993, McGill University\n");

    

	if (specFile) {

	    /*
	     * if  a machine configuration file is specified, we parse it and
	     * default parameters are replaced by the ones specified in this
	     * file
	     */

	    if ((configFile = fopen(fileName, "r")) == NULL) {
		     printf("File %s does not exist\n", fileName);
		     exit(0);
	    }
	    result = readConfigFile(configFile, interp, machPtr);
	    fclose(configFile);
	    if (*interp->result != 0) {   // Si se produce algun error, se termina
		   printf("%s\n", interp->result);
		   exit(0);
	    }
	}
	/*
	 * we initialize all structures such as memory, buffers, queues in
	 * the machine structure machPtr
	 */
	Sim_Create(machPtr, 1);


	/* (void) signal(SIGINT, Interrupt); */         /* AÑADIDO bueno, quitado  */


	printf("\nFor a list of the available commands: type \"help\"\n\n");

	/*
	 * The following parses the command line under the simulator's
	 * prompt. This has been taken from dlxsim
	 */

	while (1) {
	    
		clearerr(stdin);   // Se borra el fichero de salida estandar de error 
	    p = Tcl_GetVar(interp, "prompt", 1);
	    if ((*p == 0) || (Tcl_Eval(interp, p, 0, (char **) 0) != TCL_OK)) {
		    fputs("(superdlx) ", stdout); /* es como si la función Tcl_Eval escribiese el propio mensaje de error */
	    } 
		else { /* Si no ocurre ningún error, Tcl_Eval escribe en interp->result el resultado de la ejecución de la orden */
		    fputs(interp->result, stdout);
	    }
	    fflush(stdout);
	    p = cmd;

	    while (1) {
		  c = getchar();
		  if (c == EOF) {     // comprueba el final de una linea, para pasar la orden
		    if (p == cmd) { // introducida al programa. Si no se introduce nada se termina
			   exit(0);
		    }
		    goto gotCommand; // Se pasa la orden al interprete
		  }
		  *p = c;  // los caracteres que se captan se van metiendo en p.
		  p++;     // se incrementa p (recordar que p se mueve dentro de cmd
		
          if (c == '\n') {       // Si se introdujo un fin de linea
		    register char *p2;
		    int parens;   // numero de llaves abiertas
            int brackets; // numero de corchetes abiertos
            int numBytes;

		    for (p2 = cmd, parens = 0, brackets = 0; p2 != p; p2++) {
			 switch (*p2) {
			  case '\\':
			             Tcl_Backslash(p2, &numBytes); // se modifica numBytes
			             p2 += numBytes - 1;
			             break;
			  case '{':
			             parens++;
			             break;
			  case '}':
			             parens--;
			             break;
			  case '[':
			             brackets++;
			             break;
			  case ']':
			             brackets--;
			             break;
			 } /* switch */
		    } /* for */
		    if ((parens <= 0) && (brackets <= 0)) 
			     goto gotCommand;
		    
		}/* if c == \n */
	   } /* while (1) mas interno */
       
		gotCommand:  *p = 0;
        	         (void) Tcl_Eval(interp, cmd, 0, &p);
	                  if (*interp->result != 0) {
		                  printf("%s\n", interp->result);
	                  }
	}/* while (1) mas externo */

    
    return 0;
}


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
/* AÑADIDO */
/* ----------------- funcion añadida para eliminar ordenes goto -------- */
void usageError(char * command) {
/* Esta funcion escribe un mensaje de error sobre el uso del programa y se sale */
       
    fprintf(stderr,"usage : %s [-f <file name>] [-ms#] [-bp] [-v] \n", command);
	exit(1);
}
