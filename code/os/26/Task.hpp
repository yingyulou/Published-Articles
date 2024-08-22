#pragma once

#include "Task.h"
#include "Bitmap.h"
#include "Queue.h"
#include "Memory.h"
#include "HD.h"
#include "Util.h"

#define __CPU_COUNT 4

Queue taskQueue, exitQueue;

void __tssInit()
{
    memset((void *)0xffff800000092000, 0, __CPU_COUNT * 128);

    for (uint64_t idx = 0; idx < __CPU_COUNT; idx++)
    {
        *(uint16_t *)(0xffff800000092000 + idx * 128 + 102) = 103;
    }

    __asm__ __volatile__("ltr %w0":: "r"(7 << 3));
    __asm__ __volatile__("wrmsr":: "c"(0xc0000101), "a"(0x00092000), "d"(0xffff8000));
}


void __kernelTaskInit()
{
    TCB *tcbPtr = (TCB *)0xffff80000009f000;

    tcbPtr->__CR3       = 0x100000;
    tcbPtr->__taskQueue = &taskQueue;
}


void taskInit()
{
    queueInit(&taskQueue);
    queueInit(&exitQueue);

    __tssInit();
    __kernelTaskInit();
}


TCB *getTCB()
{
    register uint64_t RSP0 __asm__("rsp");

    return (TCB *)((RSP0 - 1) & 0xfffffffffffff000);
}


uint64_t __getRFLAGS()
{
    uint64_t RFLAGS;

    __asm__ __volatile__("pushf; btsq $9, (%%rsp); pop %0": "=g"(RFLAGS));

    return RFLAGS;
}


void loadTaskPL0(void *RIP)
{
    uint8_t *taskPtr = (uint8_t *)allocateKernelPage(3);

    TCB *tcbPtr         = (TCB *)taskPtr;
    uint64_t *vCR3      = (uint64_t *)(taskPtr + 0x1000);
    uint64_t pCR3       = *(uint64_t *)(((uint64_t)vCR3 >> 12 << 3) | 0xffffff8000000000) & 0xfffffffffffff000;
    uint8_t *vBitmapBuf = taskPtr + 0x2000;
    uint64_t *RSP0      = (uint64_t *)((uint8_t *)tcbPtr + 0x1000 - 20 * 8);

    tcbPtr->__CR3       = pCR3;
    tcbPtr->__RSP0      = (uint64_t)RSP0;
    tcbPtr->__taskQueue = &taskQueue;
    bitmapInit(&tcbPtr->__vBitmap, vBitmapBuf, 0x8000);

    memcpy(vCR3 + 256, (void *)0xfffffffffffff800, 255 * 8);
    vCR3[511] = pCR3 | 0x3;

    RSP0[0x00] = 0;
    RSP0[0x01] = 0;
    RSP0[0x02] = 0;
    RSP0[0x03] = 0;
    RSP0[0x04] = 0;
    RSP0[0x05] = 0;
    RSP0[0x06] = 0;
    RSP0[0x07] = 0;
    RSP0[0x08] = 0;
    RSP0[0x09] = 0;
    RSP0[0x0a] = 0;
    RSP0[0x0b] = 0;
    RSP0[0x0c] = 0;
    RSP0[0x0d] = 0;
    RSP0[0x0e] = 0;
    RSP0[0x0f] = (uint64_t)RIP;
    RSP0[0x10] = 3 << 3;
    RSP0[0x11] = __getRFLAGS();
    RSP0[0x12] = (uint64_t)((uint8_t *)tcbPtr + 0x1000);
    RSP0[0x13] = 4 << 3;

    queuePush(&taskQueue, (Node *)tcbPtr);
}


void loadTaskPL3(uint32_t startSector, uint8_t sectorCount)
{
    uint8_t *taskPtr = (uint8_t *)allocateKernelPage(3);

    TCB *tcbPtr         = (TCB *)taskPtr;
    uint64_t *vCR3      = (uint64_t *)(taskPtr + 0x1000);
    uint64_t pCR3       = *(uint64_t *)(((uint64_t)vCR3 >> 12 << 3) | 0xffffff8000000000) & 0xfffffffffffff000;
    uint8_t *vBitmapBuf = taskPtr + 0x2000;
    uint64_t *RSP0      = (uint64_t *)((uint8_t *)tcbPtr + 0x1000 - 20 * 8);

    tcbPtr->__CR3       = pCR3;
    tcbPtr->__RSP0      = (uint64_t)RSP0;
    tcbPtr->__taskQueue = &taskQueue;
    bitmapInit(&tcbPtr->__vBitmap, vBitmapBuf, 0x8000);

    memcpy(vCR3 + 256, (void *)0xfffffffffffff800, 255 * 8);
    vCR3[511] = pCR3 | 0x3;

    uint64_t curCR3;

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
            uint8_t *srcPtr   = elfBuf + *(uint64_t *)(phPtr + 0x8);
            uint8_t *tarPtr   = *(uint8_t **)(phPtr + 0x10);
            uint64_t fileSize = *(uint64_t *)(phPtr + 0x20);
            uint64_t memSize  = *(uint64_t *)(phPtr + 0x28);

            installTaskPage(&tcbPtr->__vBitmap, (uint64_t)tarPtr, memSize);
            memcpy(tarPtr, srcPtr, fileSize);
        }
    }

    RSP0[0x00] = 0;
    RSP0[0x01] = 0;
    RSP0[0x02] = 0;
    RSP0[0x03] = 0;
    RSP0[0x04] = 0;
    RSP0[0x05] = 0;
    RSP0[0x06] = 0;
    RSP0[0x07] = 0;
    RSP0[0x08] = 0;
    RSP0[0x09] = 0;
    RSP0[0x0a] = 0;
    RSP0[0x0b] = 0;
    RSP0[0x0c] = 0;
    RSP0[0x0d] = 0;
    RSP0[0x0e] = 0;
    RSP0[0x0f] = (uint64_t)RIP;
    RSP0[0x10] = (6 << 3) | 0x3;
    RSP0[0x11] = __getRFLAGS();
    RSP0[0x12] = 0x800000000000;
    RSP0[0x13] = (5 << 3) | 0x3;

    installTaskPage(&tcbPtr->__vBitmap, 0x800000000000 - 0x1000, 0x1000);

    __asm__ __volatile__("mov %0, %%cr3":: "r"(curCR3));

    queuePush(&taskQueue, (Node *)tcbPtr);
}


void deleteTask()
{
    while (!queueEmpty(&exitQueue))
    {
        deallocateKernelPage(queuePop(&exitQueue), 3);
    }
}
