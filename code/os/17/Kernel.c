#include "Bitmap.hpp"
#include "Queue.hpp"
#include "IOQueue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"
#include "FS.hpp"
#include "Keyboard.hpp"
#include "Shell.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    intInit();
    memoryInit();
    taskInit();
    fsInit();
    keyboardInit();
    shellInit();

    __asm__ __volatile__("sti");

    while (1)
    {
        deleteTask();

        __asm__ __volatile__("hlt");
    }

    return 0;
}
