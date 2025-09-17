#include "Bitmap.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    intInit();
    memoryInit();

    __asm__ __volatile__("sti");

    for (;;);

    return 0;
}
