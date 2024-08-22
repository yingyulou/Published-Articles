#pragma once

#include "FS.h"
#include "Bitmap.h"
#include "Print.h"
#include "Task.h"
#include "HD.h"
#include "Util.h"

#define __FS_SUPER_BLOCK  98
#define __FS_BITMAP_BLOCK 99
#define __FS_START_BLOCK  100
#define __FS_MAGIC        0x6666666666666666
#define __FS_FCB_COUNT    15

FCB fcbList[__FS_FCB_COUNT + 1];
uint8_t hdBitmapBuf[512];
Bitmap hdBitmap;

void fsInit()
{
    hdRead(fcbList, __FS_SUPER_BLOCK, 1);

    if (*(uint64_t *)(fcbList + __FS_FCB_COUNT) == __FS_MAGIC)
    {
        hdRead(hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
        bitmapInit(&hdBitmap, hdBitmapBuf, 512 * 8);
    }
    else
    {
        memset(fcbList, 0, 512);
        *(uint64_t *)(fcbList + __FS_FCB_COUNT) = __FS_MAGIC;

        bitmapInit(&hdBitmap, hdBitmapBuf, 512 * 8);

        hdWrite(fcbList, __FS_SUPER_BLOCK, 1);
        hdWrite(hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
    }
}


void fsList()
{
    for (uint64_t idx = 0; idx < __FS_FCB_COUNT; idx++)
    {
        if (fcbList[idx].__startSector)
        {
            printf("%s %d %d\n", fcbList[idx].__fileName, fcbList[idx].__startSector, fcbList[idx].__sectorCount);
        }
    }
}


uint64_t __allocateFCB()
{
    for (uint64_t idx = 0; idx < __FS_FCB_COUNT; idx++)
    {
        if (!fcbList[idx].__startSector)
        {
            return idx;
        }
    }

    return -1;
}


void fsCreate(const char *fileName, uint32_t startSector, uint8_t sectorCount)
{
    uint64_t fcbIdx = __allocateFCB();

    if (fcbIdx == -1)
    {
        return;
    }

    strcpy(fcbList[fcbIdx].__fileName, fileName, 24);
    fcbList[fcbIdx].__startSector = bitmapAllocate(&hdBitmap, sectorCount) + __FS_START_BLOCK;
    fcbList[fcbIdx].__sectorCount = sectorCount;

    uint8_t hdBuf[512];

    for (uint64_t idx = 0; idx < sectorCount; idx++)
    {
        hdRead(hdBuf, startSector + idx, 1);
        hdWrite(hdBuf, fcbList[fcbIdx].__startSector + idx, 1);
    }

    hdWrite(fcbList, __FS_SUPER_BLOCK, 1);
    hdWrite(hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
}


uint64_t __findFCB(const char *fileName)
{
    for (uint64_t idx = 0; idx < __FS_FCB_COUNT; idx++)
    {
        if (fcbList[idx].__startSector && strcmp(fcbList[idx].__fileName, fileName))
        {
            return idx;
        }
    }

    return -1;
}


void fsDelete(const char *fileName)
{
    uint64_t fcbIdx = __findFCB(fileName);

    if (fcbIdx == -1)
    {
        return;
    }

    bitmapDeallocate(&hdBitmap, fcbList[fcbIdx].__startSector - __FS_START_BLOCK, fcbList[fcbIdx].__sectorCount);

    memset(fcbList + fcbIdx, 0, 32);

    hdWrite(fcbList, __FS_SUPER_BLOCK, 1);
    hdWrite(hdBitmapBuf, __FS_BITMAP_BLOCK, 1);
}


void fsLoad(const char *fileName)
{
    uint64_t fcbIdx = __findFCB(fileName);

    if (fcbIdx != -1)
    {
        loadTaskPL3(fcbList[fcbIdx].__startSector, fcbList[fcbIdx].__sectorCount);
    }
}
