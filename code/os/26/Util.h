#pragma once

typedef _Bool bool;

#define true  ((bool)1)
#define false ((bool)0)

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned           uint32_t;
typedef unsigned long long uint64_t;

#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_arg   __builtin_va_arg
#define va_end   __builtin_va_end

void memcpy(void *tarPtr, const void *srcPtr, uint64_t memSize);
void memset(void *tarPtr, uint8_t setVal, uint64_t memSize);
bool strcmp(const char *lhs, const char *rhs);
void strcpy(char *tarStr, const char *srcStr, long long strLen);
const char *nextStr(char **strPtr);
uint32_t nextNum(char **strPtr);
