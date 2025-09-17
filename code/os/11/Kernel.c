#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"
#include "Util.hpp"

void __testTask()
{
    for (;;)
    {
        __asm__ __volatile__("cli");
        printStr("Task");
        __asm__ __volatile__("sti");
    }
}


int main()
{
    printInit();
    intInit();
    memoryInit();
    taskInit();

    loadTaskPL0(__testTask);
    loadTaskPL0(__testTask);

    __asm__ __volatile__("sti");

    for (;;)
    {
        __asm__ __volatile__("cli");
        printStr("Kernel");
        __asm__ __volatile__("sti");
    }

    return 0;
}
