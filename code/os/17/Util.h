#pragma once

typedef _Bool bool;

#define true  1
#define false 0

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned           uint32_t;
typedef unsigned long long uint64_t;

typedef uint8_t *va_list;

#define va_start(vaList, preArg) (vaList = (va_list)&preArg + sizeof(preArg))
#define va_arg(vaList, argType) (*(argType *)((vaList += sizeof(argType)) - sizeof(argType)))
#define va_end(vaList) (vaList = 0)

bool strEq(const char *lhs, const char *rhs);
void strCopy(char *tarStr, const char *srcStr, unsigned strLen);
const char *nextStr(char **strPtr);
uint32_t nextNum(char **strPtr);
