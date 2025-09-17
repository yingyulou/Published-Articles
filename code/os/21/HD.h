#pragma once

#include "Util.h"

void hdRead(void *tarPtr, uint32_t startSector, uint8_t sectorCount);
void hdWrite(void *srcPtr, uint32_t startSector, uint8_t sectorCount);
