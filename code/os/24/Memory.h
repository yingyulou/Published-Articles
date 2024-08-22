#pragma once

#include "Util.h"

void memoryInit();
void *allocateKernelPage(uint64_t pageCount);
void installTaskPage(Bitmap *vBitmap, uint64_t startAddr, uint64_t memorySize);
void deallocateKernelPage(void *vPtr, uint64_t pageCount);
void deallocateTaskCR3();
