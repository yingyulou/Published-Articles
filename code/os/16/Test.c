int main()
{
    char inputStr[128];

    __asm__ __volatile__("int $0x30":: "a"(1), "b"(inputStr), "c"(128));
    __asm__ __volatile__("int $0x30":: "a"(0), "b"(inputStr));

    return 0;
}
