[bits 32]

extern curTask
extern getNextTask

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

    mov eax, [curTask]
    mov [eax + 12], esp

    call getNextTask

    mov ebx, [eax + 8]
    mov cr3, ebx

    mov esp, [eax + 12]

    popa
    pop gs
    pop fs
    pop es
    pop ds

    iret
