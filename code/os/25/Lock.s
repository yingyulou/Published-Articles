[bits 64]
[default rel]

extern lockInit
extern lockAcquire
extern lockRelease

lockInit:

    mov qword [rdi], 0x0

    ret

lockAcquire:

    push rdx

    mov rdx, 0x1

.__tryLock:

    xor rax, rax
    lock cmpxchg [rdi], rdx
    jne .__tryLock

    pushf
    pop rax
    cli

    pop rdx

    ret

lockRelease:

    mov qword [rdi], 0x0

    push rsi
    popf

    ret
