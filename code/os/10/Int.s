[bits 32]

global __picInit
global __intUnknown
global __intTimer

__picInit:

    mov al, 0x11
    out 0x20, al
    mov al, 0x20
    out 0x21, al
    mov al, 0x04
    out 0x21, al
    mov al, 0x01
    out 0x21, al
    mov al, 0x11
    out 0xa0, al
    mov al, 0x28
    out 0xa1, al
    mov al, 0x02
    out 0xa1, al
    mov al, 0x01
    out 0xa1, al
    mov al, 0xfe
    out 0x21, al
    mov al, 0xff
    out 0xa1, al

    ret

__intUnknown:

    hlt

__intTimer:

    push ds
    push es
    push fs
    push gs
    pusha

    mov al, 0x20
    out 0x20, al
    out 0xa0, al

    popa
    pop gs
    pop fs
    pop es
    pop ds

    iret
