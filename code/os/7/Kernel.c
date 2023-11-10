#include "Print.hpp"
#include "Int.hpp"

int main()
{
    printInit();
    intInit();

    __asm__ __volatile__("sti");

    while (1);

    return 0;
}
