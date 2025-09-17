#pragma once

#include "Task.h"
#include "Queue.h"
#include "Memory.h"
#include "HD.h"
#include "Shell.h"
#include "Util.h"

#define __KERNEL_TASK_ADDR 0xc009f000

TCB *curTask = (TCB *)__KERNEL_TASK_ADDR;
Queue __taskQueue;

void __kernelTaskInit()
{
    curTask->__CR3       = 0x100000;
    curTask->__taskState = __TASK_READY;

    memset(curTask->__TSS, 0x0, 104);
    *(uint16_t *)(curTask->__TSS + 8)   = 2 << 3;
    *(uint16_t *)(curTask->__TSS + 102) = 103;

    __asm__ __volatile__("ltr %w0":: "r"(5 << 3));
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

    tcbPtr->__CR3       = pCR3;
    tcbPtr->__ESP0      = (uint32_t)ESP0;
    tcbPtr->__taskState = __TASK_READY;

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


void loadTaskPL3(uint32_t startSector, uint8_t sectorCount)
{
    uint8_t *taskMemPtr = (uint8_t *)allocateKernelPage(2);

    TCB *tcbPtr    = (TCB *)taskMemPtr;
    uint32_t vCR3  = (uint32_t)(taskMemPtr + 0x1000);
    uint32_t pCR3  = *(uint32_t *)(0xffc00000 | (vCR3 >> 12 << 2)) & 0xfffff000;
    uint32_t *ESP0 = (uint32_t *)((uint32_t)tcbPtr + 0x1000 - 17 * 4);
    uint32_t curCR3;

    tcbPtr->__CR3       = pCR3;
    tcbPtr->__ESP0      = (uint32_t)ESP0;
    tcbPtr->__taskState = __TASK_READY;

    memcpy((void *)(vCR3 + 0xc00), (void *)0xfffffc00, 255 * 4);
    ((uint32_t *)vCR3)[1023] = pCR3 | 0x3;

    __asm__ __volatile__(
        "mov %%cr3, %0\n\t"
        "mov %1, %%cr3\n\t"
        : "=&r"(curCR3)
        : "r"(pCR3)
    );

    uint8_t *elfBuf = (uint8_t *)0xc0080000;

    hdRead(elfBuf, startSector, sectorCount);

    uint32_t EIP     = *(uint32_t *)(elfBuf + 0x18);
    uint8_t *phPtr   = elfBuf + *(uint32_t *)(elfBuf + 0x1c);
    uint32_t phSize  = *(uint16_t *)(elfBuf + 0x2a);
    uint32_t phCount = *(uint16_t *)(elfBuf + 0x2c);

    for (uint32_t _ = 0; _ < phCount; _++, phPtr += phSize)
    {
        if (*(uint32_t *)phPtr == 0x1)
        {
            void *srcPtr      = elfBuf + *(uint32_t *)(phPtr + 0x4);
            void *tarPtr      = *(void **)(phPtr + 0x8);
            uint32_t fileSize = *(uint32_t *)(phPtr + 0x10);
            uint32_t memSize  = *(uint32_t *)(phPtr + 0x14);

            installTaskPage(tarPtr, memSize);
            memcpy(tarPtr, srcPtr, fileSize);
        }
    }

    ESP0[0]  = 0;
    ESP0[1]  = 0;
    ESP0[2]  = 0;
    ESP0[3]  = 0;
    ESP0[4]  = 0;
    ESP0[5]  = 0;
    ESP0[6]  = 0;
    ESP0[7]  = 0;
    ESP0[8]  = (4 << 3) | 0x3;
    ESP0[9]  = (4 << 3) | 0x3;
    ESP0[10] = (4 << 3) | 0x3;
    ESP0[11] = (4 << 3) | 0x3;
    ESP0[12] = EIP;
    ESP0[13] = (3 << 3) | 0x3;
    ESP0[14] = __getEFLAGS() | 0x200;
    ESP0[15] = 0xc0000000;
    ESP0[16] = (4 << 3) | 0x3;

    installTaskPage((void *)(0xc0000000 - 0x1000), 0x1000);

    __asm__ __volatile__("mov %0, %%cr3":: "r"(curCR3));

    queuePush(&__taskQueue, (Node *)tcbPtr);
}


TCB *getNextTask()
{
    if ((uint32_t)curTask != __KERNEL_TASK_ADDR)
    {
        queuePush(&__taskQueue, (Node *)curTask);
    }

    for (uint32_t _ = 0, queueSize = queueGetSize(&__taskQueue); _ < queueSize; _++)
    {
        TCB *nextTask = (TCB *)queuePop(&__taskQueue);

        switch (nextTask->__taskState)
        {
            case __TASK_READY:
                return curTask = nextTask;
                break;

            case __TASK_EXIT:
                deallocateKernelPage(nextTask, 2);
                break;

            case __TASK_BLOCK:
                queuePush(&__taskQueue, (Node *)nextTask);
                break;

            default:
                printf("Invalid task state\n");
                __asm__ __volatile__("cli; hlt");
                break;
        }
    }

    return curTask = (TCB *)__KERNEL_TASK_ADDR;
}


void taskExit()
{
    deallocateTaskCR3();

    curTask->__taskState   = __TASK_EXIT;
    shellTask->__taskState = __TASK_READY;

    __asm__ __volatile__("jmp __taskSwitch");
}
