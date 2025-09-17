#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"
#include "Syscall.h"
#include "Util.hpp"

int main()
{
    printInit();
    intInit();
    memoryInit();
    taskInit();
    syscallInit();

    loadTaskPL3(1000, 100);

    __asm__ __volatile__("sti");

    for (;;)
    {
        __asm__ __volatile__("hlt");
    }

    return 0;
}
