
static char rcsid[] = "$Id: trap.c,v 1.2 93/04/13 23:45:22 cecile Exp $";

/* 
 * trap.c --
 *
 *	This file contains functions that handle various predefined
 *	trap numbers, which are used to perform system calls.
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

/* one possible problem exists below in the Handle_Printf function.  I
 * did not have access to the output of the DLX compiler, so I assumed
 * that the compiler, when passing a floating point value to a function,
 * places the lower floating point register in the lower address (eg.
 * F0 in 0(r14), and F1 in 4(r14)).  If this is not correct, exchange the
 * assignments to ta[0] and ta[1].
 */

/* cecile apr 11 93: some simple traps have been added (math functions). 
 * but they have to be written by hand in the .s files. 
 * This is because dlxcc does not generate them. Chris will fix it.
 * Also, note that there is a problem with some traps  (sqrt, exp)
 * that return a double: 
 * the result is placed in R1 and R2, but superDLX only looks at R1. This
 * will be fixed. So for the moment, the results of math functions are 32
 * bit precision floats.
 */

/* to handle the added traps, one have to add in the .s files:

.global _cos
.global _sin
.global _tan
.global _atan
.global _log

_cos:
	trap #37
	jr r31
	nop
_sin:
	trap #38
	jr r31
	nop
_tan:
	trap #39
	jr r31
	nop
_atan:
	trap #40
	jr r31
	nop
_log:
	trap #41
	jr r31
	nop

*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <math.h>
#include "tcl/tcl.h" //camibado
//#include <sys/errno.h>
#include <io.h>
#include <errno.h>
#include "asm.h"
#include "dlx.h"
#include "sym.h"
#include "traps.h"

#include "varargs.h"

extern int errno;

#define DEFAULT_HEAP_START	0x2000
#define MAX_PRINTF_STRING_SIZE	512	/* Max Chars in printf output */

/*
 *----------------------------------------------------------------------
 *
 * Init_Handle_Trap --
 *
 *	Prepare the system for handling traps.  This mainly makes
 *      copies of the stdin, stdout, and stderr for use by the
 *      simluated program.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The fd_map is set up.
 *
 *----------------------------------------------------------------------
 */

