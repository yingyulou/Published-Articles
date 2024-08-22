[bits 64]
[default rel]

extern main

global _start

_start:

    call main

    mov rax, 2
    syscall
