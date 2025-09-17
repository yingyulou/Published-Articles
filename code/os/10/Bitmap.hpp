#pragma once

#include "Bitmap.h"
#include "Util.h"

void bitmapInit(Bitmap *this, uint8_t *data)
{
    this->__data = data;
}


bool __bitmapGet(Bitmap *this, uint32_t idx)
{
    return (this->__data[idx >> 3] >> (idx & 0x7)) & 0x1;
}


void __bitmapSet(Bitmap *this, uint32_t idx, bool val)
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


uint32_t bitmapAllocate(Bitmap *this, uint32_t bitCount)
{
    for (uint32_t i = 0;; i++)
    {
        bool allocateBool = true;

        for (uint32_t j = i; j < i + bitCount; j++)
        {
            if (__bitmapGet(this, j))
            {
                allocateBool = false;
                break;
            }
        }

        if (allocateBool)
        {
            for (uint32_t j = i; j < i + bitCount; j++)
            {
                __bitmapSet(this, j, 1);
            }

            return i;
        }
    }
}


void bitmapDeallocate(Bitmap *this, uint32_t startIdx, uint32_t bitCount)
{
    for (uint32_t _ = 0; _ < bitCount; _++)
    {
        __bitmapSet(this, startIdx++, 0);
    }
}
