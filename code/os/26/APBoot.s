section APBoot vstart=0x8000

    lgdt [0x7e00]

    in al, 0x92
    or al, 0x2
    out 0x92, al

    mov eax, cr0
    bts eax, 0
    mov cr0, eax

    jmp (1 << 3):.__protectMode

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

    jmp (3 << 3):.__x64Mode

[bits 64]

.__x64Mode:

    jmp [0x7e20]
