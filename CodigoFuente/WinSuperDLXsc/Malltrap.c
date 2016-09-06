static char rcsid[] = "$Id: malltrap.c,v 1.2 93/04/13 23:44:10 cecile Exp $";

/* 
 * malltrap.c -- 
 *
 *	This file contains functions that handle allocation and freeing of
 *	memory.  The routines are for the most part generic, but are passed
 *	the machPtr so when finished they may invoke a routine to keep
 *	scratch memory and simulator memory consistent.
 *
 */
#include "stdio.h"
#include "dlx.h"

#define MALLOC_MEM_SIZE		65536
#define MALLOC_IDENTIFIER	0x13572468

#define NULL			0
#define TRUE			1
#define FALSE			0

typedef struct malloc_hdr {		/* Desirable to keep size of this */
   unsigned long	size;		/* struct a power of 2.  See	  */
   struct malloc_hdr	*prev;		/* init_malloc.			  */
   struct malloc_hdr	*next;
   unsigned long	check;
} MALLOC_HDR;

static MALLOC_HDR *malloc_head;
static int	  malloc_blk_size;

/*
 *----------------------------------------------------------------------
 *
 * print_mem_ctl_struct_debug --
 *
 *	Print the list of free memory control structures.  This is
 *	for debugging only.
 *
 * Results:
 *	Returns nothing;
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

print_mem_ctl_struct_debug ()
{
  MALLOC_HDR *m;

  for (m = malloc_head; m; m = m->next) {
    printf ("\nm = 0x%X\n", m);
    printf ("\tm->size = %d\n", m->size);
    printf ("\tm->prev = 0x%X\n", m->prev);
    printf ("\tm->next = 0x%X\n", m->next);
    printf ("\tm->check = 0x%X\n", m->check);
  }
}

/*
 *----------------------------------------------------------------------
 *
 * init_malloc --
 *
 *	Initialize the system to use malloc.  Should be called
 *	before each program is run.
 *
 * Results:
 *	The return value is 0 if no error ocured, 1 if a problem
 *	arises.
 *
 * Side effects:
 *      Sets malloc_head and its contents.
 *
 *----------------------------------------------------------------------
 */

