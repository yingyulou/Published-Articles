#pragma once

#include "Task.h"
#include "Util.h"

typedef struct
{
    char __bufList[16];
    uint64_t __leftIdx;
    uint64_t __rightIdx;
    TCB *__waitTask;
} Buffer;


void bufferInit(Buffer *this);
bool bufferEmpty(Buffer *this);
bool bufferFull(Buffer *this);
void bufferPush(Buffer *this, char pushChar);
char bufferPop(Buffer *this);
