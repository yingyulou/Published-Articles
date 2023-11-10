#pragma once

#include "Bitmap.h"
#include "Queue.h"
#include "Util.h"

typedef struct
{
    Node tcbNode;
    uint32_t CR3;
    uint32_t ESP0;
    Queue *taskQueue;
    Bitmap vMemoryBitmap;
} TCB;


extern uint8_t TSS[];

extern Queue taskQueue;
extern Queue exitQueue;
extern Queue shellQueue;

void taskInit();
TCB *getTCB();
void loadTaskPL0(uint32_t EIP);
void loadTaskPL3(uint32_t startSector, uint32_t sectorCount);
void deleteTask();
