
static char rcsid[] = "$Id: strtrap.c,v 1.2 93/04/13 23:44:37 cecile Exp $";

/* 
 * strtrap.c --
 *
 *	This file contains functions that handle various predefined
 *	trap numbers, which are used to perform system calls for
 *	string functions.
 *
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include "tcl/tcl.h"
//#include <sys/errno.h>
#include "asm.h"
#include "dlx.h"
#include "sym.h"
#include "traps.h"

/*
 *----------------------------------------------------------------------
 *
 * String_From_Scratch --
 *
 *	Copy a NULL terminated string to simulator memory from the
 *	scratch pad.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator memory is altered.
 *
 *----------------------------------------------------------------------
 */

String_From_Scratch (machPtr, firstAddr)
DLX *machPtr;
unsigned firstAddr;
{
  unsigned char *first;
  unsigned char *endMem = (unsigned char *) machPtr->endScratch;
  unsigned int  word;
  unsigned char byte;
  MemWord	*wordPtr;

  if ((firstAddr < 0) || (firstAddr >= machPtr->numChars)) {
    printf ("Memory reference out of range\n");
    return 1;
  }

  first = (unsigned char *) SCRATCH_ADDR(firstAddr);
  wordPtr = machPtr->memPtr + ADDR_TO_INDEX(firstAddr);
  word = wordPtr->value;
  while (first < endMem) {
    byte = *first;
    switch (firstAddr & 0x03) {
      case 0:
        word = (word & 0x00ffffff) | (byte << 24);
        break;
      case 1:
        word = (word & 0xff00ffff) | (byte << 16);
        break;
      case 2:
        word = (word & 0xffff00ff) | (byte << 8);
        break;
      case 3:
        word = (word & 0xffffff00) | byte;
	wordPtr->value = word;
	wordPtr->opCode = OP_NOT_COMPILED;
	word = (++wordPtr)->value;
        break;
    }
    if (!byte) break;
    firstAddr++; first++;
  }

  if (wordPtr->value != word) {
    wordPtr->value = word;	/* Update the last word */
    wordPtr->opCode = OP_NOT_COMPILED;
  }

  if (first >= endMem) {
    printf ("Memory reference out of range\n");
    return 1;
  }

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Stpcpy --
 *
 *	Handle the stpcpy system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Stpcpy (machPtr)
DLX *machPtr;
{
  char *p_src, *p_dest;

  String_To_Scratch (machPtr, ARG(1));

  p_src = SCRATCH_ADDR (ARG(1));
  p_dest = SCRATCH_ADDR (ARG(0));
  while (*p_src) {
    *p_dest++ = *p_src++;
  }

  Block_From_Scratch (machPtr, ARG(0), strlen (SCRATCH_ADDR (ARG(1))));

  RETURN_VALUE = p_dest - machPtr->memScratch;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strcat --
 *
 *	Handle the strcat system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strcat (machPtr)
DLX *machPtr;
{
  int len;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  len = strlen(SCRATCH_ADDR (ARG(0)));
  strcat (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));
  String_From_Scratch (machPtr, ARG(0) + len);

  RETURN_VALUE = ARG(0);

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strchr --
 *
 *	Handle the strchr/index system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strchr (machPtr)
DLX *machPtr;
{
  char *pos;

  String_To_Scratch (machPtr, ARG(0));
  pos = strchr (SCRATCH_ADDR (ARG(0)), ARG(1));

  if (pos) RETURN_VALUE = pos - machPtr->memScratch;
  else RETURN_VALUE = NULL;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strcmp --
 *
 *	Handle the strcmp system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strcmp (machPtr)
DLX *machPtr;
{
  int cmp;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  cmp = strcmp (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));

  RETURN_VALUE = cmp;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strcpy --
 *
 *	Handle the strcpy system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strcpy (machPtr)
DLX *machPtr;
{
  char *p_ret;

  String_To_Scratch (machPtr, ARG(1));
  p_ret = strcpy (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));
  String_From_Scratch (machPtr, ARG(0));

  RETURN_VALUE = p_ret - machPtr->memScratch;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strcspn --
 *
 *	Handle the strcspn system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strcspn (machPtr)
DLX *machPtr;
{
  int size;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  size = strcspn (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));

  RETURN_VALUE = size;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strdup --
 *
 *	Handle the strdup system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strdup (machPtr)
DLX *machPtr;
{
  char *p_ret, *p_tmp;
  char *p_orig = SCRATCH_ADDR (ARG(0));
  char *do_malloc ();

  String_To_Scratch (machPtr, ARG(0));
  p_ret = do_malloc (machPtr, strlen (p_orig) + 1);
  if (!p_ret) {
    printf ("Could not get memory in strdup\n");
    return 1;
  }

  p_tmp = p_ret;
  do {
    *p_tmp++ = *p_orig;
  } while (*p_orig++);

  RETURN_VALUE = p_ret - machPtr->memScratch;

  String_From_Scratch (machPtr, RETURN_VALUE);

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Stricmp --
 *
 *	Handle the stricmp/strcmpi/strcasecmp system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Stricmp (machPtr)
DLX *machPtr;
{
  int cmp;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  cmp = strcmp (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));

  RETURN_VALUE = cmp;

  return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * Handle_Strlen --
 *
 *	Handle the strlen system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strlen (machPtr)
DLX *machPtr;
{
  int len;

  String_To_Scratch (machPtr, ARG(0));
  len = strlen (SCRATCH_ADDR (ARG(0)));

  RETURN_VALUE = len;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strncat --
 *
 *	Handle the strncat system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strncat (machPtr)
DLX *machPtr;
{
  int  len;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  len = strlen(SCRATCH_ADDR (ARG(0)));
  strncat (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)), ARG(2));
  String_From_Scratch (machPtr, ARG(0) + len);

  RETURN_VALUE = ARG(0);

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strncmp --
 *
 *	Handle the strncmp system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strncmp (machPtr)
DLX *machPtr;
{
  int cmp;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  cmp = strncmp (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)), ARG(2));

  RETURN_VALUE = cmp;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strncpy --
 *
 *	Handle the strncpy system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strncpy (machPtr)
DLX *machPtr;
{
  char *p_ret;

  String_To_Scratch (machPtr, ARG(1));
  p_ret = strncpy (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)), ARG(2));
  Block_From_Scratch (machPtr, ARG(0), ARG(2));

  RETURN_VALUE = p_ret - machPtr->memScratch;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strnicmp --
 *
 *	Handle the strnicmp/strncasecmp system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strnicmp (machPtr)
DLX *machPtr;
{
  int cmp;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  cmp = strncmp (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)), ARG(2));

  RETURN_VALUE = cmp;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strpbrk --
 *
 *	Handle the strpbrk system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strpbrk (machPtr)
DLX *machPtr;
{
  char *p_ret;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  p_ret = strpbrk (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));

  if (p_ret) RETURN_VALUE = p_ret - machPtr->memScratch;
  else RETURN_VALUE = NULL;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strrchr --
 *
 *	Handle the strrchr/rindex system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strrchr (machPtr)
DLX *machPtr;
{
  char *p_ret;

  String_To_Scratch (machPtr, ARG(0));
  p_ret = strrchr (SCRATCH_ADDR (ARG(0)), ARG(1));

  if (p_ret) RETURN_VALUE = p_ret - machPtr->memScratch;
  else RETURN_VALUE = NULL;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strspn --
 *
 *	Handle the strspn system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strspn (machPtr)
DLX *machPtr;
{
  int size;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  size = strspn (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));

  RETURN_VALUE = size;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strstr --
 *
 *	Handle the strstr system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strstr (machPtr)
DLX *machPtr;
{
  char *p_ret;

  String_To_Scratch (machPtr, ARG(0));
  String_To_Scratch (machPtr, ARG(1));
  p_ret = strstr (SCRATCH_ADDR (ARG(0)), SCRATCH_ADDR (ARG(1)));

  if (p_ret) RETURN_VALUE = p_ret - machPtr->memScratch;
  else RETURN_VALUE = NULL;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strtod --
 *
 *	Handle the strtod system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strtod (machPtr)
DLX *machPtr;
{
  double val;
  char **stop;

  String_To_Scratch (machPtr, ARG(0));

  if (ARG(1)) stop = (char **) SCRATCH_ADDR (ARG(1));
  else stop = NULL;

  val = strtod (SCRATCH_ADDR (ARG(0)), stop);

  if (stop) {
    (*stop) -= (int) machPtr->memScratch;
    Block_From_Scratch (machPtr, ARG(1), sizeof (char *));
  }

  RETURN_DVALUE_L = *((long *) &val);
  RETURN_DVALUE_H = *(((long *) &val) + 1);

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strtok --
 *
 *	Handle the strtok system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strtok (machPtr)
DLX *machPtr;
{
  char *p_ret;
  char *s1;

  if (ARG(0)) {
    s1 = SCRATCH_ADDR(ARG(0));
    String_To_Scratch (machPtr, ARG(0));
  }
  else s1 = NULL;
  String_To_Scratch (machPtr, ARG(1));

  p_ret = strtok (s1, SCRATCH_ADDR(ARG(1)));

  if (p_ret) {
    RETURN_VALUE = p_ret - machPtr->memScratch;
    Block_From_Scratch (machPtr, RETURN_VALUE + strlen (p_ret) , 1);
  }
  else RETURN_VALUE = NULL;

  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Handle_Strtol --
 *
 *	Handle the strtol system call.
 *
 * Results:
 *	The result is 0 if no error occured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      The simulator and scratch memory are altered.
 *
 *----------------------------------------------------------------------
 */

Handle_Strtol (machPtr)
DLX *machPtr;
{
  long val;
  char **stop;

  String_To_Scratch (machPtr, ARG(0));

  if (ARG(1)) stop = (char **) SCRATCH_ADDR (ARG(1));
  else stop = NULL;

  val = strtol (SCRATCH_ADDR (ARG(0)), stop, ARG(2));

  if (stop) {
    (*stop) -= (int) machPtr->memScratch;
    Block_From_Scratch (machPtr, ARG(1), sizeof (char *));
  }

  RETURN_VALUE = val;

  return 0;
}

