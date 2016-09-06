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

/* hash.h --
 *
 * 	This file contains definitions used by the hash module,
 * 	which maintains hash tables.
 *
 * Copyright 1986, 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /auto/labs/acaps/compiler/dlx/superdlx/RCS/hash.h,v 1.2 93/04/14 00:55:21 cecile Exp $ SPRITE (Berkeley)
 */


#ifndef	_HASH
#define	_HASH 1

#ifndef _LIST
#include "tcl/list.h"
#endif
#ifndef _SPRITE
#include "tcl/sprite.h"
#endif

/*
 * The following defines one entry in the hash table.
 */

typedef struct Hash_Entry {
    List_Links	      links;		/* Used to link together all the
    					 * entries associated with the same
					 * bucket. */
    ClientData	      clientData;	/* Arbitrary piece of data associated
    					 * with key. */
    union {
	Address	 ptr;			/* One-word key value to identify
					 * entry. */
	int words[1];			/* N-word key value.  Note: the actual
					 * size may be longer if necessary to
					 * hold the entire key. */
	char 	 name[4];		/* Text name of this entry.  Note: the
					 * actual size may be longer if
					 * necessary to hold the whole string.
					 * This MUST be the last entry in the
					 * structure!!! */
    } key;
} Hash_Entry;

/*
 * A hash table consists of an array of pointers to hash
 * lists.  Tables can be organized in either of three ways, depending
 * on the type of comparison keys:
 *
 *	Strings:	  these are NULL-terminated; their address
 *			  is passed to HashFind as a (char *).
 *	Single-word keys: these may be anything, but must be passed
 *			  to Hash_Find as an Address.
 *	Multi-word keys:  these may also be anything; their address
 *			  is passed to HashFind as an Address.
 *
 *	Single-word keys are fastest, but most restrictive.
 */

#define HASH_STRING_KEYS	0
#define HASH_ONE_WORD_KEYS	1

typedef struct Hash_Table {
    List_Links 	*bucketPtr;	/* Pointer to array of List_Links, one
    				 * for each bucket in the table. */
    int 	size;		/* Actual size of array. */
    int 	numEntries;	/* Number of entries in the table. */
    int 	downShift;	/* Shift count, used in hashing function. */
    int 	mask;		/* Used to select bits for hashing. */
    int 	keyType;	/* Type of keys used in table:
    				 * HASH_STRING_KEYS, HASH_ONE-WORD_KEYS,
				 * or >1 menas keyType gives number of words
				 * in keys.
				 */
} Hash_Table;

/*
 * The following structure is used by the searching routines
 * to record where we are in the search.
 */

typedef struct Hash_Search {
    Hash_Table  *tablePtr;	/* Table being searched. */
    int 	nextIndex;	/* Next bucket to check (after current). */
    Hash_Entry 	*hashEntryPtr;	/* Next entry to check in current bucket. */
    List_Links	*hashList;	/* Hash chain currently being checked. */
} Hash_Search;

/*
 * Macros.
 */

/*
 * ClientData Hash_GetValue(h)
 *     Hash_Entry *h;
 */

#define Hash_GetValue(h) ((h)->clientData)

/*
 * Hash_SetValue(h, val);
 *     HashEntry *h;
 *     char *val;
 */

#define Hash_SetValue(h, val) ((h)->clientData = (ClientData) (val))

/*
 * Hash_Size(n) returns the number of words in an object of n bytes
 */

#define	Hash_Size(n)	(((n) + sizeof (int) - 1) / sizeof (int))

/*
 * The following procedure declarations and macros
 * are the only things that should be needed outside
 * the implementation code.
 */
int  Hash(register Hash_Table *tablePtr,register char * key);
Hash_Entry * HashChainSearch(Hash_Table * tablePtr,register Address  key,register List_Links * hashList);
void Hash_DeleteEntry(Hash_Table * tablePtr,register Hash_Entry * hashEntryPtr);
void Hash_DeleteTable(Hash_Table *tablePtr);
Hash_Entry * Hash_EnumFirst(Hash_Table *tablePtr,register Hash_Search * hashSearchPtr);
Hash_Entry * Hash_FindEntry(Hash_Table *tablePtr, Address key);
Hash_Entry * Hash_EnumNext(register Hash_Search * hashSearchPtr);
Hash_Entry * Hash_FindEntry(Hash_Table * tablePtr,Address key);
void  Hash_InitTable(register Hash_Table *tablePtr,int  numBuckets,int  keyType);
/*
 * The following defines the ratio of # entries to # buckets
 * at which we rebuild the table to make it larger.
 */
//static rebuildLimit = 3;
static void RebuildTable(Hash_Table * tablePtr);
Hash_Entry * Hash_CreateEntry(register Hash_Table *tablePtr,Address key,int * newPtr);

#endif /* _HASH */

