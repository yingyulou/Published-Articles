#pragma once

#include "Util.h"

void memcpy(void *tarPtr, const void *srcPtr, uint32_t memSize)
{
    __asm__ __volatile__(
        "rep movsb"
        : "+D"(tarPtr), "+S"(srcPtr), "+c"(memSize)
        :
        : "memory"
    );
}


void memset(void *tarPtr, uint8_t setVal, uint32_t memSize)
{
    __asm__ __volatile__(
        "rep stosb"
        : "+D"(tarPtr), "+c"(memSize)
        : "a"(setVal)
        : "memory"
    );
}
