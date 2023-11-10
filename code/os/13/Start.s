[bits 32]

extern main

global _start

_start:

    call main

    mov eax, 2
    int 0x30
