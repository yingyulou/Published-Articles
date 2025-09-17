[bits 64]
[default rel]

global hdRead
global hdWrite

hdRead:

    movzx rcx, dl
    shl rcx, 8

    mov al, dl
    mov dx, 0x1f2
    out dx, al

    inc dx
    mov eax, esi
    out dx, al

    inc dx
    shr eax, 8
    out dx, al

    inc dx
    shr eax, 8
    out dx, al

    inc dx
    shr eax, 8
    or al, 0xe0
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
    rep insw

    ret

hdWrite:

    movzx rcx, dl
    shl rcx, 8

    mov al, dl
    mov dx, 0x1f2
    out dx, al

    inc dx
    mov eax, esi
    out dx, al

    inc dx
    shr eax, 8
    out dx, al

    inc dx
    shr eax, 8
    out dx, al

    inc dx
    shr eax, 8
    or al, 0xe0
    out dx, al

    inc dx
    mov al, 0x30
    out dx, al

.__waitHD:

    in al, dx
    and al, 0x88
    cmp al, 0x8
    jne .__waitHD

    mov dx, 0x1f0
    mov rsi, rdi
    rep outsw

    ret
