[bits 64]
[default rel]

extern printf
extern getNextTask

global __intList
global __taskSwitch

%macro intTmpl 1
int%1:

    mov rdi, __intFmtStr
    mov rsi, %1
    call printf

    hlt
%endmacro

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

intTimer:

    pushaq

    mov rax, 0xffff8000fee000b0
    mov dword [rax], 0x0

    mov rax, rsp
    mov rbx, 0xfffffffffffff000
    and rax, rbx

    mov [rax + 0x18], rsp

__taskSwitch:

    call getNextTask

    mov rbx, [rax + 0x10]
    mov cr3, rbx

    mov rsp, [rax + 0x18]

    lea rbx, [rax + 0x1000]
    mov [gs: 4], rbx

    popaq

    iretq

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

__intList:
    dq int0x00
    dq int0x01
    dq int0x02
    dq int0x03
    dq int0x04
    dq int0x05
    dq int0x06
    dq int0x07
    dq int0x08
    dq int0x09
    dq int0x0a
    dq int0x0b
    dq int0x0c
    dq int0x0d
    dq int0x0e
    dq int0x0f
    dq int0x10
    dq int0x11
    dq int0x12
    dq int0x13
    dq int0x14
    dq int0x15
    dq int0x16
    dq int0x17
    dq int0x18
    dq int0x19
    dq int0x1a
    dq int0x1b
    dq int0x1c
    dq int0x1d
    dq int0x1e
    dq int0x1f
    dq intTimer
    dq 0x0

__intFmtStr:
    db `Int: %d\n\0`
