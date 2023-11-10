#pragma once

#include "Task.h"
#include "Bitmap.h"
#include "Queue.h"
#include "Memory.h"
#include "HD.h"
#include "Util.h"

uint8_t TSS[104];

Queue taskQueue;
Queue exitQueue;
Queue shellQueue;

uint64_t __makeTSSDescriptor(uint64_t tssBase, uint64_t tssLimit, uint64_t tssAttr)
{
    return (tssLimit & 0xffff) | ((tssBase & 0xffffff) << 16) | (tssAttr << 32) |
        ((tssLimit & 0xf0000) << 32) | ((tssBase & 0xff000000) << 32);
}


void __installTSS()
{
    *(uint16_t *)(TSS + 8)   = 2 << 3;
    *(uint16_t *)(TSS + 102) = 103;

    uint64_t GDTR;

    __asm__ __volatile__("sgdt %0":: "m"(GDTR));

    ((uint64_t *)(uint32_t)(GDTR >> 16))[5] = __makeTSSDescriptor((uint32_t)TSS, 103, 0x8900);

    __asm__ __volatile__("lgdt %0":: "m"(GDTR));
    __asm__ __volatile__("ltr %w0":: "r"(5 << 3));
}


void __installKernelTask()
{
    TCB *tcbPtr = (TCB *)0xc009f000;

    tcbPtr->CR3       = 0x100000;
    tcbPtr->taskQueue = &taskQueue;

    bitmapInit(&tcbPtr->vMemoryBitmap, (uint8_t *)0xc009d000, 0x8000, true);
}


void taskInit()
{
    queueInit(&taskQueue);
    queueInit(&exitQueue);
    queueInit(&shellQueue);

    __installTSS();
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

    tcbPtr->CR3       = pCR3;
    tcbPtr->taskQueue = &taskQueue;

    memset((void *)vCR3, 0, 0x1000);
    memcpy((void *)(vCR3 + 0xc00), (void *)0xfffffc00, 255 * 4);

    ((uint32_t *)vCR3)[1023] = pCR3 | 0x3;

    tcbPtr->ESP0 = (uint32_t)tcbPtr + 0x1000 - 15 * 4;

    uint32_t *ESP0 = (uint32_t *)tcbPtr->ESP0;

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
    ESP0[12] = EIP;
    ESP0[13] = 1 << 3;
    ESP0[14] = __getEFLAGS() | 0x200;

    bitmapInit(&tcbPtr->vMemoryBitmap, vMemoryBitmap, 0x8000, true);

    queuePush(&taskQueue, (Node *)tcbPtr);
}


void loadTaskPL3(uint32_t startSector, uint32_t sectorCount)
{
    uint8_t *taskMemoryPtr = (uint8_t *)allocateKernelPage(3);

    TCB *tcbPtr            = (TCB *)taskMemoryPtr;
    uint32_t vCR3          = (uint32_t)(taskMemoryPtr + 0x1000);
    uint32_t pCR3          = (*(uint32_t *)(0xffc00000 | (vCR3 >> 12 << 2))) & 0xfffff000;
    uint8_t *vMemoryBitmap = taskMemoryPtr + 0x2000;

    tcbPtr->CR3       = pCR3;
    tcbPtr->taskQueue = &taskQueue;

    memset((void *)vCR3, 0, 0x1000);
    memcpy((void *)(vCR3 + 0xc00), (void *)0xfffffc00, 255 * 4);

    ((uint32_t *)vCR3)[1023] = pCR3 | 0x3;

    uint32_t pageCount = (sectorCount * 512 + 0x1000 - 1) / 0x1000;
    uint8_t *elfBuf    = (uint8_t *)allocateKernelPage(pageCount);

    hdRead(elfBuf, startSector, sectorCount);

    uint8_t *segPtr   = (uint8_t *)(elfBuf + *(uint32_t *)(elfBuf + 28));
    uint16_t segSize  = *(uint16_t *)(elfBuf + 42);
    uint16_t segCount = *(uint16_t *)(elfBuf + 44);

    uint32_t curCR3;

    __asm__ __volatile__(
        "mov %%cr3, %0\n\t"
        "mov %1, %%cr3\n\t"
        : "=&r"(curCR3)
        : "r"(pCR3)
    );

    for (int _ = 0; _ < segCount; _++, segPtr += segSize)
    {
        if (*(uint32_t *)segPtr == 1)
        {
            void *srcPtr = elfBuf + *(uint32_t *)(segPtr + 4);
            void *tarPtr = (void *)*(uint32_t *)(segPtr + 8);

            uint32_t pageCount = (*(uint32_t *)(segPtr + 20) + 0x1000 - 1) / 0x1000;

            installTaskPage((uint32_t)tarPtr, pageCount);

            memset(tarPtr, 0, pageCount * 0x1000);
            memcpy(tarPtr, srcPtr, *(uint32_t *)(segPtr + 16));
        }
    }

    uint32_t EIP = *(uint32_t *)(elfBuf + 24);

    deallocateKernelPage(elfBuf, pageCount);

    tcbPtr->ESP0 = (uint32_t)tcbPtr + 0x1000 - 17 * 4;

    uint32_t *ESP0 = (uint32_t *)tcbPtr->ESP0;

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

    installTaskPage(0xc0000000 - 0x1000, 1);

    __asm__ __volatile__(
        "mov %0, %%cr3\n\t"
        :
        : "r"(curCR3)
    );

    bitmapInit(&tcbPtr->vMemoryBitmap, vMemoryBitmap, 0x8000, true);

    queuePush(&taskQueue, (Node *)tcbPtr);

    getTCB()->taskQueue = &shellQueue;

    __asm__ __volatile__("int $0x20");
}


void deleteTask()
{
    while (!queueEmpty(&exitQueue))
    {
        deallocateKernelPage(queuePop(&exitQueue), 3);
    }
}
