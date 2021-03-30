/* NAME: Michelle Goh
   NetId: mg2657 

hash.h is the header file for hash.c. It will define the structs used
for the string tables in both encode and decode, as well as the functions
needed to access and use the hash table for encode*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "/c/cs323/Hwk4/code.h"

#define EMPTY 0

typedef struct entry
{
	//20 max bits for codes
	unsigned int prefix : 20;

	//256 ASCII chars
	unsigned int character : 8; 
	
	//20 max bits for next index
	unsigned int next : 20;
	
	//flag for pruning
	unsigned int isPrefix: 1;

}entry;

typedef struct table
{
	//array for all nodes
	entry *entryArray;

	//array of buckets
	unsigned int *chains;
	
}table;

table *createTable(unsigned int size, unsigned int NBITS);

entry* createArr(unsigned int size);

void insertT(table* hash, unsigned int code, unsigned int pref, unsigned int ch, unsigned int NBITS);

unsigned int inTable(table* hash, unsigned int pref, unsigned int ch, unsigned int NBITS);

void destroyT(table* hash);

unsigned int pruneT(table *hash, unsigned int size, unsigned int *NBITS);

unsigned int pruneArray(entry *array, unsigned int size, unsigned int *NBITS);

void printTable(table *hash, unsigned int size);

void printArray(entry *array, unsigned int size);