init_malloc (machPtr, p, size)
DLX *machPtr;
char *p;
int  size;
{
  int i;
  unsigned p_align;

  /* Min size block allocated is the first power of 2 greater than or
     or equal to sizeof (MALLOC_HDR).
  */
  for (i = 1; i <= 65536; i <<= 1) {
    if (i >= sizeof (MALLOC_HDR)) {
      malloc_blk_size = i;
      break;
    }
  }
  if (i == 65536) {
    printf ("Size of malloc block too large.  Should never get here.\n");
    exit (1);
  }

  /* Align allocated memory on blk_size boundary */
  p_align = malloc_blk_size *
      (((unsigned) p + malloc_blk_size - 1) / malloc_blk_size);

  malloc_head = (MALLOC_HDR *) p_align;

  size -= (p_align - (unsigned) p);	   /* Adjust size for alignment. */
				/* Make size a multiple of the blk size. */
  size = malloc_blk_size * (size / malloc_blk_size);

  if (size < 2*malloc_blk_size) {
    printf ("Not enough memory to set up heap for malloc.\n");
    printf ("Use -ms# to increase your memory size.\n");
    printf ("At least %d more bytes needed.\n", 2*malloc_blk_size-size);
    return 1;
  }

  malloc_head->size = size;
  malloc_head->prev = NULL;
  malloc_head->next = NULL;
  malloc_head->check = NULL;

  freelist_to_sim_mem (machPtr, malloc_head);

  return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * do_malloc --
 *
 *	Return pointer to memory of size requested by user.
 *
 * Results:
 *	Returns pointer to memory if available, NULL otherwise.
 *
 * Side effects:
 *      Can change malloc_head and the control structure of
 *	allocated memory.
 *
 *----------------------------------------------------------------------
 */

void *do_malloc (machPtr, size)
DLX *machPtr;
int size;
{
  MALLOC_HDR *m, *mnew;
  MALLOC_HDR *m_flist = NULL;
  void *rmem = NULL;

  if (size == 0) return rmem;		/* Don't try to allocate no space */
  else {			
    size = malloc_blk_size *		/* Make size multiple of blk size */
	      ((size + malloc_blk_size - 1) / malloc_blk_size);
    size += malloc_blk_size;		/* Include hdr as part of size */
  }

  for (m = malloc_head; m; m = m->next) {

    if (m->size > size) {			/* Larger than need */

      rmem = (MALLOC_HDR *) (((char *) m) + malloc_blk_size);

      mnew = (MALLOC_HDR *) (((char *) m) + size);
      m_flist = mnew;
      mnew->size = m->size - size;
      mnew->prev = m->prev;

      if (m->prev) m->prev->next = mnew;
      mnew->next = m->next;
      if (m->next) m->next->prev = mnew;
      m->check = MALLOC_IDENTIFIER;
      m->size = size;

      if (m == malloc_head) malloc_head = mnew;

      break;
    }

    else if (m->size == size) {			/* Exact Fit */

      rmem = (MALLOC_HDR *) (((char *) m) + malloc_blk_size);

      m->check = MALLOC_IDENTIFIER;
      if (m->prev) {
	m->prev->next = m->next;
	m_flist = m->prev;
      }
      if (m->next) {
	m->next->prev = m->prev;
	m_flist = m->next;
      }
      if (m == malloc_head) {
        if (m->next) malloc_head = m->next;
        else malloc_head = NULL;
      }
      break;
    }
  }

  freelist_to_sim_mem (machPtr, m_flist);
  malloc_to_sim_mem (machPtr, rmem);

  return rmem;
}

/*
 *----------------------------------------------------------------------
 *
 * do_free --
 *
 *	Return the memory previously obtained with malloc to the
 *	free memory pool.
 *
 * Results:
 *	Indicate an error if the "check" indicates that the memory
 *	was not allocated by malloc.  (This often occurs if the user
 *	goes past the amount of memory allocated.)
 *
 * Side effects:
 *      Can change malloc_head and the control structure of
 *	allocated memory.
 *
 *----------------------------------------------------------------------
 */

do_free (machPtr, p)
DLX *machPtr;
void *p;
{
  MALLOC_HDR *m, *m_prev, *m_free, *m_ret;
  int	     coalesced_next = FALSE;

  if (!p) return;			/* Do nothing on NULL ptr */

  m_free = (MALLOC_HDR *) (((char *) p) - malloc_blk_size);
  if (m_free->check != MALLOC_IDENTIFIER) {
    printf ("WARNING:  free called with ptr to unallocated memory.\n");
    printf ("Program may have written past the end of allocated memory.\n");
    m_ret = NULL;
  }
  else {

    m_ret = m_free;
    m_free->check = NULL;

    /* Search control blocks for place to insert freed block */
    m_prev = malloc_head;
    for (m = malloc_head; m; m = m->next) {
      if (m > m_free) break;
      m_prev = m;
    }

    if (m) {
/* We're at the "next" block for the freed block */
      m_free->next = m;
      m_free->prev = m->prev;

      /* Coalesce with next block if possible */
      if ((((char *) m_free) + m_free->size) == (char *) m) {
        m_free->size += m->size;
        m_free->next = m->next;
	if (m->next) {
	  m->next->prev = m_free;
	  coalesced_next = TRUE;
	}
      }

      if (m->prev) {
      /* Coalesce with previous block if possible */
        if ((((char *) m->prev) + m->prev->size) == (char *) m_free) {
	  m->prev->size += m_free->size;
	  m->prev->next = m_free->next;
	  if (coalesced_next) m->next->prev = m->prev;
	  m_ret = m->prev;
	}
	else {
	  m->prev->next = m_free;
	  m->prev = m_free;
	}
      }
      else {
	malloc_head = m_free;
	m->prev = m_free;
      }
    }
    else {
/* Freed block comes after all currently available blocks */
      if (m_prev) {
      /* Coalesce with previous block if possible */
        if ((((char *) m_prev) + m_prev->size) == (char *) m_free) {
	  m_prev->size += m_free->size;
	  m_ret = m_prev;
	}
	else {
	  m_prev->next = m_free;
	  m_free->prev = m_prev;
	  m_free->next = NULL;
	}
      }
      else {
	malloc_head = m_free;
        m_free->prev = NULL;
        m_free->next = NULL;
      }
    }
  }

  freelist_to_sim_mem (machPtr, m_ret);
}

/*
 *----------------------------------------------------------------------
 *
 * freelist_to_sim_mem --
 *
 *	Copy modified parts of the freelist from scratch memory
 *	to simulator memory.
 *
 * Results:
 *	Returns nothing.
 *
 * Side effects:
 *      The simulator data is altered.
 *
 *----------------------------------------------------------------------
 */

freelist_to_sim_mem (machPtr, m)
DLX	   *machPtr;
MALLOC_HDR *m;
{
  if (m) {
    Block_From_Scratch (
  		machPtr, ((char *) m) - machPtr->memScratch, sizeof (*m));

    if (m->next)
      Block_From_Scratch (
  		machPtr, ((char *) &m->next->prev) - machPtr->memScratch,
		sizeof (m->next->prev));

    if (m->prev)
      Block_From_Scratch (
  		machPtr, ((char *) &m->prev->next) - machPtr->memScratch,
		sizeof (m->prev->next));
  }
}

/*
 *----------------------------------------------------------------------
 *
 * malloc_to_sim_mem --
 *
 *	Copy the header of an allocated malloc (calloc) block
 *	to simulator memory.
 *
 * Results:
 *	Returns nothing.
 *
 * Side effects:
 *      The simulator data is altered.
 *
 *----------------------------------------------------------------------
 */

malloc_to_sim_mem (machPtr, rmem)
DLX  *machPtr;
char *rmem;
{
  MALLOC_HDR *m;

  if (!rmem) return;

  m = (MALLOC_HDR *) (rmem - malloc_blk_size);

  /* Really only need size and check fields of m */
  Block_From_Scratch (
  		machPtr, ((char *) m) - machPtr->memScratch, sizeof (*m));
}
