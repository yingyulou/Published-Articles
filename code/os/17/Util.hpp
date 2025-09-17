#pragma once

#include "Util.h"

void memcpy(void *tarPtr, const void *srcPtr, uint32_t memSize)
{
    __asm__ __volatile__(
        "rep movsb"
        : "+D"(tarPtr), "+S"(srcPtr), "+c"(memSize)
        :
        : "memory"
    );
}


void memset(void *tarPtr, uint8_t setVal, uint32_t memSize)
{
    __asm__ __volatile__(
        "rep stosb"
        : "+D"(tarPtr), "+c"(memSize)
        : "a"(setVal)
        : "memory"
    );
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


void strcpy(char *tarStr, const char *srcStr, uint32_t strSize)
{
    uint32_t idx = 0;

    for (; idx < strSize; idx++)
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
    return ('A' <= curChar && curChar <= 'Z') ||
        ('a' <= curChar && curChar <= 'z')    ||
        ('0' <= curChar && curChar <= '9');
}


bool isdigit(char curChar)
{
    return '0' <= curChar && curChar <= '9';
}


const char *getNextStr(char **strPtr)
{
    while (!isalnum(**strPtr))
    {
        (*strPtr)++;
    }

    const char *resStr = *strPtr;

    while (isalnum(**strPtr))
    {
        (*strPtr)++;
    }

    **strPtr = 0;

    return resStr;
}


uint32_t getNextNum(char **strPtr)
{
    uint32_t resNum = 0;

    while (!isdigit(**strPtr))
    {
        (*strPtr)++;
    }

    while (isdigit(**strPtr))
    {
        resNum = resNum * 10 + **strPtr - '0';
        (*strPtr)++;
    }

    return resNum;
}
