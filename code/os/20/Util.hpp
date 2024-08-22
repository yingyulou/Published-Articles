#pragma once

#include "Util.h"

void memcpy(void *tarPtr, const void *srcPtr, uint64_t memSize)
{
    __asm__ __volatile__("rep movsb":: "D"(tarPtr), "S"(srcPtr), "c"(memSize): "memory");
}


void memset(void *tarPtr, uint8_t setVal, uint64_t memSize)
{
    __asm__ __volatile__("rep stosb":: "D"(tarPtr), "a"(setVal), "c"(memSize): "memory");
}


bool strcmp(const char *lhs, const char *rhs)
{
    for (;;)
    {
        if (*lhs && *rhs)
        {
            if (*lhs++ != *rhs++)
            {
                return false;
            }
        }
        else if (!*lhs && !*rhs)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}


void strcpy(char *tarStr, const char *srcStr, long long strLen)
{
    long long idx = 0;

    for (; idx < strLen - 1; idx++)
    {
        if (srcStr[idx])
        {
            tarStr[idx] = srcStr[idx];
        }
        else
        {
            break;
        }
    }

    tarStr[idx] = 0;
}


bool isalnum(char curChar)
{
    return ('A' <= curChar && curChar <= 'Z') || ('a' <= curChar && curChar <= 'z') || ('0' <= curChar && curChar <= '9');
}


bool isdigit(char curChar)
{
    return '0' <= curChar && curChar <= '9';
}


const char *nextStr(char **strPtr)
{
    for (; !isalnum(**strPtr); (*strPtr)++);

    const char *resStr = *strPtr;

    for (; isalnum(**strPtr); (*strPtr)++);

    **strPtr = 0;

    return resStr;
}


uint32_t nextNum(char **strPtr)
{
    uint32_t resNum = 0;

    for (; !isdigit(**strPtr); (*strPtr)++);

    for (; isdigit(**strPtr); (*strPtr)++)
    {
        resNum = resNum * 10 + **strPtr - '0';
    }

    return resNum;
}
