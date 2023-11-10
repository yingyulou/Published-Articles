#pragma once

#include "Print.h"
#include "Util.h"

const char __HEX_LIST[] = "0123456789ABCDEF";

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
        : "edx"
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
        : "eax", "edx"
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

            *(char *)(0xc00b8000 + sectorNum * 2) = ' ';
            break;

        case '\n':
        case '\r':
            sectorNum = (sectorNum / 80 + 1) * 80;
            break;

        default:
            *(char *)(0xc00b8000 + sectorNum * 2) = tarChar;
            sectorNum++;
            break;
    }

    if (sectorNum >= 2000)
    {
        __asm__ __volatile__(
            "rep movsl\n\t"
            :
            : "S"(0xc00b80a0), "D"(0xc00b8000), "c"(960)
        );

        __asm__ __volatile__(
            "rep stosl\n\t"
            :
            : "a"(0x07200720), "D"(0xc00b8f00), "c"(40)
        );

        sectorNum = 1920;
    }

    __setSector(sectorNum);
}


void printStr(const char *tarStr)
{
    for (; *tarStr; tarStr++)
    {
        printChar(*tarStr);
    }
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


void printHex(uint32_t tarNum)
{
    printStr("0x");

    for (int idx = 7; idx >= 0; idx--)
    {
        printChar(__HEX_LIST[(tarNum >> idx * 4) & 0xf]);
    }
}


void printf(const char *fmtStr, ...)
{
    va_list vaList;
    va_start(vaList, fmtStr);

    bool lexerState = false;

    for (int idx = 0; fmtStr[idx]; idx++)
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
                    printHex(va_arg(vaList, uint32_t));
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
    __asm__ __volatile__(
        "rep stosl\n\t"
        :
        : "a"(0x07200720), "D"(0xc00b8000), "c"(1000)
    );

    __setSector(0);
}


void printInit()
{
    __cleanScreen();
}
