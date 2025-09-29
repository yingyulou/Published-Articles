%include "Boot.inc"

    jmp .__bootStart

align 0x8
GDTR:
    dq 0x0, 0x0

IDTR:
    dq 0x0, 0x0

apBoot64:
    dq 0x0

.__bootStart:

    mov ax, __AP_BOOT_SEG
    mov ds, ax
    mov es, ax

    lgdt [GDTR]

    in al, 0x92
    or al, 0x2
    out 0x92, al

    mov eax, cr0
    bts eax, 0
    mov cr0, eax

    jmp dword (1 << 3):.__protectMode + __AP_BOOT_ADDR

[bits 32]

.__protectMode:

    mov ax, 2 << 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, 0x100000
    mov cr3, eax

    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    mov ecx, 0xc0000080
    rdmsr
    bts eax, 8
    wrmsr

    mov eax, cr0
    bts eax, 31
    mov cr0, eax

    jmp (3 << 3):.__x64Mode + __AP_BOOT_ADDR

[bits 64]

.__x64Mode:

    jmp [apBoot64 + __AP_BOOT_ADDR]
