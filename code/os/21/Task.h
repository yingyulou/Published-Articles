#pragma once

#include "Util.h"

#define __TASK_READY ((uint64_t)0x0)
#define __TASK_EXIT  ((uint64_t)0x1)
#define __TASK_BLOCK ((uint64_t)0x2)

typedef struct
{
    Node _;
    uint64_t __CR3;
    uint64_t __RSP0;
    uint64_t __taskState;
    uint8_t __TSS[104];
    uint64_t __RSP3;
    uint64_t __idleTask;
} TCB;


void taskInit();
void loadTaskPL0(void *RIP);
void loadTaskPL3(uint32_t startSector, uint8_t sectorCount);
TCB *getCurTask();
TCB *getNextTask();
void taskExit();
