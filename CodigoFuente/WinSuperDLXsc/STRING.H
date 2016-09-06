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
 * string.h --
 *
 *	Declarations of ANSI C library procedures for string handling.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/tcl/RCS/string.h,v 1.1 93/04/13 13:52:56 cecile Exp $ SPRITE (Berkeley)
 */

#ifndef _STRING
#define _STRING

extern char *	memchr();
extern int	memcmp();
extern char *	memcpy();
extern char *	memset();

extern char *	strcat();
extern char *	strchr();
extern int	strcmp();
extern char *	strcpy();
extern int	strcspn();
extern char *	strerror();
extern int	strlen();
extern char *	strncat();
extern int	strncmp();
extern char *	strncpy();
extern char *	strpbrk();
extern char *	strrchr();
extern int	strspn();
extern char *	strstr();
extern char *   strtok();

/*
 * Obsolete library procedures from BSD, supported for compatibility:
 */

extern char	*index();
extern char	*rindex();

#endif /* _STRING */

