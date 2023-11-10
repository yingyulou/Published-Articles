#pragma once

#include "Memory.h"
#include "Bitmap.h"
#include "Task.h"
#include "Util.h"

Bitmap __pMemoryBitmap;

void memoryInit()
{
    bitmapInit(&__pMemoryBitmap, (uint8_t *)0xc009e000, 0x8000, true);
}


void memcpy(void *tarPtr, const void *srcPtr, uint32_t memSize)
{
    __asm__ __volatile__(
        "rep movsb\n\t"
        :
        : "D"(tarPtr), "S"(srcPtr), "c"(memSize)
        : "memory"
    );
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
    return (void *)__allocatePage(&((TCB *)0xc009f000)->vMemoryBitmap, &__pMemoryBitmap, pageCount, 0xc0100000, 0x200000);
}


void installTaskPage(uint32_t vAddr, uint32_t pageCount)
{
    Bitmap *vMemoryBitmapPtr = &getTCB()->vMemoryBitmap;

    if (vAddr / 0x1000 + pageCount <= vMemoryBitmapPtr->__size)
    {
        for (int idx = 0; idx < pageCount; idx++)
        {
            bitmapSet(vMemoryBitmapPtr, vAddr / 0x1000 + idx, 1);
        }
    }

    for (int idx = 0; idx < pageCount; idx++)
    {
        uint32_t pAddr = __allocateAddr(&__pMemoryBitmap, 0x200000, 1);

        __installPage(vAddr + idx * 0x1000, pAddr, &__pMemoryBitmap, 0x200000);
    }
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
    __deallocatePage((uint32_t)vAddr, &((TCB *)0xc009f000)->vMemoryBitmap, &__pMemoryBitmap, pageCount, 0xc0100000, 0x200000);
}


void deallocateTaskCR3()
{
    for (int i = 0; i < 768; i++)
    {
        uint32_t *pdePtr = (uint32_t *)(0xfffff000 | (i << 2));

        if (*pdePtr & 0x1)
        {
            for (int j = 0; j < 1024; j++)
            {
                uint32_t *ptePtr = (uint32_t *)(0xffc00000 | (i << 12) | (j << 2));

                if (*ptePtr & 0x1)
                {
                    __deallocateAddr(*ptePtr & 0xfffff000, &__pMemoryBitmap, 0x200000, 1);
                }
            }

            __deallocateAddr(*pdePtr & 0xfffff000, &__pMemoryBitmap, 0x200000, 1);
        }
    }
}
