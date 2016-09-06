
/* 
 * getput.c --
 *
 *	This file implements commands to query or change the state
 *	of the DLX machine being simulated.
 *
 * Copyright 1989 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /auto/labs/acaps/compiler/dlx/superdlx/RCS/getput.c,v 1.2 93/04/13 23:43:26 cecile Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcl/tcl.h" // cambiado
#include "asm.h"
#include "gp.h"
#include "dlx.h"
#include "sym.h"

/*
 * Forward declarations for procedures declared later in this file:
 */

static int	GetAddress();
static char *	GetString();

/* ERA, 11-3-91:  Added these 3 variables */
Hash_Table	*curr_LocalTbl;
extern int	num_asm_files;
extern asmHTbl	*asmHashTbl;



/*
 *----------------------------------------------------------------------
 *
 * Gp_FileCmd --
 *
 *	Set the current local file for looking up variables not
 *	defined globally.
 *
 *	ERA, 11-3-91:  Added this routine.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

int
Gp_FileCmd(machPtr, interp, argc, argv)
    DLX *machPtr;			/* Machine description. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int	       i;
    Hash_Table *newTbl;

    if (argc != 2) {
	sprintf(interp->result,
		"wrong # args:  should be \"%.50s location [flags]\"",
		argv[0]);
	return TCL_ERROR;
    }


    newTbl = NULL;
    for (i = 1; i <= num_asm_files; i++) {
	if (!strcmp (asmHashTbl[i].name, argv[1]))
	    newTbl = asmHashTbl[i].tbl;
    }

    if (newTbl) curr_LocalTbl = newTbl;
    else if (!strcmp ("global-only", argv[1]))
	curr_LocalTbl = (Hash_Table *) NULL;
    else {
	sprintf(interp->result, "File: %s not currently loaded.", argv[1]);
	return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Gp_GetCmd --
 *
 *	This procedure is invoked to process the "get" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * Gp_FGetCmd --
 *
 *	This procedure is invoked to process the "fget" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * Gp_PutCmd --
 *
 *	This procedure is invoked to process the "put" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * Gp_FPutCmd --
 *
 *	This procedure is invoked to process the "fput" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------
 *
 * GetAddress --
 *
 *	Convert a string to information about an address, suitable for
 *	reading or writing the location.
 *
 * Results:
 *	Returns a standard Tcl result.
 *
 * Side effects:
 *	Generates an error in interp if string can't be parsed into
 *	an address.
 *
 *----------------------------------------------------------------------
 */

