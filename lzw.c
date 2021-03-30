/* NAME: Michelle Goh
   NetId: mg2657 

hash.h is the header file for hash.c. It will define the structs used
for the string tables in both encode and decode, as well as the functions
needed to access and use the hash table for encode*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "/c/cs323/Hwk4/code.h"
#include "stack.h"
#include "hash.h"

#define EMPTY 0

void encode(unsigned int *args) 
{
	unsigned int numEntries = 258; //256 ASCII, 2 more for special codes
	unsigned int MAXBITS = args[2];  //Max bit size for encode. 8 < MAXBITS <= 20, otherwise 12
	unsigned int size = 512; //number of slots in string table (0 to 511)
	unsigned int NBITS = 9;  //initial bit size
	unsigned int prune = args[3];

	table *hash = createTable(size, NBITS);

	unsigned int C = EMPTY;
	unsigned int K = 0;
	putBits(5, MAXBITS); //five bits for maxbits

	bool freeze = false;
	
	while((K = getchar()) != EOF) 
	{
		if(inTable(hash, C, K, NBITS) != -1) //in hash table
		{
			C = inTable(hash, C, K, NBITS);
		}
		else 
		{
			putBits(NBITS, C);
			if(numEntries == (1<<NBITS)) //double in size or prune; changeBits
			{
				if(NBITS < MAXBITS)
				{
					putBits(NBITS, 0);
					NBITS++;
					if(!freeze)
					{
						size = size*2;
						hash->entryArray = realloc(hash->entryArray, size * sizeof(entry));
						hash->chains = realloc(hash->chains, ((1<<(NBITS-3))-1) * sizeof(unsigned int));

						for(unsigned int i = 0; i < ((1<<(NBITS-3))-1); i++)
						{
							hash->chains[i] = 1;
						}

						for(unsigned int i = 2; i < 258; i++)
						{	
							insertT(hash, i, EMPTY, i-2, NBITS);
						}

						for(unsigned int i = 258; i < size/2; i++)
						{	
							unsigned int pref = hash->entryArray[i].prefix;
							unsigned int ch = hash->entryArray[i].character;
							insertT(hash, i, pref, ch, NBITS);
						}
					}	
				}
				else 
				{
					if(prune == 1)
					{
						//prune
						putBits(NBITS, 1);

						numEntries = pruneT(hash, size, &NBITS);

						if(numEntries == size)
						{
							freeze = true;
							prune = 0;
						}

						size = 1<<NBITS;
						
						freeze = false;
						
						C = inTable(hash, EMPTY, K, NBITS);

						continue;
					}
					else if(!freeze) //stop sending 0's if frozen
					{
						putBits(NBITS, 0);
					}
					
					freeze = true;
				}
			}

			if(!freeze)
			{
				insertT(hash, numEntries, C, K, NBITS);
				numEntries++;
			}
			C = inTable(hash, EMPTY, K, NBITS);
		}
	}

	if(C != EMPTY) 
	{
		putBits(NBITS, C);
	}
	flushBits();
	destroyT(hash);
}

void decode() 
{
	unsigned int numEntries = 258;
	unsigned int NBITS = 9;  //initial bit size
	unsigned int MAXBITS = getBits(5);  //Get MAXBIT from encode
	unsigned int size = 512; //number of slots in string table
	
	//initialize string table
	entry* array = createArr(size);

	unsigned int oldC = EMPTY;
	unsigned int newC = 0;
	unsigned int C = 0;
	unsigned int K = 0;
	unsigned int finalK = 0;

	bool freeze = false;
	bool justPruned = false;

	Stack s;


	while((newC = C = getBits(NBITS)) != EOF) 
	{
		if(C == 0) //increase bits and double size of table
		{
			if(NBITS < MAXBITS) //double size of table
			{
				NBITS++;
				if(!freeze)
				{
                    size = size * 2;
                    array = realloc(array, size * sizeof(entry));
				}
				continue;
			}
			else
			{
				freeze = true;
				continue;
			}
		}

		if(C == 1) //prune special code
		{

			numEntries = pruneArray(array, size, &NBITS);

			if(numEntries == size)
			{
				freeze = true;
			}

			size = 1<<NBITS;

			/*decode doesn't do insertion at this pounsigned int
			doesn't do insertion next time it gets a code

			oldC = unset; after it gets next code after pruning

			decode gets code from incode, insertion uses first char of that code
			encode try to take prefix and char and insert but fails
			send prune
			decode knows encode could not have inserted oldC that it received
			knows encode wouldnt insert it
			decode prunes
			when done pruning
			next code doesnt insert
			goes back to state it was in first time in while loop
			don't do pending insertion
			oldC is some special value
			don't do first insertion in decode
			we agree not inserting*/


			//oldC = getBits(NBITS);

			justPruned = true;

			freeze = false;
			continue;
		}

		if(C > numEntries)
		{
			fprintf(stderr,"File not by encode\n");
			free(array);
			exit(0);
		}

		//stack for outputting chars
		s = createStack();

		if(C == numEntries)
		{
			stackPush(&s, finalK);
			
			if(!justPruned)
			{
				C = oldC;
			}	
		}

		while(array[C].prefix != EMPTY)
		{
			stackPush(&s, array[C].character);
			C = array[C].prefix;
		}

		finalK = array[C].character;
		putchar(finalK);

		while(s->length>0) 
		{
			K = stackPop(s);
			putchar(K);
		}
		free(s);

		if(oldC != EMPTY) 
		{
			if(!freeze && !justPruned) //insert
			{
				entry ent;
				ent.prefix = oldC;
				ent.character = finalK;
				ent.next = 1;
				ent.isPrefix = false;

				array[oldC].isPrefix = true;
				
				array[numEntries] = ent;
				numEntries++;
			}
			else if(justPruned)
			{
				justPruned = false;
				oldC = newC;
				continue;
			}
		}

		oldC = newC;
	}

	free(array);	
}

