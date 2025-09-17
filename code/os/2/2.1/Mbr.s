section Mbr vstart=0x7c00

    jmp $

times 510 - ($ - $$) db 0x0

db 0x55, 0xaa
