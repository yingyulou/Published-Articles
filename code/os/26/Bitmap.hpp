#pragma once

#include "Bitmap.h"
#include "Util.h"

void bitmapInit(Bitmap *this, uint8_t *buf, uint64_t size)
{
    this->__buf  = buf;
    this->__size = size;
}


bool bitmapGet(Bitmap *this, uint64_t idx)
{
    return (this->__buf[idx >> 3] >> (idx & 7)) & 1;
}


void bitmapSet(Bitmap *this, uint64_t idx, bool val)
{
    if (val)
    {
        this->__buf[idx >> 3] |= 1 << (idx & 7);
    }
    else
    {
        this->__buf[idx >> 3] &= ~(1 << (idx & 7));
    }
}


uint64_t bitmapAllocate(Bitmap *this, uint64_t bitCount)
{
    for (uint64_t i = 0;; i++)
    {
        bool allocateBool = true;

        for (uint64_t j = i; j < i + bitCount; j++)
        {
            if (bitmapGet(this, j))
            {
                allocateBool = false;
                break;
            }
        }

        if (allocateBool)
        {
            for (uint64_t j = i; j < i + bitCount; j++)
            {
                bitmapSet(this, j, 1);
            }

            return i;
        }
    }
}


void bitmapDeallocate(Bitmap *this, uint64_t startIdx, uint64_t bitCount)
{
    for (uint64_t _ = 0; _ < bitCount; _++)
    {
        bitmapSet(this, startIdx++, 0);
    }
}
