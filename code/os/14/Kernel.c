#include "Bitmap.hpp"
#include "Queue.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Task.hpp"
#include "FS.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    intInit();
    memoryInit();
    taskInit();
    fsInit();

    fsCreate("test", 1000, 100);
    fsCreate("test", 1000, 100);
    fsList();
    fsDelete("test");
    fsList();
    fsLoad("test");
    fsLoad("test");

    __asm__ __volatile__("sti");

    for (;;)
    {
        __asm__ __volatile__("hlt");
    }

    return 0;
}
