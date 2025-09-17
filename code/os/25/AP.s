%include "Boot.inc"

[bits 64]
[default rel]

extern hdRead
extern syscallInit
extern apStack

global apInit

__CPU_COUNT equ 4

apInit:

    sgdt [abs __AP_BOOT_ADDR + 0x8]
    sidt [abs __AP_BOOT_ADDR + 0x18]
    mov rax, apBoot64
    mov [abs __AP_BOOT_ADDR + 0x28], rax

    mov rax, 0xffff8000fee00300
    mov dword [rax], 0x000c4500
    db 0xeb, 0x0
    mov dword [rax], 0x000c4600 | (__AP_BOOT_ADDR >> 12)

.__waitAP:

    cmp dword [apInitFlag], __CPU_COUNT
    jne .__waitAP

    ret

apBoot64:

    lgdt [abs __AP_BOOT_ADDR + 0x8]
    lidt [abs __AP_BOOT_ADDR + 0x18]

    mov r8, 0xffff8000fee00020
    mov r8d, [r8]
    shr r8, 24

    mov r9, apStack
    lea rax, [r8 - 1]
    shl rax, 12
    add r9, rax

    lea rsp, [r9 + 0x1000]

    mov rax, 0xffff8000fee00000
    bts dword [rax + 0xf0], 8
    mov dword [rax + 0x320], 0x20020
    mov dword [rax + 0x3e0], 0xb
    mov dword [rax + 0x380], 0xffff

    lea rax, [r8 * 2 + 7]
    shl rax, 3
    ltr ax

    mov ecx, 0xc0000101
    lea rax, [r9 + 0x28]
    mov rdx, rax
    shr rdx, 32
    wrmsr

    call syscallInit

    lock inc dword [apInitFlag]

    sti

.__idle:

    hlt
    jmp .__idle

apInitFlag:
    dd 0x1
