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
 * sym.h --
 *
 *	Declarations for procedures exported by the symbol table
 *	module for mipsim.
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
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/RCS/sym.h,v 1.1 93/04/13 14:05:14 cecile Exp Locker: cecile $ SPRITE (Berkeley)
 */

#ifndef _SYM
#define _SYM 1


#include "dlx.h"
#include "sym.h"
#include "hash.h"

/*
 * Return values from Sym_GetSym:
 */



#define SYM_FOUND	0
#define SYM_REGISTER	1
#define SYM_AMBIGUOUS	2
#define SYM_NOT_FOUND	3
#define SYM_REG_NOT_OK	4
#define SYM_FREG_FOUND	5

/*
 * Flags to pass to Sym_GetSym:
 */

#define SYM_REGS_OK		0x1
#define SYM_PSEUDO_OK		0x2
#define SYM_FREGS_OK		0x4

/*
 * Flags to pass to Sym_AddSymbol:
 */

/* Leave value 1 available so SYM_REGISTER from above can be used */
#define SYM_NO_ADDR	0x2
#define SYM_GLOBAL	0x4


/* ERA, 11-3-91:  Hash Tables for local files including the name of the file */

typedef struct {
    Hash_Table  *tbl;
    char	*name;
} asmHTbl;

typedef struct {
    DLX *machPtr;		/* Machine information for things like
				 * symbol table and error messages. */
    char *fileName;		/* Filename for symbol lookup;  see Sym_GetSym
				 * argument of same name for more info. */
    int ignoreUndef;		/* Non-zero means don't worry about
				 * undefined symbols. */
    char *expr;			/* Position of the next character to be
				 * scanned from the expression string. */
    int token;			/* Type of the last token to be parsed from
				 * expr.  See below for definitions.
				 * Corresponds to the characters just
				 * before expr. */
    char *tokenChars;		/* Poiner to first character of expression
				 * that mapped to token (i.e. the value of
				 * expr before the call to Lex that produced
				 * token). */
    int number;			/* If token is NUMBER, gives value of
				 * the number. */
} ExprInfo2;



void Sym_DeleteSymbols(Hash_Table *delTbl, char *fileName);
int Sym_AddSymbol(DLX * machPtr,Hash_Table * localTbl,char * fileName,char * name,unsigned int address, int flags);
int Sym_GetSym(DLX *machPtr,Hash_Table * localTbl,char* fileName, char *name,int  flags,unsigned int *addrPtr);
static int Lex(register ExprInfo2* infoPtr, Hash_Table * localTbl);
static int GetValue(register ExprInfo2 * infoPtr, Hash_Table * localTbl,int prec);
int Sym_EvalExpr(DLX *machPtr, Hash_Table * localTbl,char * fileName,
                 char * exprString,int  ignoreUndef,int * valuePtr,char ** endPtr);
char * Sym_GetString(DLX *machPtr,unsigned int  address);


#endif /* _SYM */


