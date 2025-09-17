#pragma once

#include "Int.h"
#include "Util.h"

extern uint64_t __intList[];

alignas(0x8) uint64_t IDT[0x22][2];

void __picInit()
{
    __asm__ __volatile__("out %%al, $0x21":: "a"(0xff));
}


void __apicInit()
{
    *(uint32_t *)0xffff8000fee00320 = 0x20020;
    *(uint32_t *)0xffff8000fee003e0 = 0xb;
    *(uint32_t *)0xffff8000fee00380 = 0xffff;

    *(uint32_t *)0xffff8000fec00000 = 0x12;
    *(uint32_t *)0xffff8000fec00010 = 0x21;
    *(uint32_t *)0xffff8000fec00000 = 0x13;
    *(uint32_t *)0xffff8000fec00010 = 0x0;
}


void __makeIntGate(uint64_t *intGateLow, uint64_t *intGateHigh, uint64_t CS, uint64_t RIP, uint64_t gateAttr)
{
    *intGateLow  = (RIP & 0xffff) | (CS << 16) | (gateAttr << 32) | ((RIP & 0xffff0000) << 32);
    *intGateHigh = RIP >> 32;
}


void __idtInit()
{
    for (uint64_t idx = 0; idx < 0x22; idx++)
    {
        __makeIntGate(IDT[idx], IDT[idx] + 1, 3 << 3, __intList[idx], 0x8e00);
    }

    struct { uint16_t _0; void *_1; } __attribute__((__packed__)) IDTR = {sizeof(IDT) - 1, IDT};

    __asm__ __volatile__("lidt %0":: "m"(IDTR));
}


void intInit()
{
    __picInit();
    __apicInit();
    __idtInit();
}
