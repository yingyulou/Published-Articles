#pragma once

#include "Util.h"

bool strEq(const char *lhs, const char *rhs)
{
    while (true)
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


void strCopy(char *tarStr, const char *srcStr, unsigned strLen)
{
    int idx = 0;

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


uint32_t nextNum(char **strPtr)
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
