/* NAME: Michelle Goh
   NetId: mg2657 
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "stack.h"

Stack createStack()
{
    Stack s = malloc(sizeof(*s) + sizeof(uint8_t) * 4);

    s->length = 0;
    s->capacity = 2;
    return s;
}

void stackPush(Stack *s, uint8_t ch)
{
    Stack myStack = *s;
    if(myStack->length >= 1<<myStack->capacity)
    {
        myStack->capacity++;
        Stack temp = realloc(myStack, sizeof(*temp)+ (sizeof(uint8_t)) * (1<<(myStack->capacity)));

        myStack = temp;
        *s = myStack;
    }
    myStack->string[myStack->length] = ch;
    myStack->length++;
}

uint8_t stackPop(Stack s)
{
    uint8_t ch = s->string[s->length - 1];
    s->length--;
    return ch;
}
