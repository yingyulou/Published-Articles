#pragma once

#include "Bitmap.h"
#include "Util.h"

void bitmapInit(Bitmap *this, uint8_t *data)
{
    this->__data = data;
}


bool __bitmapGet(Bitmap *this, uint64_t idx)
{
    return (this->__data[idx >> 3] >> (idx & 0x7)) & 0x1;
}


void __bitmapSet(Bitmap *this, uint64_t idx, bool val)
{
    if (val)
    {
        this->__data[idx >> 3] |= 0x1 << (idx & 0x7);
    }
    else
    {
        this->__data[idx >> 3] &= ~(0x1 << (idx & 0x7));
    }
}


uint64_t bitmapAllocate(Bitmap *this, uint64_t bitCount)
{
    for (uint64_t i = 0;; i++)
    {
        bool allocateBool = true;

        for (uint64_t j = i; j < i + bitCount; j++)
        {
            if (__bitmapGet(this, j))
            {
                allocateBool = false;
                break;
            }
        }

        if (allocateBool)
        {
            for (uint64_t j = i; j < i + bitCount; j++)
            {
                __bitmapSet(this, j, 1);
            }

            return i;
        }
    }
}


void bitmapDeallocate(Bitmap *this, uint64_t startIdx, uint64_t bitCount)
{
    for (uint64_t _ = 0; _ < bitCount; _++)
    {
        __bitmapSet(this, startIdx++, 0);
    }
}
