
/*
 * Hash.c --
 *
 *	Source code for Hash, a utility procedure used by the hash
 *	table library.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /auto/labs/acaps/compiler/dlx/superdlx/RCS/Hash.c,v 1.2 93/04/13 23:26:56 cecile Exp $ SPRITE (Berkeley)";
#endif not lint
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "list.h"

/*
 *---------------------------------------------------------
 *
 * Hash --
 *	This is a local procedure to compute a hash table
 *	bucket address based on a string value.
 *
 * Results:
 *	The return value is an integer between 0 and size - 1.
 *
 * Side Effects:
 *	None.
 *
 * Design:
 *	It is expected that most keys are decimal numbers,
 *	so the algorithm behaves accordingly.  The randomizing
 *	code is stolen straight from the rand library routine.
 *
 *---------------------------------------------------------
 */

int
Hash(tablePtr, key)
    register Hash_Table *tablePtr;
    register char 	*key;
{
    register int 	i = 0;
    register int 	j;
    register int 	*intPtr;

    switch (tablePtr->keyType) {
	case HASH_STRING_KEYS:
	    while (*key != 0) {
		i = (i * 10) + (*key++ - '0');
	    }
	    break;
	case HASH_ONE_WORD_KEYS:
	    i = (int) key;
	    break;
	case 2:
	    i = ((int *) key)[0] + ((int *) key)[1];
	    break;
	default:
	    j = tablePtr->keyType;
	    intPtr = (int *) key;
	    do {
		i += *intPtr++;
		j--;
	    } while (j > 0);
	    break;
    }


    return(((i*1103515245 + 12345) >> tablePtr->downShift) & tablePtr->mask);
}

// ----------------------------------------------
/*
 *---------------------------------------------------------
 * 
 * Hash_InitTable --
 *
 *	This routine just sets up the hash table.
 *
 * Results:	
 *	None.
 *
 * Side Effects:
 *	Memory is allocated for the initial bucket area.
 *
 *---------------------------------------------------------
 */

void
Hash_InitTable(tablePtr, numBuckets, keyType)
    register Hash_Table *tablePtr;	/* Structure to use to hold table. */
    int 	        numBuckets;	/* How many buckets to create for
					 * starters. This number is rounded
					 * up to a power of two.   If <= 0,
					 * a reasonable default is chosen.
					 * The table will grow in size later
					 * as needed. */
    int 	        keyType;	/* HASH_STRING_KEYS means that key
    					 * values passed to HashFind will be
					 * strings, passed via a (char *)
					 * pointer.  HASH_ONE_WORD_KEYS means
					 * that key values will be any
					 * one-word value passed as Address.
			 		 * > 1 means that key values will be 
				 	 * multi-word values whose address is
					 * passed as Address.  In this last
					 * case, keyType is the number of
					 * words in the key, not the number
					 * of bytes. */
{
    register	int 		i;
    register	List_Links 	*bucketPtr;

    /* 
     * Round up the size to a power of two. 
     */

    if (numBuckets <= 0) {
	numBuckets = 16;
    }
    tablePtr->numEntries = 0;
    tablePtr->keyType = keyType;
    tablePtr->size = 2;
    tablePtr->mask = 1;
    tablePtr->downShift = 29;
    while (tablePtr->size < numBuckets) {
	tablePtr->size <<= 1;
	tablePtr->mask = (tablePtr->mask << 1) + 1;
	tablePtr->downShift--;
    }

    tablePtr->bucketPtr = (List_Links *) calloc(1, (unsigned) (sizeof(List_Links)
	    * tablePtr->size));
    for (i=0, bucketPtr = tablePtr->bucketPtr; i < tablePtr->size;
	    i++, bucketPtr++) {
	List_Init(bucketPtr);
    }
}

// ----------------------------------------------

/*
 *---------------------------------------------------------
 *
 * Hash_EnumNext --
 *    This procedure returns successive entries in the hash table.
 *
 * Results:
 *    The return value is a pointer to the next HashEntry
 *    in the table, or NULL when the end of the table is
 *    reached.
 *
 * Side Effects:
 *    The information in hashSearchPtr is modified to advance to the
 *    next entry.
 *
 *---------------------------------------------------------
 */

