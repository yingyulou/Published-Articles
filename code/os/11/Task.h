#pragma once

#include "Bitmap.h"
#include "Queue.h"
#include "Util.h"

typedef struct
{
    Node tcbNode;
    uint32_t CR3;
    uint32_t ESP0;
    Bitmap vMemoryBitmap;
} TCB;


extern Queue taskQueue;

void taskInit();
TCB *getTCB();
void loadTaskPL0(uint32_t EIP);
