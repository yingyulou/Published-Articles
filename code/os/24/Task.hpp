#pragma once

#include "Task.h"
#include "Queue.h"
#include "Memory.h"
#include "HD.h"
#include "Util.h"

alignas(0x8) uint64_t GDT[9];
Queue __taskQueue;

void __makeSegDesc(uint64_t *segDescLow, uint64_t *segDescHigh, uint64_t segBase, uint64_t segLimit, uint64_t segAttr)
{
    *segDescLow = (segLimit & 0xffff) | ((segBase & 0xffffff) << 16) | (segAttr << 40) |
        ((segLimit & 0xf0000) << 32) | ((segBase & 0xff000000) << 32);

    *segDescHigh = segBase >> 32;
}


void __kernelTaskInit()
{
    GDT[0] = 0x0;
    GDT[1] = 0x00cf98000000ffff;
    GDT[2] = 0x00cf92000000ffff;
    GDT[3] = 0x0020980000000000;
    GDT[4] = 0x0020920000000000;
    GDT[5] = 0x0020f20000000000;
    GDT[6] = 0x0020f80000000000;

    TCB *bspTask    = (TCB *)0xffff80000009f000;
    uint8_t *bspTSS = bspTask->__TSS;

    bspTask->__CR3       = 0x100000;
    bspTask->__taskState = __TASK_READY;

    memset(bspTSS, 0x0, 104);
    *(uint16_t *)(bspTSS + 102) = 103;
    __makeSegDesc(GDT + 7, GDT + 8, (uint64_t)bspTSS, 103, 0x89);

    struct { uint16_t _0; void *_1; } __attribute__((__packed__)) GDTR = {sizeof(GDT) - 1, GDT};

    __asm__ __volatile__("lgdt %0":: "m"(GDTR));
    __asm__ __volatile__("ltr %w0":: "r"(7 << 3));
    __asm__ __volatile__("wrmsr":: "c"(0xc0000101), "a"((uint64_t)bspTSS), "d"((uint64_t)bspTSS >> 32));
}


void taskInit()
{
    queueInit(&__taskQueue);

    __kernelTaskInit();
}


uint64_t __getRFLAGS()
{
    uint64_t RFLAGS;

    __asm__ __volatile__("pushf; pop %0": "=g"(RFLAGS));

    return RFLAGS;
}


void loadTaskPL0(void *RIP)
{
    uint8_t *taskMemPtr = (uint8_t *)allocateKernelPage(2);

    TCB *tcbPtr    = (TCB *)taskMemPtr;
    uint64_t vCR3  = (uint64_t)(taskMemPtr + 0x1000);
    uint64_t pCR3  = *(uint64_t *)(0xffffff8000000000 | (vCR3 >> 12 << 3)) & 0xfffffffffffff000;
    uint64_t *RSP0 = (uint64_t *)((uint64_t)tcbPtr + 0x1000 - 20 * 8);

    tcbPtr->__CR3       = pCR3;
    tcbPtr->__RSP0      = (uint64_t)RSP0;
    tcbPtr->__taskState = __TASK_READY;

    memcpy((void *)(vCR3 + 0x800), (void *)0xfffffffffffff800, 255 * 8);
    ((uint64_t *)vCR3)[511] = pCR3 | 0x3;

    RSP0[0]  = 0;
    RSP0[1]  = 0;
    RSP0[2]  = 0;
    RSP0[3]  = 0;
    RSP0[4]  = 0;
    RSP0[5]  = 0;
    RSP0[6]  = 0;
    RSP0[7]  = 0;
    RSP0[8]  = 0;
    RSP0[9]  = 0;
    RSP0[10] = 0;
    RSP0[11] = 0;
    RSP0[12] = 0;
    RSP0[13] = 0;
    RSP0[14] = 0;
    RSP0[15] = (uint64_t)RIP;
    RSP0[16] = 3 << 3;
    RSP0[17] = __getRFLAGS() | 0x200;
    RSP0[18] = (uint64_t)((uint8_t *)tcbPtr + 0x1000);
    RSP0[19] = 4 << 3;

    queuePush(&__taskQueue, (Node *)tcbPtr);
}