Hash_Entry *
Hash_EnumNext(hashSearchPtr)
    register Hash_Search *hashSearchPtr; /* Area used to keep state about
					    search. */
{
    register List_Links *hashList;
    register Hash_Entry *hashEntryPtr;

    hashEntryPtr = hashSearchPtr->hashEntryPtr;
    while (hashEntryPtr == (Hash_Entry *) NULL ||
	   List_IsAtEnd(hashSearchPtr->hashList,
	   (List_Links *) hashEntryPtr)) {
	if (hashSearchPtr->nextIndex >= hashSearchPtr->tablePtr->size) {
	    return((Hash_Entry *) NULL);
	}
	hashList = &(hashSearchPtr->tablePtr->bucketPtr[
		hashSearchPtr->nextIndex]);
	hashSearchPtr->nextIndex++;
	if (!List_IsEmpty(hashList)) {
	    hashEntryPtr = (Hash_Entry *) List_First(hashList);
	    hashSearchPtr->hashList = hashList;
	    break;
	}
    }

    hashSearchPtr->hashEntryPtr =
		(Hash_Entry *) List_Next((List_Links *) hashEntryPtr);

    return(hashEntryPtr);
}

// ----------------------------------------------
/*
 *---------------------------------------------------------
 *
 * HashChainSearch --
 *
 * 	Search the hash table for the entry in the hash chain.
 *
 * Results:
 *	Pointer to entry in hash chain, NULL if none found.
 *
 * Side Effects:
 *	None.
 *
 *---------------------------------------------------------
 */

Hash_Entry *
HashChainSearch(tablePtr, key, hashList)
    Hash_Table 		*tablePtr;	/* Hash table to search. */
    register Address	key;	/* A hash key. */
    register List_Links *hashList;
{
    register Hash_Entry *hashEntryPtr;
    register int 	*hashKeyPtr;
    int 		*keyPtr;
    register int	numKeys;

    numKeys = tablePtr->keyType;
    LIST_FORALL(hashList, (List_Links *) hashEntryPtr) {
	switch (numKeys) {
	    case 0:
		if (strcmp(hashEntryPtr->key.name, key) == 0) {
		    return(hashEntryPtr);
		}
		break;
	    case 1:
		if (hashEntryPtr->key.ptr == key) {
		    return(hashEntryPtr);
		}
		break;
	    case 2:
		hashKeyPtr = hashEntryPtr->key.words;
		keyPtr = (int *) key;
		if (*hashKeyPtr++ == *keyPtr++ && *hashKeyPtr == *keyPtr) {
		    return(hashEntryPtr);
		}
		break;
	    default:
      // Sustituida funci�n bcmp por strncmp
      // el if lo que busca es que las dos cadenas sean iguales en el n�mero de bytes
      // que se consideran
		if (!strncmp((char *) hashEntryPtr->key.words,
			    (char *) key, numKeys * sizeof(int))) {
		    return(hashEntryPtr);
		}
		break;
	}
    }

    /* 
     * The desired entry isn't there 
     */

    return ((Hash_Entry *) NULL);
}

// ----------------------------------------------

/*
 *---------------------------------------------------------
 *
 * Hash_FindEntry --
 *
 * 	Searches a hash table for an entry corresponding to key.
 *
 * Results:
 *	The return value is a pointer to the entry for key,
 *	if key was present in the table.  If key was not
 *	present, NULL is returned.
 *
 * Side Effects:
 *	None.
 *
 *---------------------------------------------------------
 */

Hash_Entry *
Hash_FindEntry(tablePtr, key)
    Hash_Table *tablePtr;	/* Hash table to search. */
    Address key;		/* A hash key. */
{
    return(HashChainSearch(tablePtr, key,
	    &(tablePtr->bucketPtr[Hash(tablePtr, key)])));
}

// ----------------------------------------------
/*
 *---------------------------------------------------------
 *
 * Hash_EnumFirst --
 *	This procedure sets things up for a complete search
 *	of all entries recorded in the hash table.
 *
 * Results:	
 *	The return value is the address of the first entry in
 *	the hash table, or NULL if the table is empty.
 *
 * Side Effects:
 *	The information in hashSearchPtr is initialized so that successive
 *	calls to Hash_Next will return successive HashEntry's
 *	from the table.
 *
 *---------------------------------------------------------
 */

