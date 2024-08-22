#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Memory.hpp"
#include "Int.hpp"
#include "Task.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    memoryInit();
    intInit();
    taskInit();

    loadTaskPL3(1000, 10);

    __asm__ __volatile__("sti");

    for (;;)
    {
        deleteTask();

        __asm__ __volatile__("hlt");
    }

    return 0;
}
