#pragma once

#include "IOQueue.h"
#include "Util.h"

void keyboardInit();
void keyboardDriver(uint32_t scanCode);
void inputStr(char *resStr, uint32_t strLen);
