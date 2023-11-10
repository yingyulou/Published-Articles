#pragma once

#include "IOQueue.h"
#include "Queue.h"
#include "Task.h"
#include "Util.h"

void ioqueueInit(IOQueue *this)
{
    this->__leftIdx  = 0;
    this->__rightIdx = 0;

    queueInit(&this->__waitQueue);
}


bool ioqueueEmpty(IOQueue *this)
{
    return this->__leftIdx == this->__rightIdx;
}


bool ioqueueFull(IOQueue *this)
{
    return ((this->__rightIdx + 1) & 0xf) == this->__leftIdx;
}


void ioqueuePush(IOQueue *this, char pushChar)
{
    if (!ioqueueFull(this))
    {
        this->__bufList[this->__rightIdx] = pushChar;
        this->__rightIdx = (this->__rightIdx + 1) & 0xf;
    }

    if (!queueEmpty(&this->__waitQueue))
    {
        TCB *tcbPtr = (TCB *)queuePop(&this->__waitQueue);
        tcbPtr->taskQueue = &taskQueue;
        queuePush(&taskQueue, (Node *)tcbPtr);
    }
}


char ioqueuePop(IOQueue *this)
{
    while (ioqueueEmpty(this))
    {
        getTCB()->taskQueue = &this->__waitQueue;

        __asm__ __volatile__("int $0x20");
    }

    char popChar = this->__bufList[this->__leftIdx];
    this->__leftIdx = (this->__leftIdx + 1) & 0xf;

    return popChar;
}
