#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    intInit();
    memoryInit();
    taskInit();

    loadTaskPL3(1000, 100);
    loadTaskPL3(1000, 100);

    __asm__ __volatile__("sti");

    for (;;)
    {
        __asm__ __volatile__("int $0x30":: "a"(0), "b"("Kernel"));
    }

    return 0;
}
