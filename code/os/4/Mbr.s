section mbr vstart=0x7c00

    lgdt [GDTR]

    in al, 0x92
    or al, 0x2
    out 0x92, al

    mov eax, cr0
    or eax, 0x1
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
    mov ecx, 0x2000 / 4
    rep stosd

    mov dword [0x100000], 0x101003

    mov eax, 0x3
    mov ebx, 0x101000
    mov ecx, 256

.__installPTE:

    mov [ebx], eax
    add eax, 0x1000
    add ebx, 4
    loop .__installPTE

    mov eax, 0x100000
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    mov byte [0xb8000], '6'
    mov byte [0xb8001], 0x7

    jmp $

GDT:
    dq 0
    dq 0x00cf98000000ffff
    dq 0x00cf92000000ffff

GDTR:
    dw $ - GDT - 1
    dd GDT

times 510 - ($ - $$) db 0

db 0x55, 0xaa
