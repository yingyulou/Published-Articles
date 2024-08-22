#pragma once

#include "Print.h"
#include "Lock.h"
#include "Util.h"

const char __HEX_LIST[] = "0123456789ABCDEF";
Lock __printLock;

uint32_t __getSector()
{
    uint32_t sectorNum = 0;

    __asm__ __volatile__(
        "mov $0x3d4, %%dx\n\t"
        "mov $0xe, %%al\n\t"
        "out %%al, %%dx\n\t"
        "inc %%dx\n\t"
        "in %%dx, %%al\n\t"
        "mov %%al, %%ah\n\t"
        "mov $0x3d4, %%dx\n\t"
        "mov $0xf, %%al\n\t"
        "out %%al, %%dx\n\t"
        "inc %%dx\n\t"
        "in %%dx, %%al\n\t"
        : "=a"(sectorNum)
        :
        : "rdx"
    );

    return sectorNum;
}


void __setSector(uint32_t sectorNum)
{
    __asm__ __volatile__(
        "mov $0x3d4, %%dx\n\t"
        "mov $0xe, %%al\n\t"
        "out %%al, %%dx\n\t"
        "inc %%dx\n\t"
        "mov %h0, %%al\n\t"
        "out %%al, %%dx\n\t"
        "mov $0x3d4, %%dx\n\t"
        "mov $0xf, %%al\n\t"
        "out %%al, %%dx\n\t"
        "inc %%dx\n\t"
        "mov %b0, %%al\n\t"
        "out %%al, %%dx\n\t"
        :
        : "b"(sectorNum)
        : "rax", "rdx"
    );
}


void printChar(char tarChar)
{
    uint32_t sectorNum = __getSector();

    switch (tarChar)
    {
        case '\b':

            if (sectorNum)
            {
                sectorNum--;
            }

            *(char *)(0xffff8000000b8000 + sectorNum * 2) = ' ';
            break;

        case '\n':
        case '\r':
            sectorNum = (sectorNum / 80 + 1) * 80;
            break;

        default:
            *(char *)(0xffff8000000b8000 + sectorNum * 2) = tarChar;
            sectorNum++;
            break;
    }

    if (sectorNum >= 2000)
    {
        __asm__ __volatile__("rep movsq":: "S"(0xffff8000000b80a0), "D"(0xffff8000000b8000), "c"(480));
        __asm__ __volatile__("rep stosq":: "a"(0x0720072007200720), "D"(0xffff8000000b8f00), "c"(20));

        sectorNum = 1920;
    }

    __setSector(sectorNum);
}


void printStr(const char *tarStr)
{
    uint64_t RFLAGS = lockAcquire(&__printLock);

    for (; *tarStr; tarStr++)
    {
        printChar(*tarStr);
    }

    lockRelease(&__printLock, RFLAGS);
}


void printInt(uint32_t tarNum)
{
    if (!tarNum)
    {
        printChar('0');
    }
    else
    {
        char numStr[10] = {};
        int idx = 0;

        for (; tarNum; tarNum /= 10, idx++)
        {
            numStr[idx] = (tarNum % 10) + '0';
        }

        for (idx--; idx >= 0; idx--)
        {
            printChar(numStr[idx]);
        }
    }
}


void printHex(uint64_t tarNum)
{
    printStr("0x");

    for (int idx = 15; idx >= 0; idx--)
    {
        printChar(__HEX_LIST[(tarNum >> idx * 4) & 0xf]);
    }
}


void printf(const char *fmtStr, ...)
{
    va_list vaList;
    va_start(vaList, fmtStr);

    bool lexerState = false;

    for (uint64_t idx = 0; fmtStr[idx]; idx++)
    {
        if (!lexerState)
        {
            switch (fmtStr[idx])
            {
                case '%':
                    lexerState = true;
                    break;

                default:
                    printChar(fmtStr[idx]);
                    break;
            }
        }
        else
        {
            switch (fmtStr[idx])
            {
                case '%':
                    printChar('%');
                    break;

                case 'c':
                    printChar(va_arg(vaList, uint32_t));
                    break;

                case 's':
                    printStr(va_arg(vaList, const char *));
                    break;

                case 'd':
                    printInt(va_arg(vaList, uint32_t));
                    break;

                case 'x':
                    printHex(va_arg(vaList, uint64_t));
                    break;

                default:
                    printChar('%');
                    printChar(fmtStr[idx]);
                    break;
            }

            lexerState = false;
        }
    }

    va_end(vaList);
}


void __cleanScreen()
{
    __asm__ __volatile__("rep stosq":: "a"(0x0720072007200720), "D"(0xffff8000000b8000), "c"(500));

    __setSector(0);
}


void printInit()
{
    lockInit(&__printLock);

    __cleanScreen();
}
