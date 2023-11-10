#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"

int main()
{
    printInit();
    intInit();
    memoryInit();
    taskInit();

    loadTaskPL3(1000, 20);
    loadTaskPL3(1000, 20);

    __asm__ __volatile__("sti");

    while (1)
    {
        deleteTask();

        __asm__ __volatile__("hlt");
    }

    return 0;
}
