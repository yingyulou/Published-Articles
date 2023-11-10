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