void loadTaskPL3(uint32_t startSector, uint8_t sectorCount)
{
    uint8_t *taskMemPtr = (uint8_t *)allocateKernelPage(2);

    TCB *tcbPtr    = (TCB *)taskMemPtr;
    uint64_t vCR3  = (uint64_t)(taskMemPtr + 0x1000);
    uint64_t pCR3  = *(uint64_t *)(0xffffff8000000000 | (vCR3 >> 12 << 3)) & 0xfffffffffffff000;
    uint64_t *RSP0 = (uint64_t *)((uint64_t)tcbPtr + 0x1000 - 20 * 8);
    uint64_t curCR3;

    tcbPtr->__CR3       = pCR3;
    tcbPtr->__RSP0      = (uint64_t)RSP0;
    tcbPtr->__taskState = __TASK_READY;

    memcpy((void *)(vCR3 + 0x800), (void *)0xfffffffffffff800, 255 * 8);
    ((uint64_t *)vCR3)[511] = pCR3 | 0x3;

    __asm__ __volatile__(
        "mov %%cr3, %0\n\t"
        "mov %1, %%cr3\n\t"
        : "=&r"(curCR3)
        : "r"(pCR3)
    );

    uint8_t *elfBuf = (uint8_t *)0xffff800000080000;

    hdRead(elfBuf, startSector, sectorCount);

    uint64_t RIP     = *(uint64_t *)(elfBuf + 0x18);
    uint8_t *phPtr   = elfBuf + *(uint64_t *)(elfBuf + 0x20);
    uint64_t phSize  = *(uint16_t *)(elfBuf + 0x36);
    uint64_t phCount = *(uint16_t *)(elfBuf + 0x38);

    for (uint64_t _ = 0; _ < phCount; _++, phPtr += phSize)
    {
        if (*(uint32_t *)phPtr == 0x1)
        {
            void *srcPtr      = elfBuf + *(uint64_t *)(phPtr + 0x8);
            void *tarPtr      = *(void **)(phPtr + 0x10);
            uint64_t fileSize = *(uint64_t *)(phPtr + 0x20);
            uint64_t memSize  = *(uint64_t *)(phPtr + 0x28);

            installTaskPage(tarPtr, memSize);
            memcpy(tarPtr, srcPtr, fileSize);
        }
    }

    RSP0[0]  = 0;
    RSP0[1]  = 0;
    RSP0[2]  = 0;
    RSP0[3]  = 0;
    RSP0[4]  = 0;
    RSP0[5]  = 0;
    RSP0[6]  = 0;
    RSP0[7]  = 0;
    RSP0[8]  = 0;
    RSP0[9]  = 0;
    RSP0[10] = 0;
    RSP0[11] = 0;
    RSP0[12] = 0;
    RSP0[13] = 0;
    RSP0[14] = 0;
    RSP0[15] = (uint64_t)RIP;
    RSP0[16] = (6 << 3) | 0x3;
    RSP0[17] = __getRFLAGS() | 0x200;
    RSP0[18] = 0x800000000000;
    RSP0[19] = (5 << 3) | 0x3;

    installTaskPage((void *)(0x800000000000 - 0x1000), 0x1000);

    __asm__ __volatile__("mov %0, %%cr3":: "r"(curCR3));

    queuePush(&__taskQueue, (Node *)tcbPtr);
}


TCB *getCurTask()
{
    register uint64_t RSP0 __asm__("rsp");

    return (TCB *)((RSP0 - 1) & 0xfffffffffffff000);
}


TCB *getNextTask()
{
    TCB *curTask = getCurTask();

    queuePush(&__taskQueue, (Node *)curTask);

    for (;;)
    {
        TCB *nextTask = (TCB *)queuePop(&__taskQueue);

        switch (nextTask->__taskState)
        {
            case __TASK_READY:
                return nextTask;
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
}


void taskExit()
{
    deallocateTaskCR3();

    getCurTask()->__taskState = __TASK_EXIT;

    __asm__ __volatile__("jmp __taskSwitch");
}
