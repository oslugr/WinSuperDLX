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
 * stdlib.h --
 *
 *	Declares facilities exported by the "stdlib" portion of
 *	the C library.
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
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/tcl/RCS/stdlib.h,v 1.1 93/04/13 13:52:54 cecile Exp $ SPRITE (Berkeley)
 */

#ifndef _STDLIB
#define _STDLIB

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

/*
 *----------------------------
 * String conversion routines:
 *----------------------------
 */

extern double		atof();
extern int		atoi();
extern long int		atol();
extern double		strtod();
extern long int		strtol();
extern unsigned long	strtoul();

/*
 *------------------
 * Memory allocator:
 *------------------
 */

extern char *	alloca();
extern char *	calloc();
extern char *	malloc();
extern char *	realloc();
extern void	Mem_Bin();
extern char *	Mem_CallerPC();
extern void	Mem_DumpTrace();
extern void	Mem_PrintConfig();
extern void	Mem_PrintInUse();
extern void	Mem_PrintStats();
extern void	Mem_PrintStatsInt();
extern void	Mem_SetPrintProc();
extern void	Mem_SetTraceSizes();
extern int	Mem_Size();

/*
 * The mips compiler cannot handle some coercions on the left hand side
 */
#if defined(KERNEL) && !defined(mips)
extern 		_free();

#ifdef lint
#define		free(ptr) _free(ptr)
#else
#define		free(ptr) {_free(ptr); (ptr) = (char *) NIL; }
#endif /* lint */

#else
extern		free();
#endif /* KERNEL */

/*
 * Structure used to set up memory allocation traces.
 */

typedef struct {
    int		size;	/* Size of block to trace. */
    int		flags;	/* Flags defined below */
} Mem_TraceInfo;

/*
 * Flags to determine what type of tracing to do.
 *
 *	MEM_PRINT_TRACE		A trace record will be printed each time that
 *				an object of this size is alloc'd or freed.
 *	MEM_STORE_TRACE		The number of blocks in use by each caller
 *				up to a predefined maximum number of callers
 *				is kept in a trace array .
 *	MEM_DONT_USE_ORIG_SIZE	Don't use the original size for tracing, but use
 *				the modified size used by malloc.
 *	MEM_TRACE_NOT_INIT	The trace records stored for MEM_STORE_TRACE
 *				have not been initialized yet.
 */

#define	MEM_PRINT_TRACE		0x1
#define	MEM_STORE_TRACE		0x2
#define	MEM_DONT_USE_ORIG_SIZE	0x4
#define	MEM_TRACE_NOT_INIT	0x8

extern int	mem_SmallMinNum;
extern int	mem_LargeMinNum;
extern int	mem_LargeMaxSize;

/*
 * Statistics counters;  only incremented when tracing is enabled.
 */

extern int	mem_NumAllocs;
extern int	mem_NumFrees;

/*
 *----------------------------------------------------------------
 * Additional integer math routines, plus structures for returning
 * results from them:
 *----------------------------------------------------------------
 */

typedef struct div_t {
    int quot;
    int rem;
} div_t;

typedef struct {
    long int quot;
    long int rem;
} ldiv_t;

extern int	abs();
extern div_t	div();
extern long int	labs();
extern ldiv_t	ldiv();

/*
 *-----------------------------------
 * Miscellaneous additional routines:
 *-----------------------------------
 */

extern void	abort();
extern int	atexit();
extern char *   bsearch();
extern 		exit();
extern char *	getenv();
extern void	qsort();
extern int	rand();
extern long	random();
extern void	setenv();
extern 		srand();
extern		srandom();
extern int	system();

#endif /* _STDLIB */

