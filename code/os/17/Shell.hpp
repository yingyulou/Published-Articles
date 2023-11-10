#pragma once

#include "Shell.h"
#include "Print.h"
#include "Task.h"
#include "FS.h"
#include "Keyboard.h"
#include "Util.h"

void __parseCmd(char *cmdStr)
{
    if (cmdStr[0] == 'l' && cmdStr[1] == '\n')
    {
        fsList();
    }
    else if (cmdStr[0] == 'c' && cmdStr[1] == ' ')
    {
        cmdStr += 2;

        const char *fileName = nextStr(&cmdStr);
        uint32_t startSector = nextNum(&cmdStr);
        uint32_t sectorCount = nextNum(&cmdStr);

        fsCreate(fileName, startSector, sectorCount);
    }
    else if (cmdStr[0] == 'd' && cmdStr[1] == ' ')
    {
        cmdStr += 2;

        fsDelete(nextStr(&cmdStr));
    }
    else if (cmdStr[0] == 'r' && cmdStr[1] == ' ')
    {
        cmdStr += 2;

        fsLoad(nextStr(&cmdStr));
    }
    else
    {
        printStr(cmdStr);
    }
}


void shell()
{
    char cmdStr[128];

    while (true)
    {
        printStr("[Arena]$ ");
        inputStr(cmdStr, 128);
        __parseCmd(cmdStr);
    }
}


void shellInit()
{
    loadTaskPL0((uint32_t)shell);
}
