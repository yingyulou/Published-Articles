#pragma once

#include "FS.h"
#include "Bitmap.h"
#include "Print.h"
#include "Task.h"
#include "HD.h"
#include "Util.h"

#define __FS_SUPER_BLOCK  98
#define __FS_BITMAP_BLOCK ((__FS_SUPER_BLOCK) + 1)
#define __FS_START_BLOCK  ((__FS_BITMAP_BLOCK) + 1)
#define __FS_MAGIC        0x66666666
#define __FS_FCB_COUNT    (512 / sizeof(FCB) - 1)

FCB __fcbList[__FS_FCB_COUNT + 1];
uint8_t __hdBitmapBuf[512];
Bitmap __hdBitmap;

void fsInit()
{
    hdRead(__fcbList, __FS_SUPER_BLOCK, 1);

    if (*(uint32_t *)(__fcbList + __FS_FCB_COUNT) == __FS_MAGIC)
    {
        hdRead(__hdBitmapBuf, __FS_BITMAP_BLOCK, 1);

        bitmapInit(&__hdBitmap, __hdBitmapBuf);
    }
    else
    {
        memset(__fcbList, 0, 512);
        *(uint32_t *)(__fcbList + __FS_FCB_COUNT) = __FS_MAGIC;
        bitmapInit(&__hdBitmap, __hdBitmapBuf);

        hdWrite(__fcbList, __FS_SUPER_BLOCK, 1);
        hdWrite(__hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
    }
}


void fsList()
{
    for (uint32_t idx = 0; idx < __FS_FCB_COUNT; idx++)
    {
        if (__fcbList[idx].__startSector)
        {
            printf("%s %d %d\n", __fcbList[idx].__fileName, __fcbList[idx].__startSector, __fcbList[idx].__sectorCount);
        }
    }
}


uint32_t __allocateFCB()
{
    for (uint32_t idx = 0; idx < __FS_FCB_COUNT; idx++)
    {
        if (!__fcbList[idx].__startSector)
        {
            return idx;
        }
    }

    return -1;
}


void fsCreate(const char *fileName, uint32_t startSector, uint8_t sectorCount)
{
    uint32_t fcbIdx = __allocateFCB();

    if (fcbIdx == -1)
    {
        return;
    }

    strcpy(__fcbList[fcbIdx].__fileName, fileName, sizeof(__fcbList[fcbIdx].__fileName) - 1);
    __fcbList[fcbIdx].__startSector = bitmapAllocate(&__hdBitmap, sectorCount) + __FS_START_BLOCK;
    __fcbList[fcbIdx].__sectorCount = sectorCount;

    uint8_t hdBuf[512];

    for (uint32_t idx = 0; idx < sectorCount; idx++)
    {
        hdRead(hdBuf, startSector + idx, 1);
        hdWrite(hdBuf, __fcbList[fcbIdx].__startSector + idx, 1);
    }

    hdWrite(__fcbList, __FS_SUPER_BLOCK, 1);
    hdWrite(__hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
}


uint32_t __findFCB(const char *fileName)
{
    for (uint32_t idx = 0; idx < __FS_FCB_COUNT; idx++)
    {
        if (__fcbList[idx].__startSector && strcmp(__fcbList[idx].__fileName, fileName))
        {
            return idx;
        }
    }

    return -1;
}


void fsDelete(const char *fileName)
{
    uint32_t fcbIdx = __findFCB(fileName);

    if (fcbIdx == -1)
    {
        return;
    }

    bitmapDeallocate(&__hdBitmap, __fcbList[fcbIdx].__startSector - __FS_START_BLOCK, __fcbList[fcbIdx].__sectorCount);
    memset(__fcbList + fcbIdx, 0, sizeof(__fcbList[fcbIdx]));

    hdWrite(__fcbList, __FS_SUPER_BLOCK, 1);
    hdWrite(__hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
}


void fsLoad(const char *fileName)
{
    uint32_t fcbIdx = __findFCB(fileName);

    if (fcbIdx != -1)
    {
        loadTaskPL3(__fcbList[fcbIdx].__startSector, __fcbList[fcbIdx].__sectorCount);
    }
}
