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

/*
 * asm.h --
 *
 *	Definitions used to assemble and disassemble DLX instructions.
 *
 * Copyright 1989 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/RCS/asm.h,v 1.2 93/04/14 00:55:04 cecile Exp $ SPRITE (Berkeley)
 */

#ifndef _ASM_HDR
#define _ASM_HDR 1

/*
 * Variables and procedures exported to rest of simulator:
 */
#include "dlx.h"

typedef struct {
    char *file;			/* Name of file currently being assembled
				 * (none means info to assemble comes from
				 * some source other than file). */
    int lineNum;		/* Line number within file (-1 if info being
				 * assembled doesn't come from file). */
    char *line;			/* Contents of entire line being assembled
				 * (useful for error messages). */
    unsigned int codeAddr;	/* Next address to place information in
				 * code area. */
    unsigned int dataAddr;	/* Next address to place information in
				 * data area. */
    unsigned int dot;		/* Address at which to place information
				 * (can be either code or data).

    /*
     * Information used to build up a concatenated version of all the
     * error strings that occur while reading the files:
     */

    char *message;		/* Pointer to current message (calloc-ed).
				 * NULL means no error has occurred so far. */
    char *end;			/* Address of NULL byte at end of message;
				 * append new messages here. */
    int totalBytes;		/* # of bytes allocated at message. 0 means
				 * no error has occurred yet. */
    int errorCount;		/* If too many errors occur, give up. */

    int flags;			/* Various flags:  see below for values. */
} LoadInfo;



int Asm_Assemble(DLX * machPtr,Hash_Table * localTbl,char * fileName,char * string,
                  unsigned int dot, char * wholeLine, int sizeOnly,
	               int * sizePtr,int * codePtr);

char * Asm_Disassemble(DLX *machPtr,int  ins,unsigned int pc);
int Asm_AsmCmdOld(DLX *machPtr,Tcl_Interp * interp,int argc,char ** argv);
int Asm_AsmCmd(DLX * machPtr,Tcl_Interp * interp,int  argc,char ** argv);
int alloc_asmHashTbl(DLX *machPtr);
int Asm_LoadCmd(DLX * machPtr,Tcl_Interp * interp,int argc, char **argv);
/// *********** Por problemas de linkado, se ha eliminado la funci�n READFILE ****** 
static void ReadFile(char * fileName,Hash_Table *  localTbl,DLX * machPtr,register LoadInfo * infoPtr);
static void AddErrMsg(Tcl_Interp * interp,register LoadInfo * infoPtr,int  addHeader);
static int StoreWords(register DLX * machPtr,unsigned int  address,int * wordPtr,int  numWords);
static void IndicateError(Tcl_Interp * interp,char * errMsg,char * string,char * pos);

#endif /* _ASM_HDR */

