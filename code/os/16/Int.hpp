#pragma once

#include "Int.h"
#include "Util.h"

extern uint32_t intList[];
uint64_t IDT[49];

void __picInit();


uint64_t __makeIntGate(uint64_t CS, uint64_t EIP, uint64_t intGateAttr)
{
    return (EIP & 0xffff) | (CS << 16) | (intGateAttr << 32) | ((EIP & 0xffff0000) << 32);
}


void __installIDT()
{
    for (int idx = 0; idx < 0x30; idx++)
    {
        IDT[idx] = __makeIntGate(1 << 3, intList[idx], 0x8e00);
    }

    IDT[0x30] = __makeIntGate(1 << 3, intList[0x30], 0xee00);

    uint64_t IDTR = (sizeof(IDT) - 1) | ((uint64_t)(uint32_t)IDT << 16);

    __asm__ __volatile__("lidt %0":: "m"(IDTR));
}


void intInit()
{
    __picInit();
    __installIDT();
}