Init_Handle_Trap(machPtr)
DLX *machPtr;
{
  int i;

  for (i = 0; i < FD_SIZE; i++)
    machPtr->fd_map[i] = -1;
  if ((machPtr->fd_map[0] = dup(0)) == -1)
    printf("Could not open simulated standard input\n");
  if ((machPtr->fd_map[1] = dup(1)) == -1)
    printf("Could not open simulated standard output\n");
  if ((machPtr->fd_map[2] = dup(2)) == -1)
    printf("Could not open simulated standard error\n");
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Trap --
 *
 *	Handle the requested trap number.  Any arguments are expected
 *      to start at the address in r14 (the first argument) with other
 *      arguments in words above that in memory.  The result of the
 *      simulated trap is placed in r1.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if the trap is
 *      known, but the trap handler had a problem, or 2 if the trap
 *      number is not known
 *
 * Side effects:
 *      The fd_map or the simulated DLX's memory may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Trap(machPtr, trapNum)
DLX *machPtr;
int trapNum;
{
  switch (trapNum) {
  case TRAP_OPEN :
    return Handle_Open(machPtr);
  case TRAP_FOPEN :
    return Handle_Fopen(machPtr);
  case TRAP_CLOSE :
    return Handle_Close(machPtr);
  case TRAP_FCLOSE :
    return Handle_Fclose(machPtr);
  case TRAP_READ :
    return Handle_Read(machPtr);
  case TRAP_GETS :
    return Handle_Gets(machPtr);
  case TRAP_FGETS :
    return Handle_Fgets(machPtr);
  case TRAP_WRITE :
    return Handle_Write(machPtr);
  case TRAP_PRINTF :
    return Handle_Printf(machPtr);
  case TRAP_SPRINTF :
    return Handle_Sprintf(machPtr);
  case TRAP_FPRINTF :
    return Handle_Fprintf(machPtr);
  case TRAP_MALLOC :
    return Handle_Malloc(machPtr);
  case TRAP_CALLOC :
    return Handle_Calloc(machPtr);
  case TRAP_FREE :
    return Handle_Free(machPtr);
  case TRAP_SQRT :
    return Handle_Sqrt(machPtr);
  case TRAP_EXP :
    return Handle_Exp(machPtr);
  case TRAP_STRCAT :
    return Handle_Strcat(machPtr);
  case TRAP_STRNCAT :
    return Handle_Strncat(machPtr);
  case TRAP_STRDUP :
    return Handle_Strdup(machPtr);
  case TRAP_STRCMP :
    return Handle_Strcmp(machPtr);
  case TRAP_STRNCMP :
    return Handle_Strncmp(machPtr);
  case TRAP_STRICMP :
    return Handle_Stricmp(machPtr);
  case TRAP_STRNICMP :
    return Handle_Strnicmp(machPtr);
  case TRAP_STRCPY :
    return Handle_Strcpy(machPtr);
  case TRAP_STRNCPY :
    return Handle_Strncpy(machPtr);
  case TRAP_STRLEN :
    return Handle_Strlen(machPtr);
  case TRAP_STRCHR :
    return Handle_Strchr(machPtr);
  case TRAP_STRRCHR :
    return Handle_Strrchr(machPtr);
  case TRAP_STRPBRK :
    return Handle_Strpbrk(machPtr);
  case TRAP_STRSPN :
    return Handle_Strspn(machPtr);
  case TRAP_STRCSPN :
    return Handle_Strcspn(machPtr);
  case TRAP_STRSTR :
    return Handle_Strstr(machPtr);
  case TRAP_STRTOK :
    return Handle_Strtok(machPtr);
  case TRAP_STRTOD :
    return Handle_Strtod(machPtr);
  case TRAP_STRTOL :
    return Handle_Strtol(machPtr);
  case TRAP_STPCPY :
    return Handle_Stpcpy(machPtr);

/* traps added by cecile: have to be written in the .s file*/
    case TRAP_COS:
    	return Handle_Cos(machPtr);
    case TRAP_SIN:
     	return Handle_Sin(machPtr);	
    case TRAP_TAN:
	return Handle_Tan(machPtr);
    case TRAP_ATAN:
	return Handle_Atan(machPtr);
    case TRAP_LOG:
	return Handle_Log(machPtr);

  default :
    if (trapNum) printf ("Unimplemented Trap (%d)\n", trapNum);
    return 2;
  }
}

/*
 *----------------------------------------------------------------------
 *
 * Block_To_Scratch --
 *
 *	Copy a block of memory from the simulated memory to the scratch
 *      pad.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The memScratch is altered.
 *
 *----------------------------------------------------------------------
 */

int Block_To_Scratch(machPtr, firstAddr, length)
DLX *machPtr;
register unsigned firstAddr, length;
{
  unsigned char *first, *last, bytes[4];
  register unsigned int word;
  MemWord *wordPtr;

  if (!length)
    return 0;
  length--;
  if (length < 0) {
    printf("Invalid block movement\n");
    return 1;
  }
  if ((firstAddr < 0) || (firstAddr + length >= machPtr->numChars)) {
    printf("Memory reference out of range\n");
    return 1;
  }

  first = (unsigned char *) SCRATCH_ADDR(firstAddr);
  last = first + length;
  wordPtr = machPtr->memPtr + ADDR_TO_INDEX(firstAddr);
  word = wordPtr->value;
  bytes[3] = (word & 0xff);
  bytes[2] = ((word >>= 8) & 0xff);
  bytes[1] = ((word >>= 8) & 0xff);
  bytes[0] = ((word >> 8) & 0xff);
  while (first <= last) {
    *first = bytes[firstAddr & 0x3];
    firstAddr++; first++;
    if (!(firstAddr & 0x3)) {
      word = (++wordPtr)->value;
      bytes[3] = (word & 0xff);
      bytes[2] = ((word >>= 8) & 0xff);
      bytes[1] = ((word >>= 8) & 0xff);
      bytes[0] = ((word >> 8) & 0xff);
    }
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Block_From_Scratch --
 *
 *	Copy a block of memory from the scratch pad to the simulated
 *      memory.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The memPtr data is altered.
 *
 *----------------------------------------------------------------------
 */

int Block_From_Scratch(machPtr, firstAddr, length)
DLX *machPtr;
register unsigned firstAddr, length;
{
  unsigned char *first, *last;
  register unsigned int word;
  MemWord *wordPtr;

  if (!length)
    return 0;
  length--;
  if (length < 0) {
    printf("Invalid block movement\n");
    return 1;
  }
  if ((firstAddr < 0) || (firstAddr + length >= machPtr->numChars)) {
    printf("Memory reference out of range\n");
    return 1;
  }

  wordPtr = machPtr->memPtr + ADDR_TO_INDEX(firstAddr);
  word = wordPtr->value;
  first = (unsigned char *) SCRATCH_ADDR(firstAddr);
  last = first + length;
  while (first <= last) {
    switch (firstAddr & 0x3) {
    case 0 :
      word = (word & 0x00ffffff) | (*first << 24);
      break;
    case 1 :
      word = (word & 0xff00ffff) | (*first << 16);
      break;
    case 2 :
      word = (word & 0xffff00ff) | (*first << 8);
      break;
    case 3 :
      word = (word & 0xffffff00) | *first;
      wordPtr->value = word;
      wordPtr->opCode = OP_NOT_COMPILED;
      word = (++wordPtr)->value;
      break;
    }
    firstAddr++; first++;
  }
  if (wordPtr->value != word) {
    wordPtr->value = word;    /* update the last word */
    wordPtr->opCode = OP_NOT_COMPILED;
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * String_To_Scratch --
 *
 *	Copy a NULL terminated string from simulator memory to the
 *      scratch pad.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The memScratch data is altered.
 *
 *----------------------------------------------------------------------
 */

int String_To_Scratch(machPtr, firstAddr)
DLX *machPtr;
register int firstAddr;
{
  char *first, *endMem = machPtr->endScratch;
  register unsigned int word;
  register int value;
  unsigned char bytes[4];
  MemWord *wordPtr;

  if ((firstAddr < 0) || (firstAddr >= machPtr->numChars)) {
    printf("Memory reference out of range\n");
    return 1;
  }

  first = SCRATCH_ADDR(firstAddr);
  wordPtr = machPtr->memPtr + ADDR_TO_INDEX(firstAddr);
  word = wordPtr->value;
  bytes[3] = (word & 0xff);
  bytes[2] = ((word >>= 8) & 0xff);
  bytes[1] = ((word >>= 8) & 0xff);
  bytes[0] = ((word >> 8) & 0xff);
  while (first < endMem) {
    if (!(*first = bytes[firstAddr & 0x3]))
      break;
    firstAddr++; first++;
    if (!(firstAddr & 0x3)) {
      word = (++wordPtr)->value;
      bytes[3] = (word & 0xff);
      bytes[2] = ((word >>= 8) & 0xff);
      bytes[1] = ((word >>= 8) & 0xff);
      bytes[0] = ((word >> 8) & 0xff);
    }
  }

  if (first >= endMem) {
    printf("Memory reference out of range\n");
    return 1;
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Open --
 *
 *	Handle the open system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The fd_map may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Open(machPtr)
DLX *machPtr;
{
  int where, path, result;

  for (where = 0; where < FD_SIZE; where++)
    if (machPtr->fd_map[where] == -1)
      break;
  if (where == FD_SIZE) {    /* no free file descriptors */
    Set_Errno(machPtr, EMFILE);
    RETURN_VALUE = -1;
    return 0;
  }
  path = ARG(0);
  if (String_To_Scratch(machPtr, path))
    return 1;
  result = open(SCRATCH_ADDR(path), ARG(1), ARG(2));
  if (result == -1) {
    Set_Errno(machPtr, errno);
    RETURN_VALUE = -1;
  } else {
    machPtr->fd_map[where] = result;
    RETURN_VALUE = where;
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Fopen --
 *
 *	Handle the fopen system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch space may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Fopen(machPtr)
DLX *machPtr;
{
  int  path, mode;
  FILE *fp;

  path = ARG(0);
  if (String_To_Scratch(machPtr, path))
    return 1;

  mode = ARG(1);
  if (String_To_Scratch(machPtr, mode))
    return 1;

  fp = fopen(SCRATCH_ADDR(path), SCRATCH_ADDR(mode));

  if (fp == 0) {
    Set_Errno(machPtr, errno);
    RETURN_VALUE = 0;
  }
  else RETURN_VALUE = (int) fp;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Close --
 *
 *	Handle the close system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The fd_map may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Close(machPtr)
DLX *machPtr;
{
  int result, fd;

  fd = ARG(0);
  if ((fd < 0) || (fd > FD_SIZE) || (machPtr->fd_map[fd] == -1)) {
    /* illegal file descriptor */
    Set_Errno(machPtr, EBADF);
    RETURN_VALUE = -1;
    return 0;
  }
  result = close(machPtr->fd_map[fd]);
  if (result == -1) {
    Set_Errno(machPtr, errno);
    RETURN_VALUE = -1;
  } else {
    RETURN_VALUE = result;
    machPtr->fd_map[fd] = -1;
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Fclose --
 *
 *	Handle the fclose system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Fclose(machPtr)
DLX *machPtr;
{
  fclose ((FILE*)(ARG(0)));

  RETURN_VALUE = 0;
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Read --
 *
 *	Handle the read system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The simulator memory and scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Read(machPtr)
DLX *machPtr;
{
  int result, fd, buf, nbytes;

  fd = ARG(0);
  buf = ARG(1);
  nbytes = ARG(2);
  if ((fd < 0) || (fd > FD_SIZE) || (machPtr->fd_map[fd] == -1)) {
    /* illegal file descriptor */ 
    Set_Errno(machPtr, EBADF);
    RETURN_VALUE = -1;
    return 0;
  }
  if ((buf < 0) || (buf >= machPtr->numChars)) {
    printf("Memory reference out of range\n");
    return 1;
  }
  if ((nbytes < 0) || (buf + nbytes - 1 >= machPtr->numChars)) {
    printf("Memory reference out of range\n");
    return 1;
  }
  result = read(machPtr->fd_map[fd], SCRATCH_ADDR(buf), nbytes);
  if (result == -1) {
    Set_Errno(machPtr, errno);
    RETURN_VALUE = -1;
  } else {
    if (Block_From_Scratch(machPtr, buf, result))
      return 1;
    RETURN_VALUE = result;
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Write --
 *
 *	Handle the write system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Write(machPtr)
DLX *machPtr;
{
  int result, fd, buf, nbytes;

  fd = ARG(0);
  buf = ARG(1);
  nbytes = ARG(2);
  if ((fd < 0) || (fd > FD_SIZE) || (machPtr->fd_map[fd] == -1)) {
    /* illegal file descriptor */
    Set_Errno(machPtr, EBADF);
    RETURN_VALUE = -1;
    return 0;
  }
  if (Block_To_Scratch(machPtr, buf, nbytes))
    return 1;
  result = write(machPtr->fd_map[fd], SCRATCH_ADDR(buf), nbytes);
  if (result == -1) {
    Set_Errno(machPtr, errno);
    RETURN_VALUE = -1;
  } else {
    RETURN_VALUE = result;
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Gets --
 *
 *	Handle the gets system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Gets(machPtr)
DLX *machPtr;
{
  char *s;

  s = gets (SCRATCH_ADDR (ARG(0)));

  if (s == SCRATCH_ADDR (ARG(0))) {
     if (Block_From_Scratch(machPtr, ARG(0), strlen(s)+1))
         return 1;

     RETURN_VALUE = ARG(0);
  }
  else RETURN_VALUE = 0;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Fgets --
 *
 *	Handle the fgets system call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Fgets(machPtr)
DLX *machPtr;
{
  char *s;

  char *scratch;
  int arg0,arg1,arg2;
/*
  s = fgets (SCRATCH_ADDR (ARG(0)), ARG(1), ARG(2));
*/
  arg0 = machPtr->memPtr[ADDR_TO_INDEX(machPtr->regs[14])+0].value;
  arg1 = machPtr->memPtr[ADDR_TO_INDEX(machPtr->regs[14])+1].value;
  arg2 = machPtr->memPtr[ADDR_TO_INDEX(machPtr->regs[14])+2].value;

   scratch = ((char *)machPtr->memScratch +0);
  s = fgets (SCRATCH_ADDR (arg0), arg1,(FILE*) arg2);

  if (s == SCRATCH_ADDR (ARG(0))) {
     if (Block_From_Scratch(machPtr, ARG(0), strlen(s)+1))
         return 1;

     RETURN_VALUE = ARG(0);
  }
  else RETURN_VALUE = 0;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Printf --
 *
 *	Handle the printf system call.  This is done by calling the
 *	generic Xprintf routine.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Printf(machPtr)
DLX *machPtr;
{
  char dest[MAX_PRINTF_STRING_SIZE];

  Handle_Xprintf (machPtr, dest, 1);

  printf (dest);
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Sprintf --
 *
 *	Handle the sprintf system call.  This is done by calling the
 *	generic Xprintf routine.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Sprintf(machPtr)
DLX *machPtr;
{
  char *s = SCRATCH_ADDR (ARG(0));

  if (Handle_Xprintf (machPtr, s, 2) == 0) {
     if (Block_From_Scratch(machPtr, ARG(0), strlen(s)+1))
         return 1;

     RETURN_VALUE = ARG(0);
  }
  else RETURN_VALUE = 0;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Fprintf --
 *
 *	Handle the fprintf system call.  This is done by calling the
 *	generic Xprintf routine.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Fprintf(machPtr)
DLX *machPtr;
{
  char dest[MAX_PRINTF_STRING_SIZE];
  FILE *fp;

  fp = (FILE *) ARG(0);

  Handle_Xprintf (machPtr, dest, 2);

  fprintf (fp, dest);
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Xprintf --
 *
 *	Handle printf, sprintf, and fprintf system calls.  This is done
 *	by scanning through the format string provided, and sending each
 *	% command and following plain text to the sprintf library function
 *	(with the appropriate argument).
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      The scratch pad may be changed.
 *
 *----------------------------------------------------------------------
 */

int Handle_Xprintf(machPtr, dest, argNumber)
DLX *machPtr;
char *dest;
int  argNumber;
{
  char *start, *end, *s, temp;
  int fmt = ARG(argNumber-1), ta[2], loc, done, result = 0;
  double *pd = (double *)&(ta[0]);

  /* convert the format string */
  if (String_To_Scratch(machPtr, fmt))
    return 1;

  /* now break the format string into blocks of characters beginning with
   * either the start of the format string, or a % command, and continuing
   * through either the start of the next % command, or the end of the
   * string.  */
  for (start = SCRATCH_ADDR(fmt); *start; start = end) {
    if (*start == '%') {
      if (start[1] == '%')
	end = start + 2;
      else
	end = start + 1;
    } else
      end = start;
    for ( ; *end && (*end != '%'); end++)     /* find the end of this block */
      ;
    temp = *end;
    *end = '\0';
    if (*start != '%')
      sprintf(dest, start);
    else {
      /* find the type of argument this block takes (if any) */
      for (done = 0, s = start + 1; *s && !done; s++)
	switch (*s) {
	case 'c' :
	case 'd' :
	case 'o' :
	case 'u' :
	case 'x' :
	case 'X' :
	  sprintf(dest, start, ARG(argNumber++));
	  done = 1;
	  break;
	case 'f' :
	case 'e' :
	case 'E' :
	case 'g' :
	case 'G' :
	  ta[0] = ARG(argNumber++);
	  ta[1] = ARG(argNumber++);
	  sprintf(dest, start, *pd);
	  done = 1;
	  break;
	case 's' :
	  loc = ARG(argNumber++);
	  if (String_To_Scratch(machPtr, loc))
	    return 1;
	  sprintf(dest, start, SCRATCH_ADDR(loc));
	  done = 1;
	  break;
	case '%' :
	  sprintf(dest, start);
	  done = 1;
	  break;
	}
      if (!done)    /* no % command found */
	sprintf(dest, start);
    }
    *end = temp;
    while (*dest) {dest++; result++;}
  }
  RETURN_VALUE = result;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Malloc --
 *
 *	Handle the malloc system call.
 *
 * Results:
 *	The return value is 0 if no error ocured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The memScratch data is altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Malloc (machPtr)
DLX *machPtr;
{
  unsigned pmem;

  pmem = do_malloc (machPtr, ARG(0));

  if (pmem) RETURN_VALUE = pmem - (unsigned) machPtr->memScratch;
  else RETURN_VALUE = pmem;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Calloc --
 *
 *	Handle the calloc system call.
 *
 * Results:
 *	The return value is 0 if no error ocured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The memScratch data is altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Calloc (machPtr)
DLX *machPtr;
{
  char	   *pmem;
  unsigned i;
  unsigned numbytes = ARG(0) * ARG(1);

  pmem = (char *) do_malloc (machPtr, numbytes);

  if (!pmem) RETURN_VALUE = (int) pmem;
  else {
    RETURN_VALUE = (int) (pmem - machPtr->memScratch);

    for (i = 0; i < numbytes; i++)
      *pmem++ = 0;

    if (Block_From_Scratch(machPtr, RETURN_VALUE, numbytes))
      return 1;
  }

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Free --
 *
 *	Handle the free system call.
 *
 * Results:
 *	The return value is 0 if no error ocured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The memScratch data is altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Free (machPtr)
DLX *machPtr;
{
  do_free (machPtr, SCRATCH_ADDR (ARG(0)));

  RETURN_VALUE = 0;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * check_malloc_init
 *
 *	TEMPORARY routine to initialize system for allocating memory.
 *	If this has already been done, check_malloc_init does nothing.
 *	This routine will be replaced by one which is called whenever
 *	a program is loaded, and the start address of the heap will be
 *	immediately after the last byte loaded.
 *
 * Results:
 *	The return value is 0 if no error ocured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The memScratch data is altered.
 *
 *----------------------------------------------------------------------
 */

check_malloc_init (machPtr)
DLX *machPtr;
{

  static malloc_init_done;

  if (!malloc_init_done) {
    malloc_init_done = TRUE;

    init_malloc (machPtr, SCRATCH_ADDR (DEFAULT_HEAP_START),
    	(machPtr->endScratch - machPtr->memScratch) - DEFAULT_HEAP_START);
  }
}

/*
 *----------------------------------------------------------------------
 *
 * do_malloc_init
 *
 *	Routine to initialize heap for malloc/calloc.  It is passed
 *	the start address of the heap, which is generally the first
 *	address after the program and data.  If the address passed
 *	is 0, a default value is used for the heap start.  Heap is
 *	assumed to continue to the end of memory.  Note that the
 *	stack begins at the end of memory and works down, so it is
 *	possible for the heap to grow up into the stack or the
 *	stack to grow down into the heap.
 *
 * Results:
 *	The return value is 0 if no error ocured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The memScratch data is altered.
 *
 *----------------------------------------------------------------------
 */

do_malloc_init (machPtr, addr)
DLX	 *machPtr;
unsigned addr;
{
  if (!addr) addr = (unsigned) SCRATCH_ADDR (DEFAULT_HEAP_START);
  else addr = (unsigned) SCRATCH_ADDR (addr);

  if (init_malloc (machPtr, addr, machPtr->endScratch - addr))
    return 1;

  printf ("Heap (for malloc) begins at 0x%X\n",
	  addr - (unsigned) machPtr->memScratch);

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Sqrt --
 *
 *	Handle the square root call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Sqrt(machPtr)
DLX *machPtr;
{
  double result;
  double sqrt ();

  errno = 0;
  result = sqrt (ARG(0));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
/* carefull: dlxcc only uses the result placed in R1!! so superdlx also
   ..this will be fixed*/
    RETURN_DVALUE_L = *((long *) &result);
    RETURN_DVALUE_H = *(((long *) &result)+1);
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Exp --
 *		    x
 *	Handle the e  call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Exp(machPtr)
DLX *machPtr;
{
  double result;
  double exp ();

  errno = 0;
  result =  (exp (ARG(0)));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
 /* carefull: dlxcc only uses the result placed in R1!! so does superdlx. 
   this will be fixed*/
   RETURN_DVALUE_L =*((long *) &result) ;
  RETURN_DVALUE_H = *(((long *) &result)+1);
  }
  return 0;
}
/*
 *----------------------------------------------------------------------
 *
 * Handle_Cos --
 *		    
 *	Handle the cosinus call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Cos(machPtr)
DLX *machPtr;
{
  double result;
  double cos ();

  errno = 0;
  result = (double) cos (ARG(0));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
 /*superdlx only uses the result placed in R1!! ;
   this will be fixed*/
    RETURN_DVALUE_L = *((long *) &result); /* r1*/
    RETURN_DVALUE_H = *(((long *) &result)+1); /* r2*/
  }
  return 0;
}
/*
 *----------------------------------------------------------------------
 *
 * Handle_Sin --
 *		    
 *	Handle the sinus call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Sin(machPtr)
DLX *machPtr;
{
  double result;
  double sin ();

  errno = 0;
  result = (double) sin (ARG(0));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
 /* carefull: superdlx only uses the result placed in R1!!
   this will be fixed*/
    RETURN_DVALUE_L = *((long *) &result); /* r1*/
    RETURN_DVALUE_H = *(((long *) &result)+1); /* r2*/
  }
  return 0;
}
/*
 *----------------------------------------------------------------------
 *
 * Handle_Tan --
 *		    
 *	Handle the tangent call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Tan(machPtr)
DLX *machPtr;
{
  double result;
  double tan ();

  errno = 0;
  result = (double) tan (ARG(0));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
 /* carefull: superdlx only uses the result placed in R1!! ;
   this will be fixed*/
    RETURN_DVALUE_L = *((long *) &result); /* r1*/
    RETURN_DVALUE_H = *(((long *) &result)+1); /* r2*/
  }
  return 0;
}
/*
 *----------------------------------------------------------------------
 *
 * Handle_Atan --
 *		    
 *	Handle the atan call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Atan(machPtr)
DLX *machPtr;
{
  double result;
  double atan();

  errno = 0;
  result = atan(ARG(0));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
 /* carefull: superdlx only uses the result placed in R1!! ;
   this will be fixed*/
    RETURN_DVALUE_L = *((long *) &result); /* r1*/
   RETURN_DVALUE_H = *(((long *) &result)+1); /* r2*/
  }
  return 0;
}
/*
 *----------------------------------------------------------------------
 *
 * Handle_Log --
 *		    
 *	Handle the log call.
 *
 * Results:
 *	The return value is 0 if no error occured, 1 if a problem
 *      arises.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int Handle_Log(machPtr)
DLX *machPtr;
{
  double result;
  double atan();

  errno = 0;
  result = log(ARG(0));
  if (errno) {
    Set_Errno(machPtr, errno);
    RETURN_DVALUE_L = 0;
    RETURN_DVALUE_H = 0;
  }
  else {
 /* carefull: superdlx only uses the result placed in R1!!;
   this will be fixed*/
    RETURN_DVALUE_L = *((long *) &result); /* r1*/
    RETURN_DVALUE_H = *(((long *) &result)+1); /* r2*/
  }
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Set_Errno
 *
 *	Set the _error variable (if it exists)
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      See the description above
 *
 *----------------------------------------------------------------------
 */

Set_Errno(machPtr, errorNumber)
DLX *machPtr;
int errorNumber;
{
  char *p, *end;
  int addr, index;

  p = Tcl_GetVar(machPtr->interp, "_errno", 0);
  if (*p != 0) {
    addr = strtoul(p, &end, 0);
    if (*end != 0)
      return;
    index = ADDR_TO_INDEX(addr);
    if (index < machPtr->numWords)
      machPtr->memPtr[index].value = errorNumber;
  }
}
