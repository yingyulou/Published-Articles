[bits 64]
[default rel]

global hdRead
global hdWrite

hdRead:

    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rax
    push rcx

    mov dx, 0x1f2
    mov al, [rbp - 0x18]
    out dx, al

    inc dx
    mov eax, [rbp - 0x10]
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
    movzx rcx, byte [rbp - 0x18]
    shl rcx, 8
    mov rdi, [rbp - 0x8]
    rep insw

    pop rcx
    pop rax
    pop rdx
    pop rsi
    pop rdi

    leave
    ret

hdWrite:

    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rax
    push rcx

    mov dx, 0x1f2
    mov al, [rbp - 0x18]
    out dx, al

    inc dx
    mov eax, [rbp - 0x10]
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
    movzx rcx, byte [rbp - 0x18]
    shl rcx, 8
    mov rsi, [rbp - 0x8]
    rep outsw

    pop rcx
    pop rax
    pop rdx
    pop rsi
    pop rdi

    leave
    ret
