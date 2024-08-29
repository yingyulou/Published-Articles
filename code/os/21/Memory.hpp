#pragma once

#include "Memory.h"
#include "Bitmap.h"
#include "Util.h"

#define __V_START_ADDR 0xffff800000200000
#define __P_START_ADDR 0x200000

Bitmap __vBitmap, __pBitmap;
uint8_t __memoryBitmapBuf[0x2000];

void memoryInit()
{
    bitmapInit(&__vBitmap, __memoryBitmapBuf, 0x8000);
    bitmapInit(&__pBitmap, __memoryBitmapBuf + 0x1000, 0x8000);
}


uint64_t __allocateAddr(Bitmap *memoryBitmap, uint64_t startAddr, uint64_t pageCount)
{
    return startAddr + (bitmapAllocate(memoryBitmap, pageCount) << 12);
}


void __installPage(uint64_t vAddr, uint64_t pAddr)
{
    uint64_t *pml4Ptr  = (uint64_t *)((vAddr >> 39 << 3) | 0xfffffffffffff000);
    uint64_t *pdptePtr = (uint64_t *)((vAddr >> 30 << 3) | 0xffffffffffe00000);
    uint64_t *pdePtr   = (uint64_t *)((vAddr >> 21 << 3) | 0xffffffffc0000000);
    uint64_t *ptePtr   = (uint64_t *)((vAddr >> 12 << 3) | 0xffffff8000000000);

    if (!(*pml4Ptr & 0x1))
    {
        *pml4Ptr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint64_t)pdptePtr & 0xfffffffffffff000), 0, 0x1000);
    }

    if (!(*pdptePtr & 0x1))
    {
        *pdptePtr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint64_t)pdePtr & 0xfffffffffffff000), 0, 0x1000);
    }

    if (!(*pdePtr & 0x1))
    {
        *pdePtr = __allocateAddr(&__pBitmap, __P_START_ADDR, 1) | 0x7;
        memset((void *)((uint64_t)ptePtr & 0xfffffffffffff000), 0, 0x1000);
    }

    *ptePtr = pAddr | 0x7;

    __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
}


void *allocateKernelPage(uint64_t pageCount)
{
    uint64_t vAddr = __allocateAddr(&__vBitmap, __V_START_ADDR, pageCount);

    for (uint64_t idx = 0; idx < pageCount; idx++)
    {
        __installPage(vAddr + (idx << 12), __allocateAddr(&__pBitmap, __P_START_ADDR, 1));
    }

    memset((void *)vAddr, 0, pageCount << 12);

    return (void *)vAddr;
}


void installTaskPage(Bitmap *vBitmap, uint64_t startAddr, uint64_t memorySize)
{
    uint64_t endAddr = (startAddr + memorySize + 0x1000 - 1) & 0xfffffffffffff000;

    startAddr &= 0xfffffffffffff000;

    uint64_t pageCount = (endAddr - startAddr) >> 12;
    uint64_t startIdx  = startAddr >> 12;

    for (uint64_t pageIdx = 0; pageIdx < pageCount; pageIdx++)
    {
        if (startIdx + pageIdx < vBitmap->__size)
        {
            bitmapSet(vBitmap, startIdx + pageIdx, 1);
        }

        __installPage(startAddr + (pageIdx << 12), __allocateAddr(&__pBitmap, __P_START_ADDR, 1));
    }

    memset((void *)startAddr, 0, pageCount << 12);
}


void __deallocateAddr(Bitmap *memoryBitmap, uint64_t startAddr, uint64_t pageCount, uint64_t memoryAddr)
{
    bitmapDeallocate(memoryBitmap, (memoryAddr - startAddr) >> 12, pageCount);
}


void deallocateKernelPage(void *vPtr, uint64_t pageCount)
{
    __deallocateAddr(&__vBitmap, __V_START_ADDR, pageCount, (uint64_t)vPtr);

    for (uint64_t pageIdx = 0; pageIdx < pageCount; pageIdx++)
    {
        uint64_t vAddr   = (uint64_t)vPtr + (pageIdx << 12);
        uint64_t *ptePtr = (uint64_t *)((vAddr >> 12 << 3) | 0xffffff8000000000);
        uint64_t pAddr   = *ptePtr & 0xfffffffffffff000;

        *ptePtr = 0;
        __deallocateAddr(&__pBitmap, __P_START_ADDR, 1, pAddr);

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
