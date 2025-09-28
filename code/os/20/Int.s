[bits 64]
[default rel]

extern printInt

global __intUnknown
global __intTimer

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

    mov rdi, 6
    call printInt

    popaq

    iretq
