#pragma once

#include "Util.h"

typedef struct
{
    char __fileName[24];
    uint32_t __startSector;
    uint32_t __sectorCount;
} FCB;


void fsInit();
void fsList();
void fsCreate(const char *fileName, uint32_t startSector, uint32_t sectorCount);
void fsDelete(const char *fileName);
void fsLoad(const char *fileName);