Hash_Entry *
Hash_EnumFirst(tablePtr, hashSearchPtr)
    Hash_Table *tablePtr;			/* Table to be searched. */
    register Hash_Search *hashSearchPtr;	/* Area in which to keep state 
						 * about search.*/
{
    hashSearchPtr->tablePtr = tablePtr;
    hashSearchPtr->nextIndex = 0;
    hashSearchPtr->hashEntryPtr = (Hash_Entry *) NULL;
    return Hash_EnumNext(hashSearchPtr);
}

// ----------------------------------------------
/*
 *---------------------------------------------------------
 *
 * Hash_DeleteTable --
 *
 *	This routine removes everything from a hash table
 *	and frees up the memory space it occupied (except for
 *	the space in the Hash_Table structure).
 *
 * Results:	
 *	None.
 *
 * Side Effects:
 *	Lots of memory is freed up.
 *
 *---------------------------------------------------------
 */

void
Hash_DeleteTable(tablePtr)
    Hash_Table *tablePtr;		/* Hash table whose entries are all to
					 * be freed.  */
{
    register List_Links *hashTableEnd;
    register Hash_Entry *hashEntryPtr;
    register List_Links *bucketPtr;

    bucketPtr = tablePtr->bucketPtr;
    hashTableEnd = &(bucketPtr[tablePtr->size]);
    for (; bucketPtr < hashTableEnd; bucketPtr++) {
	while (!List_IsEmpty(bucketPtr)) {
	    hashEntryPtr = (Hash_Entry *) List_First(bucketPtr);
	    List_Remove((List_Links *) hashEntryPtr);
	    free((Address) hashEntryPtr);
	}
    }
    free((Address) tablePtr->bucketPtr);

    /*
     * Set up the hash table to cause memory faults on any future
     * access attempts until re-initialization.
     */

    tablePtr->bucketPtr = (List_Links *) NIL;
}

// ----------------------------------------------
/*
 *---------------------------------------------------------
 *
 * Hash_DeleteEntry --
 *
 * 	Delete the given hash table entry and free memory associated with
 *	it.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Hash chain that entry lives in is modified and memory is freed.
 *
 *---------------------------------------------------------
 */

void
Hash_DeleteEntry(tablePtr, hashEntryPtr)
    Hash_Table			*tablePtr;
    register	Hash_Entry	*hashEntryPtr;
{
    if (hashEntryPtr != (Hash_Entry *) NULL) {
	List_Remove((List_Links *) hashEntryPtr);
	free((Address) hashEntryPtr);
	tablePtr->numEntries--;
    }
}

// ----------------------------------------------

/*
 * The following defines the ratio of # entries to # buckets
 * at which we rebuild the table to make it larger.
 */

static rebuildLimit = 3;

/*
 *---------------------------------------------------------
 *
 * RebuildTable --
 *	This local routine makes a new hash table that
 *	is larger than the old one.
 *
 * Results:
 * 	None.
 *
 * Side Effects:
 *	The entire hash table is moved, so any bucket numbers
 *	from the old table are invalid.
 *
 *---------------------------------------------------------
 */

static void
RebuildTable(tablePtr)
    Hash_Table 	*tablePtr;		/* Table to be enlarged. */
{
    int 		 oldSize;
    int 		 bucket;
    List_Links		 *oldBucketPtr;
    register Hash_Entry  *hashEntryPtr;
    register List_Links	 *oldHashList;

    oldBucketPtr = tablePtr->bucketPtr;
    oldSize = tablePtr->size;

    /*
     * Build a new table 4 times as large as the old one.
     */

    Hash_InitTable(tablePtr, tablePtr->size * 4, tablePtr->keyType);

    for (oldHashList = oldBucketPtr; oldSize > 0; oldSize--, oldHashList++) {
	while (!List_IsEmpty(oldHashList)) {
	    hashEntryPtr = (Hash_Entry *) List_First(oldHashList);
	    List_Remove((List_Links *) hashEntryPtr);
	    switch (tablePtr->keyType) {
		case HASH_STRING_KEYS:
		    bucket = Hash(tablePtr, (Address) hashEntryPtr->key.name);
		    break;
		case HASH_ONE_WORD_KEYS:
		    bucket = Hash(tablePtr, (Address) hashEntryPtr->key.ptr);
		    break;
		default:
		    bucket = Hash(tablePtr, (Address) hashEntryPtr->key.words);
		    break;
	    }
	    List_Insert((List_Links *) hashEntryPtr,
		LIST_ATFRONT(&(tablePtr->bucketPtr[bucket])));
	    tablePtr->numEntries++;
	}
    }

    free((Address) oldBucketPtr);
}

