#pragma once

#include "FS.h"
#include "Print.h"
#include "Task.h"
#include "HD.h"
#include "Util.h"

FCB fcbList[16];
uint8_t hdBitmapBuf[512];
Bitmap hdBitmap;

void fsInit()
{
    hdRead(fcbList, 100, 1);

    if (*(uint32_t *)(fcbList + 15) == 0x66666666)
    {
        hdRead(hdBitmapBuf, 101, 1);
        bitmapInit(&hdBitmap, hdBitmapBuf, 512 * 8, false);
    }
    else
    {
        memset(fcbList, 0, 512);
        *(uint32_t *)(fcbList + 15) = 0x66666666;

        bitmapInit(&hdBitmap, hdBitmapBuf, 512 * 8, true);

        hdWrite(fcbList, 100, 1);
        hdWrite(hdBitmapBuf, 101, 1);
    }
}


void fsList()
{
    for (int idx = 0; idx < 15; idx++)
    {
        if (fcbList[idx].__startSector)
        {
            printf("%s %d %d\n", fcbList[idx].__fileName, fcbList[idx].__startSector, fcbList[idx].__sectorCount);
        }
    }
}


uint32_t __allocateFCB()
{
    for (int idx = 0; idx < 15; idx++)
    {
        if (!fcbList[idx].__startSector)
        {
            return idx;
        }
    }

    return -1;
}


void fsCreate(const char *fileName, uint32_t startSector, uint32_t sectorCount)
{
    uint32_t fcbIdx = __allocateFCB();

    if (fcbIdx == -1)
    {
        return;
    }

    strCopy(fcbList[fcbIdx].__fileName, fileName, 24);
    fcbList[fcbIdx].__startSector = bitmapAllocate(&hdBitmap, sectorCount) + 102;
    fcbList[fcbIdx].__sectorCount = sectorCount;

    uint8_t hdBuf[512];

    for (int idx = 0; idx < sectorCount; idx++)
    {
        hdRead(hdBuf, startSector + idx, 1);
        hdWrite(hdBuf, fcbList[fcbIdx].__startSector + idx, 1);
    }

    hdWrite(fcbList, 100, 1);
    hdWrite(hdBitmapBuf, 101, 1);
}


uint32_t __findFCB(const char *fileName)
{
    for (int idx = 0; idx < 15; idx++)
    {
        if (fcbList[idx].__startSector && strEq(fcbList[idx].__fileName, fileName))
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

    bitmapDeallocate(&hdBitmap, fcbList[fcbIdx].__startSector - 102, fcbList[fcbIdx].__sectorCount);

    memset(fcbList + fcbIdx, 0, 32);

    hdWrite(fcbList, 100, 1);
    hdWrite(hdBitmapBuf, 101, 1);
}


void fsLoad(const char *fileName)
{
    uint32_t fcbIdx = __findFCB(fileName);

    if (fcbIdx != -1)
    {
        loadTaskPL3(fcbList[fcbIdx].__startSector, fcbList[fcbIdx].__sectorCount);
    }
}
