#include "Bitmap.hpp"
#include "Print.hpp"
#include "Int.hpp"
#include "Memory.hpp"
#include "Util.hpp"
#include "HD.h"

uint8_t __hdBuf[1024] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

int main()
{
    printInit();
    intInit();
    memoryInit();

    hdWrite(__hdBuf, 1000, 1);
    hdRead(__hdBuf + 512, 1000, 1);

    for (uint32_t idx = 0; idx < 10; idx++)
    {
        printf("%d\n", __hdBuf[idx + 512]);
    }

    __asm__ __volatile__("sti");

    for (;;);

    return 0;
}
