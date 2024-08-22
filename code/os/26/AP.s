[bits 64]
[default rel]

extern hdRead
extern taskQueue
extern syscallInit

global apInit

__CPU_COUNT equ 4

apInit:

    push rdx
    push rsi
    push rdi

    sgdt [abs 0x7e00]
    sidt [abs 0x7e10]
    mov rdx, apBoot64
    mov [abs 0x7e20], rdx

    mov rdi, 0x8000
    mov rsi, 1
    mov rdx, 1
    call hdRead

    mov rdi, 0xffff8000fee00300
    mov dword [rdi], 0x000c4500
    db 0xeb, 0x0
    mov dword [rdi], 0x000c4608

.__waitAP:

    cmp qword [apInitFlag], __CPU_COUNT
    jne .__waitAP

    pop rdi
    pop rsi
    pop rdx

    ret

apBoot64:

    lgdt [abs 0x7e00]
    lidt [abs 0x7e10]

    mov r8, 0xffff8000fee00020
    mov r8d, [r8]
    shr r8, 24

    mov r9, 0xffff8000000a0000
    lea rax, [r8 + 1]
    shl rax, 12
    sub r9, rax

    lea rsp, [r9 + 0x1000]

    mov rbx, 0xffff8000fee00000
    bts dword [rbx + 0xf0], 8
    mov dword [rbx + 0x320], 0x20020
    mov dword [rbx + 0x3e0], 0xb
    mov dword [rbx + 0x380], 0xffff

    mov rbx, 0xffff8000fec00000
    mov dword [rbx], 0x12
    mov dword [rbx + 0x10], 0x21
    mov dword [rbx], 0x13
    mov dword [rbx + 0x10], 0x0

    lea rax, [r8 * 2 + 7]
    shl rax, 3
    ltr ax

    mov ecx, 0xc0000101
    mov rax, r8
    shl rax, 7
    mov rdx, 0xffff800000092000
    add rax, rdx
    mov rdx, rax
    shr rdx, 32
    wrmsr

    mov qword [r9 + 0x10], 0x100000
    mov rax, taskQueue
    mov [r9 + 0x20], rax

    call syscallInit

    lock inc qword [apInitFlag]

    sti

.__idle:

    hlt
    jmp .__idle

apInitFlag:
    dq 0x1
