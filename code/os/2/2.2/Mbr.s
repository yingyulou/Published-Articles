section Mbr vstart=0x7c00

    mov ax, 0xb800
    mov ds, ax

    mov byte [0], '6'
    mov byte [1], 0x7

    jmp $

times 510 - ($ - $$) db 0x0

db 0x55, 0xaa
