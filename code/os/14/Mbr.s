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
    mov ecx, 0x100000 / 4
    rep stosd

    mov dword [0x100000], 0x101003

    mov eax, 0x101003
    mov ebx, 0x100c00
    mov ecx, 255

.__installPDE:

    mov [ebx], eax
    add eax, 0x1000
    add ebx, 4
    loop .__installPDE

    mov dword [0x100ffc], 0x100003

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

    mov esp, 0xc00a0000

    or dword [GDTR + 2], 0xc0000000
    lgdt [GDTR]

    mov dx, 0x1f2
    mov al, 99
    out dx, al

    inc dx
    mov al, 1
    out dx, al

    inc dx
    xor al, al
    out dx, al

    inc dx
    out dx, al

    inc dx
    mov al, 0xe0
    out dx, al

    inc dx
    mov al, 0x20
    out dx, al

.__waitDisk:

    in al, dx
    and al, 0x88
    cmp al, 0x8
    jne .__waitDisk

    mov dx, 0x1f0
    mov edi, 0xc0080000
    mov ecx, 99 * 512 / 2
    rep insw

    xor ecx, ecx
    xor edx, edx

    mov ebx, [0xc008001c]
    add ebx, 0xc0080000
    mov dx, [0xc008002a]
    mov cx, [0xc008002c]

.__parseElf:

    push ecx

    cmp dword [ebx], 0x1
    jne .__parseElfEnd

    mov esi, [ebx + 0x4]
    add esi, 0xc0080000
    mov edi, [ebx + 0x8]
    mov ecx, [ebx + 0x10]
    rep movsb

    xor al, al
    mov ecx, [ebx + 0x14]
    sub ecx, [ebx + 0x10]
    rep stosb

.__parseElfEnd:

    add ebx, edx

    pop ecx
    loop .__parseElf

    jmp [0xc0080018]

GDT:
    dq 0
    dq 0x00cf98000000ffff
    dq 0x00cf92000000ffff
    dq 0x00cff8000000ffff
    dq 0x00cff2000000ffff
    dq 0

GDTR:
    dw $ - GDT - 1
    dd GDT

times 510 - ($ - $$) db 0

db 0x55, 0xaa
