#pragma once

#include "Memory.h"
#include "Bitmap.h"
#include "Util.h"

Bitmap __pMemoryBitmap;
Bitmap __vMemoryBitmap;

void memoryInit()
{
    bitmapInit(&__pMemoryBitmap, (uint8_t *)0x8000, 0x8000, true);
    bitmapInit(&__vMemoryBitmap, (uint8_t *)0x9000, 0x8000, true);
}


void memset(void *tarPtr, uint8_t setVal, uint32_t memSize)
{
    __asm__ __volatile__(
        "rep stosb\n\t"
        :
        : "D"(tarPtr), "a"(setVal), "c"(memSize)
        : "memory"
    );
}


uint32_t __allocateAddr(Bitmap *memoryBitmapPtr, uint32_t startAddr, uint32_t pageCount)
{
    return startAddr + (bitmapAllocate(memoryBitmapPtr, pageCount) << 12);
}


void __installPage(uint32_t vAddr, uint32_t pAddr, Bitmap *pMemoryBitmapPtr, uint32_t pStartAddr)
{
    uint32_t *pdePtr = (uint32_t *)(0xfffff000 | (vAddr >> 22 << 2));
    uint32_t *ptePtr = (uint32_t *)(0xffc00000 | (vAddr >> 12 << 2));

    if (!(*pdePtr & 1))
    {
        *pdePtr = __allocateAddr(pMemoryBitmapPtr, pStartAddr, 1) | 0x7;
        memset((void *)((uint32_t)ptePtr & 0xfffff000), 0, 0x1000);
    }

    *ptePtr = pAddr | 0x7;

    __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
}


uint32_t __allocatePage(Bitmap *vMemoryBitmapPtr, Bitmap *pMemoryBitmapPtr, uint32_t pageCount,
    uint32_t vStartAddr, uint32_t pStartAddr)
{
    uint32_t vAddr = __allocateAddr(vMemoryBitmapPtr, vStartAddr, pageCount);

    for (int idx = 0; idx < pageCount; idx++)
    {
        uint32_t pAddr = __allocateAddr(pMemoryBitmapPtr, pStartAddr, 1);

        __installPage(vAddr + idx * 0x1000, pAddr, pMemoryBitmapPtr, pStartAddr);
    }

    return vAddr;
}


void *allocateKernelPage(uint32_t pageCount)
{
    return (void *)__allocatePage(&__vMemoryBitmap, &__pMemoryBitmap, pageCount, 0x66600000, 0x200000);
}


uint32_t __deallocateAddr(uint32_t deallocateAddr, Bitmap *memoryBitmapPtr, uint32_t startAddr, uint32_t pageCount)
{
    bitmapDeallocate(memoryBitmapPtr, (deallocateAddr - startAddr) / 0x1000, pageCount);
}


void __uninstallPage(uint32_t vAddr)
{
    *(uint32_t *)(0xffc00000 | (vAddr >> 12 << 2)) = 0;

    __asm__ __volatile__("invlpg (%0)":: "r"(vAddr));
}


void __deallocatePage(uint32_t vAddr, Bitmap *vMemoryBitmapPtr, Bitmap *pMemoryBitmapPtr,
    uint32_t pageCount, uint32_t vStartAddr, uint32_t pStartAddr)
{
    __deallocateAddr(vAddr, vMemoryBitmapPtr, vStartAddr, pageCount);

    for (int idx = 0, curAddr = vAddr; idx < pageCount; idx++, curAddr += 0x1000)
    {
        uint32_t pAddr = (*(uint32_t *)(0xffc00000 | (curAddr >> 12 << 2))) & 0xfffff000;

        __deallocateAddr(pAddr, pMemoryBitmapPtr, pStartAddr, 1);
        __uninstallPage(curAddr);
    }
}


void deallocateKernelPage(void *vAddr, uint32_t pageCount)
{
    __deallocatePage((uint32_t)vAddr, &__vMemoryBitmap, &__pMemoryBitmap, pageCount, 0x66600000, 0x200000);
}
