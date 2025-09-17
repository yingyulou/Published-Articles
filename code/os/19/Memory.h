#pragma once

#include "Util.h"

void memoryInit();
void *allocateKernelPage(uint64_t pageCount);
void installTaskPage(void *startPtr, uint64_t memSize);
void deallocateKernelPage(void *startPtr, uint64_t pageCount);
void deallocateTaskCR3();
