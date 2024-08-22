#pragma once

#include "Util.h"

typedef uint64_t Lock;

void lockInit(Lock *this);
uint64_t lockAcquire(Lock *this);
void lockRelease(Lock *this, uint64_t RFLAGS);
