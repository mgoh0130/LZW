/* NAME: Michelle Goh
   NetId: mg2657 

hash.c contains code needed for the string table data structure for
encode. hash.c defines structs for the table entries and for the 
hash table that uses chaining.*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "hash.h"


//initialize table, returning pounsigned inter to table
table *createTable(unsigned int size, unsigned int NBITS)
{
	table *hash = malloc(sizeof(table));
	hash->entryArray = malloc(sizeof(entry)*size);

	//NCHAINS = 2^(NBITS-3) - 1
	unsigned int NCHAINS = (1<<(NBITS-3))-1;
	hash->chains = malloc(sizeof(unsigned int) * NCHAINS);

	for(unsigned int i = 0; i < NCHAINS; i++)
	{
		hash->chains[i] = 1;
	}

	for(unsigned int i = 2; i < 258; i++)
	{
		//prunsigned intf("%d\n", i);
		insertT(hash, i, EMPTY, i-2, NBITS);
	}
	
	return hash;
}

entry* createArr(unsigned int size)
{
	entry* array = malloc(sizeof(entry)*size); 
	for(unsigned int i = 2; i < 258; i++)
	{
		entry ent;
		ent.prefix = EMPTY;
		ent.character = i-2;
		ent.isPrefix = false;
		ent.next = i;

		array[i] = ent;
	}

	for(unsigned int i = 258; i < size; i++)
	{
		entry ent;
		ent.prefix = 1;
		ent.character = 0;
		ent.isPrefix = false;
		ent.next = 1;

		array[i] = ent;
	}

	return array;
}

void insertT(table* hash, unsigned int code, unsigned int pref, unsigned int ch, unsigned int NBITS)
{
	unsigned int NCHAINS = (1<<(NBITS-3))-1;
	
	//get index from hash function; SIZE is the number of chains
	//index of bucket
	unsigned int index = (((unsigned long)(pref) << CHAR_BIT) | (unsigned)(ch)) % NCHAINS;

	if(hash->chains[index] == 1) //-1, no head node
	{
		hash->chains[index] = code;
	}
	else //add to chain
	{
		unsigned int nextInd = hash->chains[index];
		unsigned int prevInd = nextInd;
		while(nextInd != 1)
		{
			prevInd = nextInd;
			nextInd = hash->entryArray[nextInd].next;
		}
		hash->entryArray[prevInd].next = code;
	}

	entry insert;
	insert.prefix = pref;
	insert.character = ch;
	insert.next = 1;
	insert.isPrefix = false;

	hash->entryArray[code] = insert;
	hash->entryArray[pref].isPrefix = true;
	return;
}

unsigned int inTable(table* hash, unsigned int pref, unsigned int ch, unsigned int NBITS)
{
	unsigned int NCHAINS = (1<<(NBITS-3))-1;
	
	//find chain of (PREFIX, CHAR)
	unsigned int hashInd = (((unsigned long)(pref) << CHAR_BIT) | (unsigned)(ch)) % NCHAINS;

	unsigned int nextInd = hash->chains[hashInd];
	while(nextInd != 1)
	{
		if((hash->entryArray[nextInd].prefix == pref) && 
			(hash->entryArray[nextInd].character == ch)) //found code
		{
			//hash->entryArray[nextInd].isPrefix = true;
			return nextInd;
		}
		nextInd = hash->entryArray[nextInd].next;
	}
	return -1;
}

void destroyT(table *hash)
{
	free(hash->entryArray);
	free(hash->chains);
	free(hash);
}

unsigned int pruneT(table *hash, unsigned int size, unsigned int *NBITS)
{
	for(unsigned int i = 2; i < 258; i++)
	{
		hash->entryArray[i].next = i;
	}

	unsigned int indexTwo = 258;
	for(unsigned int i = 258; i < size; i++)
	{
		if(hash->entryArray[i].isPrefix)
		{			
			hash->entryArray[i].next = indexTwo;
			//new prefix is next var of old prefix
			hash->entryArray[indexTwo].prefix = hash->entryArray[hash->entryArray[i].prefix].next;
			hash->entryArray[indexTwo].character = hash->entryArray[i].character;

			hash->entryArray[hash->entryArray[hash->entryArray[i].prefix].next].isPrefix = true;
			
			hash->entryArray[indexTwo].isPrefix = false;
			indexTwo++;
		}
	}

	unsigned int numBits = *NBITS;

	while(indexTwo < 1<<numBits)
	{
		numBits--;
	}
	numBits++;

	unsigned int newSize = 1<<numBits;
	unsigned int NCHAINS = (1<<(numBits-3))-1;

	if(newSize != size)
	{
		hash->entryArray = realloc(hash->entryArray, newSize * sizeof(entry));
		hash->chains = realloc(hash->chains, NCHAINS * sizeof(unsigned int));
	}

	unsigned int index;

	//rehash
	for(unsigned int i = 0; i < NCHAINS; i++)
	{
		hash->chains[i] = 1;
	}

	for(unsigned int i = 2; i < indexTwo; i++)
	{
		hash->entryArray[i].next = 1;
	}

	for(unsigned int i = 2; i < indexTwo; i++)
	{
		index = (((unsigned long)(hash->entryArray[i].prefix) << CHAR_BIT) |
			(unsigned)(hash->entryArray[i].character)) % NCHAINS;
		
		if(hash->chains[index] == 1) //-1, no head node
		{
			hash->chains[index] = i;
		}
		else
		{
			unsigned int nextInd = hash->chains[index];
			unsigned int prevInd = nextInd;
			while(nextInd != 1)
			{
				prevInd = nextInd;
				nextInd = hash->entryArray[nextInd].next;
			}
			hash->entryArray[prevInd].next = i;
		}
	}
	*NBITS = numBits;
	return indexTwo; //number of entries
}

unsigned int pruneArray(entry *array, unsigned int size, unsigned int *NBITS)
{
	unsigned int indexTwo = 258;
	for(unsigned int i = 258; i < size; i++)
	{
		if(array[i].isPrefix)
		{			
			array[i].next = indexTwo;
			//new prefix is next var of old prefix
			
			array[indexTwo].prefix = array[array[i].prefix].next;

			array[indexTwo].character = array[i].character;
						
			array[array[array[i].prefix].next].isPrefix = true;
			
			array[indexTwo].isPrefix = false;
			indexTwo++;
		}
	}

	unsigned int numBits = *NBITS;

	while(indexTwo < 1<<numBits)
	{
		numBits--;
	}
	numBits++;

	unsigned int newSize = 1<<numBits;

	if(newSize != size)
	{
		array = realloc(array, newSize * sizeof(entry));
	}

	*NBITS = numBits;
	return indexTwo;
}

void printTable(table *hash, unsigned int size)
{
	for(unsigned int i = 2; i < size; i++)
	{
		fprintf(stderr,"CODE: %d", i);
		fprintf(stderr," PREFIX: %d", hash->entryArray[i].prefix);
		fprintf(stderr," CHAR: %d", hash->entryArray[i].character);
		fprintf(stderr," BOOL: %d\n", hash->entryArray[i].isPrefix);
	}

	fprintf(stderr,"SIZE: %d\n", size);
}

void printArray(entry *array, unsigned int size)
{
	for(unsigned int i = 2; i < size; i++)
	{
		fprintf(stderr, "CODE: %d", i);
		fprintf(stderr, " PREFIX: %d", array[i].prefix);
		fprintf(stderr, " CHAR: %d", array[i].character);
		fprintf(stderr," BOOL: %d\n", array[i].isPrefix);
	}

	fprintf(stderr, "SIZE: %d\n", size);
}

