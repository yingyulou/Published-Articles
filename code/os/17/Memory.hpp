#pragma once

#include "Memory.h"
#include "Bitmap.h"
#include "Util.h"

#define __V_START_ADDR 0xc0100000
#define __P_START_ADDR 0x200000

Bitmap __vBitmap, __pBitmap;
uint8_t __memBitmapBuf[0x2000];

void memoryInit()
{
    bitmapInit(&__vBitmap, __memBitmapBuf);
    bitmapInit(&__pBitmap, __memBitmapBuf + 0x1000);
}


uint32_t __allocateAddr(Bitmap *memBitmap, uint32_t startAddr, uint32_t pageCount)
{
    return startAddr + bitmapAllocate(memBitmap, pageCount) * 0x1000;
}


void __installPage(uint32_t vAddr, uint32_t pAddr)
{
    uint32_t *pdePtr = (uint32_t *)(0xfffff000 | (vAddr >> 22 << 2));
    uint32_t *ptePtr = (uint32_t *)(0xffc00000 | (vAddr >> 12 << 2));

    if (!(*pdePtr & 0x1))
    {
        *pdePtr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint32_t)ptePtr & 0xfffff000), 0x0, 0x1000);
    }

    *ptePtr = pAddr | 0x7;

    __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
}


void __installMemory(uint32_t startAddr, uint32_t pageCount)
{
    for (uint32_t pageIdx = 0; pageIdx < pageCount; pageIdx++)
    {
        __installPage(startAddr + pageIdx * 0x1000, __allocateAddr(&__pBitmap, __P_START_ADDR, 1));
    }

    memset((void *)startAddr, 0x0, pageCount * 0x1000);
}


void *allocateKernelPage(uint32_t pageCount)
{
    uint32_t startAddr = __allocateAddr(&__vBitmap, __V_START_ADDR, pageCount);

    __installMemory(startAddr, pageCount);

    return (void *)startAddr;
}


void installTaskPage(void *startPtr, uint32_t memSize)
{
    uint32_t startAddr = (uint32_t)startPtr & 0xfffff000;
    uint32_t endAddr   = ((uint32_t)startPtr + memSize + 0x1000 - 0x1) & 0xfffff000;
    uint32_t pageCount = (endAddr - startAddr) / 0x1000;

    __installMemory(startAddr, pageCount);
}


void __deallocateAddr(Bitmap *memBitmap, uint32_t startAddr, uint32_t pageCount, uint32_t memAddr)
{
    bitmapDeallocate(memBitmap, (memAddr - startAddr) / 0x1000, pageCount);
}


void deallocateKernelPage(void *startPtr, uint32_t pageCount)
{
    uint32_t startAddr = (uint32_t)startPtr;

    __deallocateAddr(&__vBitmap, __V_START_ADDR, pageCount, startAddr);

    for (uint32_t pageIdx = 0; pageIdx < pageCount; pageIdx++)
    {
        uint32_t vAddr   = startAddr + pageIdx * 0x1000;
        uint32_t *ptePtr = (uint32_t *)(0xffc00000 | (vAddr >> 12 << 2));
        uint32_t pAddr   = *ptePtr & 0xfffff000;

        __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, pAddr);

        *ptePtr = 0x0;
        __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
    }
}


void deallocateTaskCR3()
{
    for (uint32_t pdeIdx = 0; pdeIdx < 768; pdeIdx++)
    {
        uint32_t *pdePtr = (uint32_t *)(0xfffff000 | (pdeIdx << 2));

        if (*pdePtr & 0x1)
        {
            for (uint32_t pteIdx = 0; pteIdx < 1024; pteIdx++)
            {
                uint32_t *ptePtr = (uint32_t *)(0xffc00000 | (pdeIdx << 12) | (pteIdx << 2));

                if (*ptePtr & 0x1)
                {
                    __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, *ptePtr & 0xfffff000);
                }
            }

            __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, *pdePtr & 0xfffff000);
        }
    }
}
