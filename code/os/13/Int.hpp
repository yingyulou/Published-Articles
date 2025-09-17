#pragma once

#include "Int.h"
#include "Util.h"

extern uint32_t __intList[];
void __picInit();

alignas(0x8) uint64_t IDT[0x31];

uint64_t __makeIntGate(uint64_t CS, uint64_t EIP, uint64_t gateAttr)
{
    return (EIP & 0xffff) | (CS << 16) | (gateAttr << 32) | ((EIP & 0xffff0000) << 32);
}


void __idtInit()
{
    for (uint32_t idx = 0; idx < 0x30; idx++)
    {
        IDT[idx] = __makeIntGate(1 << 3, __intList[idx], 0x8e00);
    }

    IDT[0x30] = __makeIntGate(1 << 3, __intList[0x30], 0xee00);

    struct { uint16_t _0; void *_1; } __attribute__((__packed__)) IDTR = {sizeof(IDT) - 1, IDT};

    __asm__ __volatile__("lidt %0":: "m"(IDTR));
}


void intInit()
{
    __picInit();
    __idtInit();
}
