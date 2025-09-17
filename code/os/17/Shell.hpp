#pragma once

#include "Shell.h"
#include "Task.h"
#include "Print.h"
#include "FS.h"
#include "Keyboard.h"
#include "Util.h"

TCB *shellTask = 0;

void __parseCmd(char *cmdStr)
{
    if (cmdStr[0] == 'l' && cmdStr[1] == '\n')
    {
        fsList();
    }
    else if (cmdStr[0] == 'c' && cmdStr[1] == ' ')
    {
        cmdStr += 2;

        const char *fileName = getNextStr(&cmdStr);
        uint32_t startSector = getNextNum(&cmdStr);
        uint32_t sectorCount = getNextNum(&cmdStr);

        fsCreate(fileName, startSector, sectorCount);
    }
    else if (cmdStr[0] == 'd' && cmdStr[1] == ' ')
    {
        cmdStr += 2;

        fsDelete(getNextStr(&cmdStr));
    }
    else if (cmdStr[0] == 'r' && cmdStr[1] == ' ')
    {
        cmdStr += 2;

        fsLoad(getNextStr(&cmdStr));

        shellTask->__taskState = __TASK_BLOCK;
        __asm__ __volatile__("int $0x20");
    }
    else
    {
        printStr(cmdStr);
    }
}


void arenaShell()
{
    char cmdStr[128];

    for (;;)
    {
        printStr("[Arena]$ ");
        inputStr(cmdStr, 128);
        __parseCmd(cmdStr);
    }
}


void shellInit()
{
    shellTask = loadTaskPL0(arenaShell);
}
