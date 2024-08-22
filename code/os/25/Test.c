int main()
{
    for (unsigned _ = 0; _ < 0x8000000; _++);

    __asm__ __volatile__("syscall":: "a"(0), "D"("666\n"));

    return 0;
}
