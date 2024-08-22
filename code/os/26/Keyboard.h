#pragma once

#include "IOQueue.h"
#include "Util.h"

void keyboardInit();
void keyboardDriver(uint64_t scanCode);
void inputStr(char *resStr, uint64_t strLen);
