#pragma once

#include "Util.h"

void memoryInit();
void memcpy(void *tarPtr, const void *srcPtr, uint32_t memSize);
void memset(void *tarPtr, uint8_t setVal, uint32_t memSize);
void *allocateKernelPage(uint32_t pageCount);
void installTaskPage(uint32_t vAddr, uint32_t pageCount);
void deallocateKernelPage(void *vAddr, uint32_t pageCount);
void deallocateTaskCR3();
