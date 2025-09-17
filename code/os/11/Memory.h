#pragma once

#include "Util.h"

void memoryInit();
void *allocateKernelPage(uint32_t pageCount);
void deallocateKernelPage(void *startPtr, uint32_t pageCount);
