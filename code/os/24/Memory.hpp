#pragma once

#include "Memory.h"
#include "Bitmap.h"
#include "Util.h"

#define __V_START_ADDR 0xffff800000200000
#define __P_START_ADDR 0x200000

Bitmap __vBitmap, __pBitmap;
uint8_t __memBitmapBuf[0x2000];

void memoryInit()
{
    bitmapInit(&__vBitmap, __memBitmapBuf);
    bitmapInit(&__pBitmap, __memBitmapBuf + 0x1000);
}


uint64_t __allocateAddr(Bitmap *memBitmap, uint64_t startAddr, uint64_t pageCount)
{
    return startAddr + bitmapAllocate(memBitmap, pageCount) * 0x1000;
}


void __installPage(uint64_t vAddr, uint64_t pAddr)
{
    uint64_t *pml4Ptr  = (uint64_t *)(0xfffffffffffff000 | (vAddr >> 39 << 3));
    uint64_t *pdptePtr = (uint64_t *)(0xffffffffffe00000 | (vAddr >> 30 << 3));
    uint64_t *pdePtr   = (uint64_t *)(0xffffffffc0000000 | (vAddr >> 21 << 3));
    uint64_t *ptePtr   = (uint64_t *)(0xffffff8000000000 | (vAddr >> 12 << 3));

    if (!(*pml4Ptr & 0x1))
    {
        *pml4Ptr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint64_t)pdptePtr & 0xfffffffffffff000), 0x0, 0x1000);
    }

    if (!(*pdptePtr & 0x1))
    {
        *pdptePtr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint64_t)pdePtr & 0xfffffffffffff000), 0x0, 0x1000);
    }

    if (!(*pdePtr & 0x1))
    {
        *pdePtr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint64_t)ptePtr & 0xfffffffffffff000), 0x0, 0x1000);
    }

    *ptePtr = pAddr | 0x7;

    __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
}


void __installMemory(uint64_t startAddr, uint64_t pageCount)
{
    for (uint64_t pageIdx = 0; pageIdx < pageCount; pageIdx++)
    {
        __installPage(startAddr + pageIdx * 0x1000, __allocateAddr(&__pBitmap, __P_START_ADDR, 1));
    }

    memset((void *)startAddr, 0x0, pageCount * 0x1000);
}


void *allocateKernelPage(uint64_t pageCount)
{
    uint64_t startAddr = __allocateAddr(&__vBitmap, __V_START_ADDR, pageCount);

    __installMemory(startAddr, pageCount);

    return (void *)startAddr;
}


void installTaskPage(void *startPtr, uint64_t memSize)
{
    uint64_t startAddr = (uint64_t)startPtr & 0xfffffffffffff000;
    uint64_t endAddr   = ((uint64_t)startPtr + memSize + 0x1000 - 0x1) & 0xfffffffffffff000;
    uint64_t pageCount = (endAddr - startAddr) / 0x1000;

    __installMemory(startAddr, pageCount);
}


void __deallocateAddr(Bitmap *memBitmap, uint64_t startAddr, uint64_t pageCount, uint64_t memAddr)
{
    bitmapDeallocate(memBitmap, (memAddr - startAddr) / 0x1000, pageCount);
}


void deallocateKernelPage(void *startPtr, uint64_t pageCount)
{
    uint64_t startAddr = (uint64_t)startPtr;

    __deallocateAddr(&__vBitmap, __V_START_ADDR, pageCount, startAddr);

    for (uint64_t pageIdx = 0; pageIdx < pageCount; pageIdx++)
    {
        uint64_t vAddr   = startAddr + pageIdx * 0x1000;
        uint64_t *ptePtr = (uint64_t *)(0xffffff8000000000 | (vAddr >> 12 << 3));
        uint64_t pAddr   = *ptePtr & 0xfffffffffffff000;

        __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, pAddr);

        *ptePtr = 0x0;
        __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
    }
}


void deallocateTaskCR3()
{
    for (uint64_t pml4Idx = 0; pml4Idx < 256; pml4Idx++)
    {
        uint64_t *pml4Ptr = (uint64_t *)(0xfffffffffffff000 | (pml4Idx << 3));

        if (*pml4Ptr & 0x1)
        {
            for (uint64_t pdpteIdx = 0; pdpteIdx < 512; pdpteIdx++)
            {
                uint64_t *pdptePtr = (uint64_t *)(0xffffffffffe00000 | (pml4Idx << 12) | (pdpteIdx << 3));

                if (*pdptePtr & 0x1)
                {
                    for (uint64_t pdeIdx = 0; pdeIdx < 512; pdeIdx++)
                    {
                        uint64_t *pdePtr = (uint64_t *)(0xffffffffc0000000 | (pml4Idx << 21) | (pdpteIdx << 12) | (pdeIdx << 3));

                        if (*pdePtr & 0x1)
                        {
                            for (uint64_t pteIdx = 0; pteIdx < 512; pteIdx++)
                            {
                                uint64_t *ptePtr = (uint64_t *)(0xffffff8000000000 | (pml4Idx << 30) | (pdpteIdx << 21) | (pdeIdx << 12) | (pteIdx << 3));

                                if (*ptePtr & 0x1)
                                {
                                    __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, *ptePtr & 0xfffffffffffff000);
                                }
                            }

                            __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, *pdePtr & 0xfffffffffffff000);
                        }
                    }

                    __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, *pdptePtr & 0xfffffffffffff000);
                }
            }

            __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, *pml4Ptr & 0xfffffffffffff000);
        }
    }
}