int main(int argc, char **argv) 
{
	//setenv ("STAGE", "2", 1);
	static char bin[64], bout[64];             // Use small static buffers
	setvbuf (stdin,  bin,  _IOFBF, 64);                                      
	setvbuf (stdout, bout, _IOFBF, 64);    

	unsigned int args[4];

	for(unsigned int i = 0; i < 4; i++)
	{
		args[i] = 0;
	}

	if(!strcmp((*argv+(strlen(argv[0])-6)), "decode"))
	{
		if(argc > 1)
		{
			fprintf(stderr, "Bad args: decode\n");
			exit(0);
		}
		else 
		{
			args[0] = 1;
			decode();
		}
	}
	else
	{
		for(unsigned int i = 1; i < argc; i++) 
		{
			if(!strcmp(argv[i], "-m")) 
			{
				args[1] = 1;
				if(i + 1 == argc)
				{
					fprintf(stderr, "encode: invalid MAXBITS empty\n");
					exit(0);
				}
				else
				{
					args[2] = strtol(argv[++i], NULL, 10);
					if(args[2] == 0)
					{
						if(strlen(argv[i]) == 1 && isdigit(argv[i][0]))
						{
							;
						}
						else if(strlen(argv[i]) > 1)
						{
							if(!(isdigit(argv[i][0]) || argv[i][0] == '-' || argv[i][0] == '+'))
							{
								fprintf(stderr, "encode: invalid MAXBITS\n");
								exit(0);
							}
							for(int j = 1; j < strlen(argv[i]); j++)
							{
								if(!isdigit(argv[i][j]))
								{
									fprintf(stderr, "encode: invalid MAXBITS\n");
									exit(0);
								}
							}
						}
						else
						{
							fprintf(stderr, "encode: invalid MAXBITS\n");
							exit(0);
						}
					}
				}
			}
			else if(!strcmp(argv[i], "-p")) 
			{
				args[3] = 1;
			}
			else
			{
				fprintf(stderr, "encode: invalid args\n");
				exit(0);
			}
		}
		if(args[2] <= 8 || args[2] > 20) 
		{
			args[2] = 12;
		}
		encode(args);
	}
	exit(0);
}
