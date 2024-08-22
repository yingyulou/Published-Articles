int main()
{
    __asm__ __volatile__("syscall":: "a"(0), "D"("666\n"));

    return 0;
}
