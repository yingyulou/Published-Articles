#pragma once

#include "Bitmap.h"
#include "Queue.h"
#include "Util.h"

typedef struct
{
    Node __tcbNode;
    uint64_t __CR3;
    uint64_t __RSP0;
    Queue *__taskQueue;
    Bitmap __vBitmap;
} TCB;

extern Queue taskQueue, exitQueue;

void taskInit();
TCB *getTCB();
void loadTaskPL0(void *RIP);
void loadTaskPL3(uint32_t startSector, uint8_t sectorCount);
void deleteTask();
void taskExit();
