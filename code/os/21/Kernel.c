#include "Bitmap.hpp"
#include "Print.hpp"
#include "Memory.hpp"
#include "Int.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    memoryInit();
    intInit();

    __asm__ __volatile__("sti");

    for (;;);

    return 0;
}
