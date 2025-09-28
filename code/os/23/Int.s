[bits 64]
[default rel]

extern getNextTask
extern keyboardDriver

global __intUnknown
global __intTimer
global __intKeyboard
global __taskSwitch

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

__intUnknown:

    hlt

__intTimer:

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

__intKeyboard:

    pushaq

    mov rax, 0xffff8000fee000b0
    mov dword [rax], 0x0

    in al, 0x60
    mov dil, al
    call keyboardDriver

    popaq

    iretq
