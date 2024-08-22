#pragma once

#include "Queue.h"
#include "Util.h"

typedef struct
{
    char __bufList[0x10];
    uint64_t __leftIdx;
    uint64_t __rightIdx;
    Queue __waitQueue;
} IOQueue;


void ioqueueInit(IOQueue *this);
bool ioqueueEmpty(IOQueue *this);
bool ioqueueFull(IOQueue *this);
void ioqueuePush(IOQueue *this, char pushChar);
char ioqueuePop(IOQueue *this);
