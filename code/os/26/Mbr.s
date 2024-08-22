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
    mov dword [0x101018], 0x91000 | 0x3

    mov dword [0x90000], 0x0 | 0x83
    mov dword [0x91fb0], 0xfec00000 | 0x93
    mov dword [0x91fb8], 0xfee00000 | 0x93

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

    mov dx, 0x1f2
    mov al, 96
    out dx, al

    inc dx
    mov al, 2
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

.__waitHD:

    in al, dx
    and al, 0x88
    cmp al, 0x8
    jne .__waitHD

    mov dx, 0x1f0
    mov rdi, 0x80000
    mov rcx, 96 * 256
    rep insw

    mov rbx, [0x80020]
    add rbx, 0x80000
    movzx rcx, word [0x80038]
    movzx rdx, word [0x80036]

.__parseELF:

    cmp dword [rbx], 0x1
    jne .__notLoad

    mov rsi, [rbx + 0x8]
    add rsi, 0x80000
    mov rdi, [rbx + 0x10]
    push rcx
    mov rcx, [rbx + 0x20]
    rep movsb
    xor rax, rax
    mov rcx, [rbx + 0x28]
    sub rcx, [rbx + 0x20]
    rep stosb
    pop rcx

.__notLoad:

    add rbx, rdx
    loop .__parseELF

    jmp [0x80018]

GDT:
    dq 0x0
    dq 0x00cf98000000ffff
    dq 0x00cf92000000ffff
    dq 0x0020980000000000
    dq 0x0020920000000000
    dq 0x0020f20000000000
    dq 0x0020f80000000000
    dq 0x0000890920000067, 0x00000000ffff8000
    dq 0x0000890920800067, 0x00000000ffff8000
    dq 0x0000890921000067, 0x00000000ffff8000
    dq 0x0000890921800067, 0x00000000ffff8000

GDTR:
    dw $ - GDT - 1
    dd GDT
    dd 0xffff8000

times 510 - ($ - $$) db 0x0

db 0x55, 0xaa
