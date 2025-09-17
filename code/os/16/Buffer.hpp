#pragma once

#include "Buffer.h"
#include "Task.h"
#include "Util.h"

void bufferInit(Buffer *this)
{
    this->__leftIdx  = 0;
    this->__rightIdx = 0;
    this->__waitTask = 0;
}


bool bufferEmpty(Buffer *this)
{
    return this->__leftIdx == this->__rightIdx;
}


bool bufferFull(Buffer *this)
{
    return ((this->__rightIdx + 1) & 0xf) == this->__leftIdx;
}


void bufferPush(Buffer *this, char pushChar)
{
    if (!bufferFull(this))
    {
        this->__bufList[this->__rightIdx] = pushChar;
        this->__rightIdx = (this->__rightIdx + 1) & 0xf;
    }

    if (this->__waitTask)
    {
        this->__waitTask->__taskState = __TASK_READY;
        this->__waitTask = 0;
    }
}


char bufferPop(Buffer *this)
{
    while (bufferEmpty(this))
    {
        curTask->__taskState = __TASK_BLOCK;
        this->__waitTask = curTask;

        __asm__ __volatile__("int $0x20");
    }

    char popChar = this->__bufList[this->__leftIdx];
    this->__leftIdx = (this->__leftIdx + 1) & 0xf;

    return popChar;
}
