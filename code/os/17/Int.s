[bits 32]

extern curTask
extern getNextTask
extern keyboardDriver
extern printStr
extern inputStr
extern taskExit

global __picInit
global __intUnknown
global __intTimer
global __intKeyboard
global __intSyscall
global __taskSwitch

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
    mov al, 0xfc
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

__taskSwitch:

    call getNextTask

    mov ebx, [eax + 8]
    mov cr3, ebx

    mov esp, [eax + 12]

    lea ebx, [eax + 0x1000]
    mov [0xc009f018], ebx

    popa
    pop gs
    pop fs
    pop es
    pop ds

    iret

__intKeyboard:

    pusha

    mov al, 0x20
    out 0x20, al
    out 0xa0, al

    in al, 0x60
    push eax
    call keyboardDriver
    add esp, 4

    popa

    iret

__intSyscall:

    push edx
    push ecx
    push ebx
    call [__syscallList + eax * 4]
    add esp, 12

    iret

__syscallList:
    dd printStr
    dd inputStr
    dd taskExit
