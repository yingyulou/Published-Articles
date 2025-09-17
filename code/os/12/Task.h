#pragma once

#include "Util.h"

typedef struct
{
    Node _;
    uint32_t __CR3;
    uint32_t __ESP0;
    uint32_t __taskState;
    uint8_t __TSS[104];
} TCB;


extern TCB *curTask;

void taskInit();
TCB *loadTaskPL0(void *EIP);
void loadTaskPL3(uint32_t startSector, uint8_t sectorCount);
TCB *getNextTask();
