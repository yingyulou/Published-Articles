#pragma once

#include "Util.h"

typedef struct
{
    uint8_t *__buf;
    uint64_t __size;
} Bitmap;


void bitmapInit(Bitmap *this, uint8_t *buf, uint64_t size);
bool bitmapGet(Bitmap *this, uint64_t idx);
void bitmapSet(Bitmap *this, uint64_t idx, bool val);
uint64_t bitmapAllocate(Bitmap *this, uint64_t bitCount);
void bitmapDeallocate(Bitmap *this, uint64_t startIdx, uint64_t bitCount);
