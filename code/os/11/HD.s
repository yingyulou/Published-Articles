[bits 32]

global hdRead
global hdWrite

hdRead:

    push ebp
    mov ebp, esp

    pusha

    mov dx, 0x1f2
    mov al, [ebp + 16]
    out dx, al

    inc dx
    mov eax, [ebp + 12]
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
    mov ecx, [ebp + 16]
    shl ecx, 8
    mov edi, [ebp + 8]
    rep insw

    popa

    leave
    ret

hdWrite:

    push ebp
    mov ebp, esp

    pusha

    mov dx, 0x1f2
    mov al, [ebp + 16]
    out dx, al

    inc dx
    mov eax, [ebp + 12]
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
    mov ecx, [ebp + 16]
    shl ecx, 8
    mov esi, [ebp + 8]
    rep outsw

    popa

    leave
    ret
