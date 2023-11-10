[bits 32]

extern printStr
extern printf
extern keyboardDriver
extern inputStr
extern queuePush
extern queuePop
extern TSS
extern taskQueue
extern exitQueue
extern shellQueue
extern deallocateTaskCR3

global intList
global __picInit

__picInit:

    push eax
    push edx

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

    pop edx
    pop eax

    ret

%macro intTmpl 1

int%1:

    push %1
    push __fmtStr
    call printf
    add esp, 8

    hlt

%endmacro

intTmpl 0x00
intTmpl 0x01
intTmpl 0x02
intTmpl 0x03
intTmpl 0x04
intTmpl 0x05
intTmpl 0x06
intTmpl 0x07
intTmpl 0x08
intTmpl 0x09
intTmpl 0x0a
intTmpl 0x0b
intTmpl 0x0c
intTmpl 0x0d
intTmpl 0x0e
intTmpl 0x0f
intTmpl 0x10
intTmpl 0x11
intTmpl 0x12
intTmpl 0x13
intTmpl 0x14
intTmpl 0x15
intTmpl 0x16
intTmpl 0x17
intTmpl 0x18
intTmpl 0x19
intTmpl 0x1a
intTmpl 0x1b
intTmpl 0x1c
intTmpl 0x1d
intTmpl 0x1e
intTmpl 0x1f
; intTmpl 0x20
; intTmpl 0x21
intTmpl 0x22
intTmpl 0x23
intTmpl 0x24
intTmpl 0x25
intTmpl 0x26
intTmpl 0x27
intTmpl 0x28
intTmpl 0x29
intTmpl 0x2a
intTmpl 0x2b
intTmpl 0x2c
intTmpl 0x2d
intTmpl 0x2e
intTmpl 0x2f

intTimer:

    push ds
    push es
    push fs
    push gs
    pusha

    mov al, 0x20
    out 0x20, al
    out 0xa0, al

    mov eax, esp
    and eax, 0xfffff000

    mov [eax + 12], esp

    push eax
    push dword [eax + 16]
    call queuePush
    add esp, 8

    push taskQueue
    call queuePop
    add esp, 4

    mov ebx, [eax + 8]
    mov cr3, ebx

    mov esp, [eax + 12]

    add eax, 0x1000
    mov [TSS + 4], eax

    popa
    pop gs
    pop fs
    pop es
    pop ds

    iret

intKeyboard:

    push eax

    xor eax, eax

    mov al, 0x20
    out 0x20, al
    out 0xa0, al

    in al, 0x60
    push eax
    call keyboardDriver
    add esp, 4

    pop eax

    iret

intSyscall:

    push edx
    push ecx
    push ebx
    call [syscallList + eax * 4]
    add esp, 12

    iret

taskExit:

    call deallocateTaskCR3

    mov eax, esp
    and eax, 0xfffff000

    push eax
    push exitQueue
    call queuePush
    add esp, 8

    push shellQueue
    call queuePop
    add esp, 4

    mov dword [eax + 16], taskQueue

    mov ebx, [eax + 8]
    mov cr3, ebx

    mov esp, [eax + 12]

    add eax, 0x1000
    mov [TSS + 4], eax

    popa
    pop gs
    pop fs
    pop es
    pop ds

    iret

__fmtStr:
    db `Int: %d\n`, 0

intList:
    dd int0x00
    dd int0x01
    dd int0x02
    dd int0x03
    dd int0x04
    dd int0x05
    dd int0x06
    dd int0x07
    dd int0x08
    dd int0x09
    dd int0x0a
    dd int0x0b
    dd int0x0c
    dd int0x0d
    dd int0x0e
    dd int0x0f
    dd int0x10
    dd int0x11
    dd int0x12
    dd int0x13
    dd int0x14
    dd int0x15
    dd int0x16
    dd int0x17
    dd int0x18
    dd int0x19
    dd int0x1a
    dd int0x1b
    dd int0x1c
    dd int0x1d
    dd int0x1e
    dd int0x1f
    dd intTimer
    dd intKeyboard
    dd int0x22
    dd int0x23
    dd int0x24
    dd int0x25
    dd int0x26
    dd int0x27
    dd int0x28
    dd int0x29
    dd int0x2a
    dd int0x2b
    dd int0x2c
    dd int0x2d
    dd int0x2e
    dd int0x2f
    dd intSyscall

syscallList:
    dd printStr
    dd inputStr
    dd taskExit
