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

    void *memPtrA = allocateKernelPage(1);
    void *memPtrB = allocateKernelPage(2);
    void *memPtrC = allocateKernelPage(3);

    deallocateKernelPage(memPtrB, 2);

    void *memPtrD = allocateKernelPage(1);
    void *memPtrE = allocateKernelPage(6);

    /*
        0xc0100000
        0xc0101000
        0xc0103000
        0xc0101000
        0xc0106000
    */
    printf("%x\n%x\n%x\n%x\n%x\n", memPtrA, memPtrB, memPtrC, memPtrD, memPtrE);

    deallocateKernelPage(memPtrA, 1);
    deallocateKernelPage(memPtrC, 3);
    deallocateKernelPage(memPtrD, 1);
    deallocateKernelPage(memPtrE, 6);

    __asm__ __volatile__("sti");

    for (;;);

    return 0;
}
