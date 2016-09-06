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
 * tclInt.h --
 *
 *	Declarations of things used internally by the Tcl interpreter.
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
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/tcl/RCS/tclInt.h,v 1.1 93/04/13 13:53:07 cecile Exp $ SPRITE (Berkeley)
 */

#ifndef _TCLINT
#define _TCLINT 1

#ifndef _TCL
#include "tcl.h"
#endif

/*
 * The structure below defines one Tcl command, by associating a procedure
 * with a textual string.
 */

#define CMD_SIZE(nameLength) ((unsigned) sizeof(Command) + nameLength - 3)

/*
 * The structure below defines a variable, which associates a string name
 * with a string value.  To cut down on the number of malloc's and free's
 * (particularly for procedure parameters), space for both the variable's
 * name and initial value is allocated at the end of the structure (in
 * "storage").  If the variable's value changes later, a new dynamic
 * string is allocated, if there is insufficient space in the current
 * storage area.
 */

#define VAR_SIZE(nameLength, valueLength) \
	((unsigned) sizeof(Var) + nameLength + valueLength - 2)

/*
 * Variable flags:
 *
 * VAR_DYNAMIC:		1 means the storage space for the value was
 *			dynamically allocated, and must eventually be
 *			freed.
 * VAR_GLOBAL:		Used only in local variables.  Means that this
 *			is really a global variable.
 */

#define VAR_DYNAMIC	1
#define VAR_GLOBAL	2


/*
 * The structure below defines a frame, which is a procedure invocation.
 * These structures exist only while procedures are being executed, and
 * provide a sort of call stack.
 */

/*
 * This structure defines an interpreter, which is a collection of commands
 * plus other state information related to interpreting commands, such as
 * variable storage.  The lists of commands and variables are sorted by usage:
 * each time a command or variable is used it is pulled to the front of its
 * list.
 */


/*
 * Flag bits for Interp structures:
 *
 * DELETED:		Non-zero means the interpreter has been deleted:
 *			don't process any more commands for it, and destroy
 *			the structure as soon as all nested invocations of
 *			Tcl_Eval are done.
 */

#define DELETED 1

/*
 * Additional flags passed to Tcl_Eval.  See tcl.h for other flags to
 * Tcl_Eval;  these ones are only used internally by Tcl.
 *
 * TCL_RECORD_BOUNDS	Tells Tcl_Eval to record information in the
 *			evalFirst and evalLast fields for each command
 *			executed directly from the string (top-level
 *			commands and those from command substitution).
 */

#define TCL_RECORD_BOUNDS	0x100

/*
 * Maximum number of levels of nesting permitted in Tcl commands.
 */

#define MAX_NESTING_DEPTH	100

/*
 * Procedures shared among Tcl modules but not used by the outside
 * world:
 */

extern void		TclCopyAndCollapse();
extern void		TclDeleteVars();
extern Command *	TclFindCmd();
extern int		TclFindElement();
extern Proc *		TclFindProc();
extern Proc *		TclIsProc();
extern char *		TclWordEnd();

#endif _TCLINT

