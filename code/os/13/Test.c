int main()
{
    __asm__ __volatile__(
        "int $0x30\n\t"
        :
        : "a"(0), "b"("Task")
    );

    return 0;
}
