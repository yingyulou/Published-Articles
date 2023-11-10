#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"

void __testTask()
{
    while (1)
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

    loadTaskPL0((uint32_t)__testTask);
    loadTaskPL0((uint32_t)__testTask);

    __asm__ __volatile__("sti");

    while (1)
    {
        __asm__ __volatile__("cli");
        printStr("Kernel");
        __asm__ __volatile__("sti");
    }

    return 0;
}
