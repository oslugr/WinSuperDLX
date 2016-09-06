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
 * tcl.h --
 *
 *	This header file describes the externally-visible facilities
 *	of the Tcl interpreter.
 *
 * Copyright 1987 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/tcl/RCS/tcl.h,v 1.1 93/04/13 13:53:01 cecile Exp $ SPRITE (Berkeley)
 */
#ifndef _CLIENTDATA
typedef int *ClientData;
#define _CLIENTDATA
#endif


#ifndef _TCL
#define _TCL
#define TCL_VERSION "tcl 1.1"        /* A�ADIDO */
/*
 * Data structures defined opaquely in this module.  The definitions
 * below just provide dummy types.  A few fields are made visible in
 * Tcl_Interp structures, namely those for returning string values.
 * Note:  any change to the Tcl_Interp definition below must be mirrored
 * in the "real" definition in tclInt.h.
 */
#include <stdio.h>
//#include <stdarg.h>


typedef struct {
    char *result;		/* Points to result string returned by last
				 * command. */
    int dynamic;		/* Non-zero means result is dynamically-
				 * allocated and must be freed by Tcl_Eval
				 * before executing the next command. */
    int errorLine;		/* When TCL_ERROR is returned, this gives
				 * the line number within the command where
				 * the error occurred (1 means first line). */
} Tcl_Interp;

typedef int *Tcl_Trace;
typedef int *Tcl_CmdBuf;

/*
 * When a TCL command returns, the string pointer interp->result points to
 * a string containing return information from the command.  In addition,
 * the command procedure returns an integer value, which is one of the
 * following:
 *
 * TCL_OK		Command completed normally;  interp->result contains
 *			the command's result.
 * TCL_ERROR		The command couldn't be completed successfully;
 *			interp->result describes what went wrong.
 * TCL_RETURN		The command requests that the current procedure
 *			return;  interp->result contains the procedure's
 *			return value.
 * TCL_BREAK		The command requests that the innermost loop
 *			be exited;  interp->result is meaningless.
 * TCL_CONTINUE		Go on to the next iteration of the current loop;
 *			interp->result is meaninless.
 */

#define TCL_OK		0
#define TCL_ERROR	1
#define TCL_RETURN	2
#define TCL_BREAK	3
#define TCL_CONTINUE	4

#define TCL_RESULT_SIZE 199

/*
 * Flag values passed to Tcl_Eval (see the man page for details;  also
 * see tclInt.h for additional flags that are only used internally by
 * Tcl):
 */

#define TCL_BRACKET_TERM	1
typedef struct Var {
    char *value;		/* Current value of variable (either points
				 * to static space after name, or to dynamic
				 * space if VAR_DYNAMIC is set). */
    int valueLength;		/* Number of bytes of storage at the place
				 * referred to by value, not including space
				 * for NULL terminator. */
    int flags;			/* Miscellaneous flags:  see below. */
    struct Var *globalPtr;	/* If VAR_GLOBAL is set, this points to the
				 * global variable corresponding to name. */
    struct Var *nextPtr;	/* Next variable in list, or NULL for end
				 * of list. */
    char name[4];		/* Storage space for variable's name (and
				 * initial value).  The name is at the
				 * beginning, and is null-terminated.
				 * May contain more than 4 bytes (see
				 * VAR_SIZE macro below). */
} Var;
typedef struct Command {
    int (*proc)();		/* Procedure to process command. */
    ClientData clientData;	/* Arbitrary value to pass to proc. */
    void (*deleteProc)();	/* Procedure to invoke when deleting
				 * command. */
    struct Command *nextPtr;	/* Pointer to next command in list, or NULL
				 * for end of list. */
    char name[4];		/* Name of command.  The actual size of this
				 * portion is as large as is necessary to
				 * hold the characters.  This must be the
				 * last subfield of the record. */
} Command;
typedef struct CallFrame {
    Var *varPtr;		/* First in list of all local variables
				 * and arguments for this procedure
				 * invocation. */
    int level;			/* Level of this procedure, for "uplevel"
				 * purposes (i.e. corresponds to nesting of
				 * callerVarPtr's, not callerPtr's).  1 means
				 * outer-most procedure, 0 means top-level. */
    int argc;			/* This and argv below describe name and
				 * arguments for this procedure invocation. */
    char **argv;		/* Array of arguments. */
    struct CallFrame *callerPtr;
				/* Frame of procedure that invoked this one
				 * (NULL if level == 1). */
    struct CallFrame *callerVarPtr;
				/* Frame used by caller for accessing local
				 * variables (same as callerPtr unless an
				 * "uplevel" command was active in the
				 * caller).  This field is used in the
				 * implementation of "uplevel". */
} CallFrame;