/*
 *---------------------------------------------------------
 *
 * Hash_CreateEntry --
 *
 *	Searches a hash table for an entry corresponding to
 *	key.  If no entry is found, then one is created.
 *
 * Results:
 *	The return value is a pointer to the entry.  If *newPtr
 *	isn't NULL, then *newPtr is filled in with TRUE if a
 *	new entry was created, and FALSE if an entry already existed
 *	with the given key.
 *
 * Side Effects:
 *	Memory may be allocated, and the hash buckets may be modified.
 *---------------------------------------------------------
 */

Hash_Entry *
Hash_CreateEntry(tablePtr, key, newPtr)
    register Hash_Table *tablePtr;	/* Hash table to search. */
    Address key;			/* A hash key. */
    int *newPtr;			/* Filled in with TRUE if new entry
    					 * created, FALSE otherwise. */
					 /* cecile: int replaces Boolean */
{
    register Hash_Entry *hashEntryPtr;
    register int 	*hashKeyPtr;
    register int 	*keyPtr;
    List_Links 		*hashList;

    keyPtr = (int *) key;

    hashList = &(tablePtr->bucketPtr[Hash(tablePtr, (Address) keyPtr)]);
    hashEntryPtr = HashChainSearch(tablePtr, (Address) keyPtr, hashList);

    if (hashEntryPtr != (Hash_Entry *) NULL) {
	if (newPtr != NULL) {
	    *newPtr = FALSE;
	}
    	return hashEntryPtr;
    }

    /* 
     * The desired entry isn't there.  Before allocating a new entry,
     * see if we're overloading the buckets.  If so, then make a
     * bigger table.
     */

    if (tablePtr->numEntries >= rebuildLimit * tablePtr->size) {
	RebuildTable(tablePtr);
	hashList = &(tablePtr->bucketPtr[Hash(tablePtr, (Address) keyPtr)]);
    }
    tablePtr->numEntries += 1;

    /*
     * Not there, we have to allocate.  If the string is longer
     * than 3 bytes, then we have to allocate extra space in the
     * entry.
     */

    switch (tablePtr->keyType) {
	case HASH_STRING_KEYS:
	    hashEntryPtr = (Hash_Entry *) calloc(1, (unsigned)
		    (sizeof(Hash_Entry) + strlen((char *) keyPtr) - 3));
	    strcpy(hashEntryPtr->key.name, (char *) keyPtr);
	    break;
	case HASH_ONE_WORD_KEYS:
	    hashEntryPtr = (Hash_Entry *) calloc(1, sizeof(Hash_Entry));
	    hashEntryPtr->key.ptr = (Address) keyPtr;
	    break;
	case 2:
	    hashEntryPtr = 
		(Hash_Entry *) calloc(1, sizeof(Hash_Entry) + sizeof(int));
	    hashKeyPtr = hashEntryPtr->key.words;
	    *hashKeyPtr++ = *keyPtr++;
	    *hashKeyPtr = *keyPtr;
	    break;
	default: {
	    register 	n;

	    n = tablePtr->keyType;
	    hashEntryPtr = (Hash_Entry *) 
		    calloc(1, (unsigned) (sizeof(Hash_Entry)
		    + (n - 1) * sizeof(int)));
	    hashKeyPtr = hashEntryPtr->key.words;
	    do { 
		*hashKeyPtr++ = *keyPtr++; 
	    } while (--n);
	    break;
	}
    }

    hashEntryPtr->clientData = (ClientData) NULL;
    List_Insert((List_Links *) hashEntryPtr, LIST_ATFRONT(hashList));

    if (newPtr != NULL) {
	*newPtr = TRUE;
    }
    return hashEntryPtr;
}






