int main()
{
    char inputStr[128];

    __asm__ __volatile__(
        "int $0x30\n\t"
        :
        : "a"(1), "b"(inputStr), "c"(128)
    );

    __asm__ __volatile__(
        "int $0x30\n\t"
        :
        : "a"(0), "b"(inputStr)
    );

    return 0;
}