/*
 * The structure below defines one history event (a previously-executed
 * command that can be re-executed in whole or in part).
 */

typedef struct {
    char *command;		/* String containing previously-executed
				 * command. */
    int bytesAvl;		/* Total # of bytes available at *event (not
				 * all are necessarily in use now). */
} HistoryEvent;

/*
 * The structure below defines a pending revision to the most recent
 * history event.  Changes are linked together into a list and applied
 * during the next call to Tcl_RecordHistory.  See the comments at the
 * beginning of tclHistory.c for information on revisions.
 */

typedef struct HistoryRev {
    int firstIndex;		/* Index of the first byte to replace in
				 * current history event. */
    int lastIndex;		/* Index of last byte to replace in
				 * current history event. */
    int newSize;		/* Number of bytes in newBytes. */
    char *newBytes;		/* Replacement for the range given by
				 * firstIndex and lastIndex. */
    struct HistoryRev *nextPtr;	/* Next in chain of revisions to apply, or
				 * NULL for end of list. */
} HistoryRev;

/*
 * The structure below defines a trace.  This is used to allow Tcl
 * clients to find out whenever a command is about to be executed.
 */

typedef struct Trace {
    int level;			/* Only trace commands at nesting level
				 * less than or equal to this. */
    void (*proc)();		/* Procedure to call to trace command. */
    ClientData clientData;	/* Arbitrary value to pass to proc. */
    struct Trace *nextPtr;	/* Next in list of traces for this interp. */
} Trace;

/*
 * The stucture below defines an interpreter callback, which is
 * a procedure to invoke just before an interpreter is deleted.
 */

typedef struct InterpCallback {
    void (*proc)();		/* Procedure to call. */
    ClientData clientData;	/* Value to pass to procedure. */
    struct InterpCallback *nextPtr;
				/* Next in list of callbacks for this
				 * interpreter (or NULL for end of
				 * list). */
} InterpCallback;


