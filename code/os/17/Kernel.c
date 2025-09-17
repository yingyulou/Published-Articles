#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Buffer.hpp"
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

    for (;;)
    {
        __asm__ __volatile__("hlt");
    }

    return 0;
}
