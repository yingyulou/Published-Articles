#include "Bitmap.hpp"
#include "Print.hpp"
#include "Memory.hpp"
#include "Util.hpp"

int main()
{
    printInit();
    memoryInit();

    printf("%c %s %d %x\n", '6', "666", 666, 0x666);

    void *memPtrA = allocateKernelPage(3);
    void *memPtrB = allocateKernelPage(3);

    deallocateKernelPage(memPtrA, 3);

    void *memPtrC = allocateKernelPage(3);

    printf("%x %x %x\n", memPtrA, memPtrB, memPtrC);

    for (;;);

    return 0;
}
