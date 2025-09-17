#pragma once

#include "Util.h"

typedef uint64_t Lock;

void lockInit(Lock *this);
void lockAcquire(Lock *this);
void lockRelease(Lock *this);