typedef struct Interp {

    /*
     * Note:  the first three fields must match exactly the fields in
     * a Tcl_Interp struct (see tcl.h).  If you change one, be sure to
     * change the other.
     */

    char *result;		/* Points to result returned by last
				 * command. */
    int dynamic;		/* Non-zero means result is dynamically-
				 * allocated and must be freed by Tcl_Eval
				 * before executing the next command. */
    int errorLine;		/* When TCL_ERROR is returned, this gives
				 * the line number within the command where
				 * the error occurred (1 means first line). */
    Command *commandPtr;	/* First command in list containing all
				 * commands defined for this table. */

    /*
     * Information related to procedures and variables.  See tclProc.c
     * for usage.
     */

    Var *globalPtr;		/* First in list of all global variables for
				 * this command table. */
    Var *localPtr;		/* First in list of all local variables and
				 * arguments for the Tcl procedure that is
				 * currently being executed.  If no procedure
				 * is being executed, or if it has no vars or
				 * args, this will be NULL. */
    int numLevels;		/* Keeps track of how many nested calls to
				 * Tcl_Eval are in progress for this
				 * interpreter.  It's used to delay deletion
				 * of the table until all Tcl_Eval invocations
				 * are completed. */
    CallFrame *framePtr;	/* If a procedure is being executed, this
				 * points to the call frame for the current
				 * procedure (most recently-called).  NULL
				 * means no procedure is active. */
    CallFrame *varFramePtr;	/* Points to the call frame whose variables
				 * are currently in use (same as framePtr
				 * unless an "uplevel" command is being
				 * executed).  NULL means no procedure is
				 * active or "uplevel 0" is being exec'ed. */

    /*
     * Information related to history:
     */

    int numEvents;		/* Number of previously-executed commands
				 * to retain. */
    HistoryEvent *events;	/* Array containing numEvents entries
				 * (dynamically allocated). */
    int curEvent;		/* Index into events of place where current
				 * (or most recent) command is recorded. */
    int curEventNum;		/* Event number associated with the slot
				 * given by curEvent. */
    HistoryRev *revPtr;		/* First in list of pending revisions. */
    char *historyFirst;		/* First char. of current command executed
				 * from history module.  NULL means don't
				 * do history revision (see tclHistory.c
				 * for details on revision). */
    char *evalFirst;		/* If TCL_RECORD_BOUNDS flag set, Tcl_Eval
				 * sets this field to point to the first
				 * char. of text from which the current
				 * command came.  Otherwise Tcl_Eval sets
				 * this to NULL. */
    char *evalLast;		/* Similar to evalFirst, except points to
				 * last character of current command. */

    /*
     * Miscellaneous information:
     */

    int cmdCount;		/* Total number of times a command procedure
				 * has been called for this interpreter. */
    int errInProgress;		/* Non-zero means an error unwind is already
				 * in progress.  Zero means Tcl_Eval has
				 * been invoked since the last error
				 * occurred. */
    int noEval;			/* Non-zero means no commands should actually
				 * be executed:  just parse only.  Used in
				 * expressions when the result is already
				 * determined. */
    int flags;			/* Various flag bits.  See below. */
    Trace *tracePtr;		/* List of traces for this interpreter. */
    InterpCallback *callbackPtr;/* List of callbacks to invoke when
				 * interpreter is deleted. */
    char resultSpace[TCL_RESULT_SIZE];
				/* Static space for storing small results. */
} Interp;
/*
 * The data structure below describes the state of parsing an expression.
 * It's passed among the routines in this module.
 */

typedef struct {
    Tcl_Interp *interp;		/* Intepreter to use for command execution
				 * and variable lookup. */
    char *originalExpr;		/* The entire expression, as originally
				 * passed to Tcl_Expr. */
    char *expr;			/* Position to the next character to be
				 * scanned from the expression string. */
    int token;			/* Type of the last token to be parsed from
				 * expr.  See below for definitions.
				 * Corresponds to the characters just
				 * before expr. */
    int number;			/* If token is NUMBER, gives value of
				 * the number. */
} ExprInfo;

/*
 * The structure below defines a command procedure, which consists of
 * a collection of Tcl commands plus information about arguments and
 * variables.
 */

typedef struct Proc {
    struct Interp *iPtr;	/* Interpreter for which this command
				 * is defined. */
    char *command;		/* Command that constitutes the body of
				 * the procedure (dynamically allocated). */
    Var *argPtr;		/* Pointer to first in list of variables
				 * giving names to the procedure's arguments.
				 * The order of the variables is the same
				 * as the order of the arguments.  The "value"
				 * fields of the variables are the default
				 * values. */
} Proc;


/*
 * Flag value passed to Tcl_RecordAndEval to request no evaluation
 * (record only).
 */

#define TCL_NO_EVAL		-1

/*
 * Flag values passed to Tcl_Return (see the man page for details):
 */

#define TCL_STATIC	0
#define TCL_DYNAMIC	1
#define TCL_VOLATILE	2
#include "tclint.h"
/*
 * Exported Tcl procedures:
 */

//Var * FindVar(Var **varListPtr,char * varName);
char * index(char * cadena,char  caracter);  // @11 funci�n a�adida para Borland C++
															// borrar en caso de gcc

