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
 * sprite.h --
 *
 * Common constants and type declarations for Sprite.
 *
 * Copyright 1985, 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/tcl/RCS/sprite.h,v 1.1 93/04/13 13:52:53 cecile Exp $ SPRITE (Berkeley)
 */

#ifndef _SPRITE
#define _SPRITE

/*
 * A boolean type is defined as an integer, not an enum. This allows a
 * boolean argument to be an expression that isn't strictly 0 or 1 valued.
 */

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

/* What is the use of this define ?? Replace it by something to
 * only define Boolean if it is not already defined */		/*AV*/
/* #ifndef _ASM */						/*AV*/
/* cecile modif: the following 3 lines  
 conflict with the definition of Boolean in the X library
#ifndef _BOOL
typedef int Boolean;
#endif
*/
/*
 * Functions that must return a status can return a ReturnStatus to
 * indicate success or type of failure.
 */

typedef int  ReturnStatus;
/* #endif */ /* _ASM */						/*AV*/

/*
 * The following statuses overlap with the first 2 generic statuses 
 * defined in status.h:
 *
 * SUCCESS			There was no error.
 * FAILURE			There was a general error.
 */

#define	SUCCESS			0x00000000
#define	FAILURE			0x00000001


/*
 * A nil pointer must be something that will cause an exception if 
 * referenced.  There are two nils: the kernels nil and the nil used
 * by user processes.
 */

#define NIL 		0xFFFFFFFF
#define USER_NIL 	0
#ifndef NULL
#define NULL	 	0
#endif

/* What is the use of this define ?? */				/*AV*/
/* #ifndef _ASM */						/*AV*/
/*
 * An address is just a pointer in C.  It is defined as a character pointer
 * so that address arithmetic will work properly, a byte at a time.
 */

typedef char *Address;

/*
 * ClientData is an uninterpreted word.  It is defined as an int so that
 * kdbx will not interpret client data as a string.  Unlike an "Address",
 * client data will generally not be used in arithmetic.
 */

#ifndef _CLIENTDATA
typedef int *ClientData;
#define _CLIENTDATA
#endif

#ifndef __STDC__
#define volatile
#define const
#endif
/* #endif */ /* _ASM */						/*AV*/


#endif /* _SPRITE */

