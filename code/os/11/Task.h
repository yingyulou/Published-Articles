#pragma once

#include "Util.h"

typedef struct
{
    Node _;
    uint32_t __CR3;
    uint32_t __ESP0;
} TCB;


extern TCB *curTask;

void taskInit();
TCB *loadTaskPL0(void *EIP);
TCB *getNextTask();
