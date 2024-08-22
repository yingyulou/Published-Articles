section Mbr vstart=0x7c00

    lgdt [GDTR]

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

    xor eax, eax
    mov edi, 0x100000
    mov ecx, 0x100000 / 4
    rep stosd

    mov edi, 0x90000
    mov ecx, 0x2000 / 4
    rep stosd

    mov dword [0x100000], 0x101000 | 0x3

    mov eax, 0x101000 | 0x3
    mov ebx, 0x100800
    mov ecx, 255

.__installPML4:

    mov [ebx], eax
    add eax, 0x1000
    add ebx, 0x8
    loop .__installPML4

    mov dword [0x100ff8], 0x100000 | 0x3
    mov dword [0x101000], 0x90000 | 0x3
    mov dword [0x90000], 0x0 | 0x83

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

    lgdt [GDTR]

    mov rsp, 0xffff8000000a0000

    jmp $

GDT:
    dq 0x0
    dq 0x00cf98000000ffff
    dq 0x00cf92000000ffff
    dq 0x0020980000000000
    dq 0x0020920000000000
    dq 0x0020f20000000000
    dq 0x0020f80000000000

GDTR:
    dw $ - GDT - 1
    dd GDT
    dd 0xffff8000

times 510 - ($ - $$) db 0x0

db 0x55, 0xaa
