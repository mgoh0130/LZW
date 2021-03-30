/* NAME: Michelle Goh
   NetId: mg2657 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct stack
{
    unsigned int capacity: 5;
    unsigned int length: 20;
    uint8_t string[];
}*Stack;

Stack createStack();

void stackPush(Stack *s, uint8_t ch);

uint8_t stackPop(Stack s);
