#include "Bitmap.hpp"
#include "Queue.hpp"
#include "IOQueue.hpp"
#include "Print.hpp"
#include "Memory.hpp"
#include "Int.hpp"
#include "Task.hpp"
#include "FS.hpp"
#include "Keyboard.hpp"
#include "Syscall.h"
#include "AP.h"
#include "Shell.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    memoryInit();
    intInit();
    taskInit();
    fsInit();
    keyboardInit();
    syscallInit();
    apInit();
    shellInit();

    __asm__ __volatile__("sti");

    for (;;)
    {
        deleteTask();

        __asm__ __volatile__("hlt");
    }

    return 0;
}
