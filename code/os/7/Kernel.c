#include "Print.hpp"
#include "Int.hpp"

int main()
{
    printInit();
    intInit();

    __asm__ __volatile__("sti");

    for (;;);

    return 0;
}
