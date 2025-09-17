#pragma once

#include "Print.h"
#include "Util.h"

const char __HEX_LIST[] = "0123456789ABCDEF";
uint32_t __cursorNum = 0;

void __setCursor()
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
        : "b"(__cursorNum)
        : "eax", "edx"
    );
}


void printChar(char tarChar)
{
    switch (tarChar)
    {
        case '\b':

            if (__cursorNum)
            {
                __cursorNum--;
            }

            *(char *)(0xc00b8000 + __cursorNum * 2) = ' ';
            break;

        case '\n':
        case '\r':
            __cursorNum = (__cursorNum + 80) / 80 * 80;
            break;

        default:
            *(char *)(0xc00b8000 + __cursorNum * 2) = tarChar;
            __cursorNum++;
            break;
    }

    if (__cursorNum >= 2000)
    {
        __asm__ __volatile__("rep movsl":: "S"(0xc00b80a0), "D"(0xc00b8000), "c"(960));
        __asm__ __volatile__("rep stosl":: "a"(0x07200720), "D"(0xc00b8f00), "c"(40));

        __cursorNum = 1920;
    }

    __setCursor();
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
        char numStr[0x10] = {};
        int32_t idx = 0;

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

    for (int32_t idx = 7; idx >= 0; idx--)
    {
        printChar(__HEX_LIST[(tarNum >> idx * 4) & 0xf]);
    }
}


void printf(const char *fmtStr, ...)
{
    va_list vaList;
    va_start(vaList, fmtStr);

    for (uint32_t idx = 0; fmtStr[idx]; idx++)
    {
        if (fmtStr[idx] != '%')
        {
            printChar(fmtStr[idx]);
        }
        else if (fmtStr[idx + 1])
        {
            switch (fmtStr[++idx])
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
        }
    }

    va_end(vaList);
}


void __cleanScreen()
{
    __asm__ __volatile__("rep stosl":: "a"(0x07200720), "D"(0xc00b8000), "c"(1000));

    __setCursor();
}


void printInit()
{
    __cleanScreen();
}
