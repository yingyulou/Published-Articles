#pragma once

#include "Util.h"

typedef struct
{
    uint8_t *__data;
} Bitmap;


void bitmapInit(Bitmap *this, uint8_t *data);
uint64_t bitmapAllocate(Bitmap *this, uint64_t bitCount);
void bitmapDeallocate(Bitmap *this, uint64_t startIdx, uint64_t bitCount);
