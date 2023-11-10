#pragma once

#include "Task.h"
#include "Bitmap.h"
#include "Queue.h"
#include "Memory.h"
#include "Util.h"

Queue taskQueue;

void __installKernelTask()
{
    TCB *tcbPtr = (TCB *)0xc009f000;

    tcbPtr->CR3 = 0x100000;

    bitmapInit(&tcbPtr->vMemoryBitmap, (uint8_t *)0xc009d000, 0x8000, true);
}


void taskInit()
{
    queueInit(&taskQueue);

    __installKernelTask();
}


TCB *getTCB()
{
    uint32_t ESP0;

    __asm__ __volatile__("mov %%esp, %0": "=g"(ESP0));

    return (TCB *)(ESP0 & 0xfffff000);
}


uint32_t __getEFLAGS()
{
    uint32_t EFLAGS;

    __asm__ __volatile__(
        "pushf\n\t"
        "pop %0\n\t"
        : "=g"(EFLAGS)
    );

    return EFLAGS;
}


void loadTaskPL0(uint32_t EIP)
{
    uint8_t *taskMemoryPtr = (uint8_t *)allocateKernelPage(3);

    TCB *tcbPtr            = (TCB *)taskMemoryPtr;
    uint32_t vCR3          = (uint32_t)(taskMemoryPtr + 0x1000);
    uint32_t pCR3          = (*(uint32_t *)(0xffc00000 | (vCR3 >> 12 << 2))) & 0xfffff000;
    uint8_t *vMemoryBitmap = taskMemoryPtr + 0x2000;

    tcbPtr->CR3 = pCR3;

    memset((void *)vCR3, 0, 0x1000);
    memcpy((void *)(vCR3 + 0xc00), (void *)0xfffffc00, 255 * 4);

    ((uint32_t *)vCR3)[1023] = pCR3 | 0x3;

    tcbPtr->ESP0 = (uint32_t)tcbPtr + 0x1000 - 11 * 4;

    uint32_t *ESP0 = (uint32_t *)tcbPtr->ESP0;

    ESP0[0]  = 0;
    ESP0[1]  = 0;
    ESP0[2]  = 0;
    ESP0[3]  = 0;
    ESP0[4]  = 0;
    ESP0[5]  = 0;
    ESP0[6]  = 0;
    ESP0[7]  = 0;
    ESP0[8] = EIP;
    ESP0[9] = 1 << 3;
    ESP0[10] = __getEFLAGS() | 0x200;

    bitmapInit(&tcbPtr->vMemoryBitmap, vMemoryBitmap, 0x8000, true);

    queuePush(&taskQueue, (Node *)tcbPtr);
}
