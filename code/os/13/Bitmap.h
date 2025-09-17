#pragma once

#include "Util.h"

typedef struct
{
    uint8_t *__data;
} Bitmap;


void bitmapInit(Bitmap *this, uint8_t *data);
uint32_t bitmapAllocate(Bitmap *this, uint32_t bitCount);
void bitmapDeallocate(Bitmap *this, uint32_t startIdx, uint32_t bitCount);
