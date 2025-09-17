#pragma once

#include "Task.h"
#include "Queue.h"
#include "Memory.h"
#include "Util.h"

#define __KERNEL_TASK_ADDR 0xc009f000

TCB *curTask = (TCB *)__KERNEL_TASK_ADDR;
Queue __taskQueue;

void __kernelTaskInit()
{
    curTask->__CR3 = 0x100000;
}


void taskInit()
{
    queueInit(&__taskQueue);

    __kernelTaskInit();
}


uint32_t __getEFLAGS()
{
    uint32_t EFLAGS;

    __asm__ __volatile__("pushf; pop %0": "=g"(EFLAGS));

    return EFLAGS;
}


TCB *loadTaskPL0(void *EIP)
{
    uint8_t *taskMemPtr = (uint8_t *)allocateKernelPage(2);

    TCB *tcbPtr    = (TCB *)taskMemPtr;
    uint32_t vCR3  = (uint32_t)(taskMemPtr + 0x1000);
    uint32_t pCR3  = *(uint32_t *)(0xffc00000 | (vCR3 >> 12 << 2)) & 0xfffff000;
    uint32_t *ESP0 = (uint32_t *)((uint32_t)tcbPtr + 0x1000 - 15 * 4);

    tcbPtr->__CR3  = pCR3;
    tcbPtr->__ESP0 = (uint32_t)ESP0;

    memcpy((void *)(vCR3 + 0xc00), (void *)0xfffffc00, 255 * 4);
    ((uint32_t *)vCR3)[1023] = pCR3 | 0x3;

    ESP0[0]  = 0;
    ESP0[1]  = 0;
    ESP0[2]  = 0;
    ESP0[3]  = 0;
    ESP0[4]  = 0;
    ESP0[5]  = 0;
    ESP0[6]  = 0;
    ESP0[7]  = 0;
    ESP0[8]  = 2 << 3;
    ESP0[9]  = 2 << 3;
    ESP0[10] = 2 << 3;
    ESP0[11] = 2 << 3;
    ESP0[12] = (uint32_t)EIP;
    ESP0[13] = 1 << 3;
    ESP0[14] = __getEFLAGS() | 0x200;

    queuePush(&__taskQueue, (Node *)tcbPtr);

    return tcbPtr;
}


TCB *getNextTask()
{
    queuePush(&__taskQueue, (Node *)curTask);
    curTask = (TCB *)queuePop(&__taskQueue);

    return curTask;
}
