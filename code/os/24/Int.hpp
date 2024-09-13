#pragma once

#include "Int.h"
#include "Util.h"

extern uint64_t intList[];
uint64_t IDT[0x22][2];

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


void __idtInit()
{
    for (uint64_t idx = 0; idx < 0x22; idx++)
    {
        IDT[idx][0] = (intList[idx] & 0xffff) | ((3ull << 3) << 16) | (0x8e00ull << 32) | ((intList[idx] & 0xffff0000) << 32);
        IDT[idx][1] = intList[idx] >> 32;
    }

    struct { uint16_t __idtLimit; void *__idtBase; } __attribute__((__packed__)) IDTR = {sizeof(IDT) - 1, IDT};

    __asm__ __volatile__("lidt %0":: "m"(IDTR));
}


void intInit()
{
    __picInit();
    __apicInit();
    __idtInit();
}
