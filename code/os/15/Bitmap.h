#pragma once

#include "Util.h"

typedef struct
{
    uint8_t *__data;
    uint32_t __size;
} Bitmap;


void bitmapInit(Bitmap *this, uint8_t *data, uint32_t size, bool zeroBool);
bool bitmapGet(Bitmap *this, uint32_t idx);
void bitmapSet(Bitmap *this, uint32_t idx, bool val);
uint32_t bitmapAllocate(Bitmap *this, uint32_t bitCount);
void bitmapDeallocate(Bitmap *this, uint32_t startIdx, uint32_t bitCount);
