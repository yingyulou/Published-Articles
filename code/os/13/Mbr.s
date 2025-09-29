%include "Boot.inc"

    mov ax, __BOOT_SEG
    mov es, ax
    mov si, 0x7c00
    xor di, di
    mov cx, 256
    rep movsw
    mov ds, ax
    jmp __BOOT_SEG:.__bootStart

.__bootStart:

    lgdt [GDTR]

    in al, 0x92
    or al, 0x2
    out 0x92, al

    mov eax, cr0
    bts eax, 0
    mov cr0, eax

    jmp dword (1 << 3):.__protectMode + __BOOT_ADDR

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
    mov edi, 0x100c00
    mov ecx, 255

.__installPDE:

    stosd
    add eax, 0x1000
    loop .__installPDE

    mov dword [0x100ffc], 0x100003

    mov eax, 0x3
    mov edi, 0x101000
    mov ecx, 256

.__installPTE:

    stosd
    add eax, 0x1000
    loop .__installPTE

    mov eax, 0x100000
    mov cr3, eax

    mov eax, cr0
    bts eax, 31
    mov cr0, eax

    or dword [GDTR + __BOOT_ADDR + 2], 0xc0000000
    lgdt [GDTR + __BOOT_ADDR]

    mov esp, 0xc00a0000

    mov dx, 0x1f2
    mov al, 97
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

.__waitHD:

    in al, dx
    and al, 0x88
    cmp al, 0x8
    jne .__waitHD

    mov dx, 0x1f0
    mov edi, 0xc0080000
    mov ecx, 97 * 512 / 2
    rep insw

    mov ebx, [0xc008001c]
    add ebx, 0xc0080000
    movzx edx, word [0xc008002a]
    movzx ecx, word [0xc008002c]

.__parseELF:

    cmp dword [ebx], 0x1
    jne .__notLoad

    push ecx
    mov esi, [ebx + 0x4]
    add esi, 0xc0080000
    mov edi, [ebx + 0x8]
    mov ecx, [ebx + 0x10]
    rep movsb
    xor al, al
    mov ecx, [ebx + 0x14]
    sub ecx, [ebx + 0x10]
    rep stosb
    pop ecx

.__notLoad:

    add ebx, edx
    loop .__parseELF

    jmp [0xc0080018]

align 0x8
GDT:
    dq 0x0
    dq 0x00cf98000000ffff
    dq 0x00cf92000000ffff
    dq 0x00cff8000000ffff
    dq 0x00cff2000000ffff
    dq 0xc0008909f0140067

GDTR:
    dw $ - GDT - 1
    dd GDT + __BOOT_ADDR

times 510 - ($ - $$) db 0x0

db 0x55, 0xaa