//void Tcl_CreateCommand(Tcl_Interp * interp,char *  cmdName,int (*proc)(),ClientData clientData,
//                       void (*deleteProc)());
//int Tcl_SplitList(Tcl_Interp * interp, char * list,int * argcPtr,char *** argvPtr);
void Tcl_Return(Tcl_Interp *interp,char *string,int status);
char Tcl_Backslash(char *src, int *readPtr);
//int TclFindElement(Tcl_Interp * interp,register char * list,char **  elementPtr,char ** nextPtr,int * sizePtr
//                    , int * bracePtr);
Var * FindVar(Var ** varListPtr,char * varName);
Var * NewVar(char * name,char * value);
//void      TclCopyAndCollapse(int count, register char * src, register char * dst);
//char *    TclWordEnd(char * start,int  nested);
//int       Tcl_StringMatch(register char *string,register char * pattern);
//char *    Tcl_Merge(int argc,char ** argv);
//char *    Tcl_Concat(int argc, char **argv);
char *    Tcl_GetVar(Tcl_Interp * interp, char * varName,int global);
int       Tcl_Eval(Tcl_Interp *interp,char * cmd,int  flags,char ** termPtr);
void      Tcl_SetVar(Tcl_Interp * interp,char * varName,char *  newValue,int global);
Command * TclFindCmd(Interp * iPtr,char * cmdName,int abbrevOK);
void      Tcl_AddErrorInfo(Tcl_Interp * interp,char * message);
//int       Tcl_RecordAndEval(Tcl_Interp *interp,char * cmd,int flags);
//int       Tcl_BreakCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_CaseCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_CatchCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_ConcatCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_ContinueCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_ErrorCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_EvalCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_ExecCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_ExprCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_FileCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_ForCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_ForeachCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_FormatCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_GlobCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_GlobalCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_IfCmd(ClientData dummy,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_IndexCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_InfoCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_LengthCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_ListCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_PrintCmd(ClientData notUsed,Tcl_Interp * interp,int  argc,char ** argv);
//int       Tcl_ProcCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_RangeCmd(ClientData notUsed,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_RenameCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_ReturnCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_ScanCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_SetCmd(ClientData dummy,register Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_SourceCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_StringCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_TimeCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_UplevelCmd(ClientData dummy,Tcl_Interp * interp,int argc,char ** argv);
//int       Tcl_Expr(Tcl_Interp * interp,char * string,int * valuePtr);
//Proc *    TclFindProc(Interp *iPtr,char * procName);
//Proc *    TclIsProc(Command * cmdPtr);
void      TclDeleteVars(Interp * iPtr);/* Interpreter to nuke. */
void      Tcl_DeleteCommand(Tcl_Interp *interp,char * cmdName);
void      Tcl_DeleteInterp(Tcl_Interp *interp);
// -----------------------------------------
//static void HistoryInit(register Interp * iPtr,int numEvents);
//static void DoRevs(register Interp *iPtr);
//static void MakeSpace(HistoryEvent * hPtr,int size);
//static HistoryEvent * GetEvent(register Interp * iPtr,char * string);
//static void RevCommand(register Interp * iPtr,char * string);
//static void RevResult(register Interp * iPtr, char * string);
//static void DisableRevs(register Interp *iPtr);
//static char * GetWords(register Interp * iPtr,char * command,char * words);
//static int SubsAndEval(register Interp * iPtr,char * cmd,char * old,char * new);
//static void InsertRev(Interp * iPtr,register HistoryRev * revPtr);
//int ExprGetNum(register char * string,register char ** termPtr);
char * Tcl_ParseVar(Tcl_Interp *interp,register char * string,char ** termPtr);
//int ExprLex(Tcl_Interp * interp,register ExprInfo * infoPtr);
//int ExprGetValue(Tcl_Interp * interp,register ExprInfo * infoPtr,int prec);
//long int strtol(char *s,char ** endPtr,int base);
//unsigned long int strtoul(char *string,char ** endPtr,int base);
//void panic(int va_alist);
void panic(char *format);

/*
 * Built-in Tcl command procedures:
 */


/*
 * Miscellaneous declarations (to allow Tcl to be used stand-alone,
 * without the rest of Sprite).
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef _CLIENTDATA
typedef int *ClientData;
#define _CLIENTDATA
#endif

#endif _TCL