static int
GetAddress(machPtr, string, addressPtr, regPtr)
    DLX *machPtr;			/* Machine address will be used with.
					 * Also used for error reporting. */
    char *string;			/* Specification of address. */
    unsigned int *addressPtr;		/* Store address of value here. */
    int *regPtr;			/* Store 1 here if address refers to
					 * a register, 0 for memory location. */
{
    unsigned int result;
    char *end;

    /* ERA, 11-3-91:  Added 2nd curr_LocalTbl parm. */
    result = Sym_GetSym(machPtr, curr_LocalTbl, (char *) NULL, string,
			SYM_PSEUDO_OK, addressPtr);
    if (result == SYM_FREG_FOUND) {
	*regPtr = 1;
	*addressPtr += 32;
	return TCL_OK;
    }
    if (result == SYM_REGISTER) {
	*regPtr = 1;
	return TCL_OK;
    }
    if (result == SYM_FOUND) {
	*regPtr = 0;
	return TCL_OK;
    }

    /* ERA, 11-3-91:  Added 2nd curr_LocalTbl parm. */
    result = Sym_EvalExpr(machPtr, curr_LocalTbl, (char *) NULL, string, 0,
	    (int *) addressPtr, &end);
    if (result != TCL_OK) {
	return result;
    }
    if (*end != 0) {
	sprintf(machPtr->interp->result,
		"mistyped expression \"%.50s\"", string);
	return TCL_ERROR;
    }
    *regPtr = 0;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * GetString --
 *
 *	Given an address, return the ASCII string at that address.
 *
 * Results:
 *	The return value is a pointer to ASCII string that's at
 *	"address" in machPtr's memory.  The integer at *countPtr
 *	is overwritten with the number of bytes in the string,
 *	including the terminating NULL character.  If the string
 *	is very long, the return value may be truncated to hold only
 *	the first few characters of the string (*countPtr will also
 *	be truncated).  The return value is stored in a static buffer
 *	that will be overwritten on the next call to this procedure.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
GetString(machPtr, address, countPtr)
    register DLX *machPtr;	/* Machine whose memory is to be examined. */
    unsigned int address;	/* Address of string. */
    int *countPtr;		/* Fill in string length (including NULL
				 * character, if any) here. */
{
#define MAX_LENGTH 200
    static char buffer[(4*MAX_LENGTH)+20];
    char *p;
    unsigned int index;
    int value, count;

    buffer[0] = '"';
    p = buffer+1;
    for (count = 0; ; count++, address++) {
	index = ADDR_TO_INDEX(address);
	if (index >= machPtr->numWords) {
	    break;
	}
	value = machPtr->memPtr[index].value;
	switch (address & 0x3) {
	    case 0:
		value >>= 24;
		break;
	    case 1:
		value >>= 16;
		break;
	    case 2:
		value >>= 8;
		break;
	}
	value &= 0xff;
	if (value == 0) {
	    *countPtr = count+1;
	    break;
	}
	if (count == MAX_LENGTH) {
	    *countPtr = MAX_LENGTH;
	    strcpy(p, "...");
	    p += 3;
	    break;
	}
	if (value == '\\') {
	    strcpy(p, "\\\\");
	    p += 2;
	} else if (value == '"') {
	    strcpy(p, "\\\"");
	    p += 2;
	} else if (isascii(value) && isprint(value)) {
	    *p = value;
	    p++;
	} else if (value == '\n') {
	    strcpy(p, "\\n");
	    p += 2;
	} else if (value == '\t') {
	    strcpy(p, "\\t");
	    p += 2;
	} else if (value == '\b') {
	    strcpy(p, "\\b");
	    p += 2;
	} else if (value == '\r') {
	    strcpy(p, "\\r");
	    p += 2;
	} else {
	    sprintf(p, "\\x%02x", value);
	    p += 4;
	}
    }
    *p = '"';
    p[1] = 0;
    return buffer;
}

/*
 *----------------------------------------------------------------------
 *
 * Gp_PutString --
 *
 *	Given an ASCII string, store it in the memory of a machine.
 *
 * Results:
 *	The return value is the number of bytes actually stored
 *	in machPtr's memory.  If endPtr isn't NULL, *endPtr is
 *	filled in with the address of the character that terminated
 *	the string (either term or 0).
 *
 * Side effects:
 *	The bytes of "string" are stored in machPtr's memory starting
 *	at "address".  Standard Tcl backslash sequences are interpreted.
 *
 *----------------------------------------------------------------------
 */

int
Gp_PutString(machPtr, string, term, address, addNull, endPtr)
    register DLX *machPtr;		/* Machine whose memory is to
					 * be modified. */
    register char *string;		/* String to store. */
    char term;				/* Character that terminates string. */
    unsigned int address;		/* Where in machPtr's memory to store.*/
    int addNull;			/* If non-zero, add a terminating
					 * NULL character to memory after
					 * the string. */
    char **endPtr;			/* If non-NULL, fill in with address
					 * of terminating character. */
{
    int backslashCount;
    int size;

    size = 0;
    while (1) {
	if (*string == '\\') {
	    Gp_PutByte(machPtr, address,
		    Tcl_Backslash(string, &backslashCount));
	    string += backslashCount;
	} else if ((*string == 0) || (*string == term)) {
	    break;
	} else {
	    Gp_PutByte(machPtr, address, *string);
	    string++;
	}
	address += 1;
	size += 1;
    }
    if (addNull) {
	Gp_PutByte(machPtr, address, 0);
	size += 1;
    }
    if (endPtr != 0) {
	*endPtr = string;
    }
    return size;
}

/*
 *----------------------------------------------------------------------
 *
 * Gp_PutByte --
 *
 *	Store a particular byte at a particular address in memory.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	MachPtr's memory gets modified.
 *
 *----------------------------------------------------------------------
 */

void
Gp_PutByte(machPtr, address, value)
    register DLX *machPtr;		/* Machine whose memory to modify. */
    unsigned int address;		/* Where to store value. */
    int value;				/* Value to store as byte at address. */
{
    MemWord *wordPtr;
    int index;

    index = ADDR_TO_INDEX(address);
    if (index < machPtr->numWords) {
	wordPtr = &machPtr->memPtr[index];
	switch (address & 0x3) {
	    case 0:
		wordPtr->value = (wordPtr->value & 0xffffff)
			| (value << 24);
		break;
	    case 1:
		wordPtr->value = (wordPtr->value & 0xff00ffff)
			| ((value << 16) & 0xff0000);
		break;
	    case 2:
		wordPtr->value = (wordPtr->value & 0xffff00ff)
			| ((value << 8) & 0xff00);
		break;
	    case 3:
		wordPtr->value = (wordPtr->value & 0xffffff00)
			| (value & 0xff);
		break;
	}
	wordPtr->opCode = OP_NOT_COMPILED;
    }
}



