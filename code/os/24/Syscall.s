[bits 64]
[default rel]

extern printStr
extern inputStr
extern taskExit

global syscallInit

syscallInit:

    push rax
    push rcx
    push rdx

    mov ecx, 0xc0000080
    rdmsr
    bts eax, 0
    wrmsr

    mov ecx, 0xc0000081
    xor eax, eax
    mov edx, (3 << 3) | (((4 << 3) | 0x3) << 16)
    wrmsr

    mov ecx, 0xc0000082
    mov rax, syscallHandle
    mov rdx, rax
    shr rdx, 32
    wrmsr

    mov ecx, 0xc0000084
    mov eax, 0x200
    xor edx, edx
    wrmsr

    pop rdx
    pop rcx
    pop rax

    ret

syscallHandle:

    mov [gs: 104], rsp
    mov rsp, [gs: 4]

    push rcx
    push r11

    mov rcx, syscallList
    call [rcx + rax * 8]

    pop r11
    pop rcx

    mov rsp, [gs: 104]

    o64 sysret

syscallList:
    dq printStr
    dq inputStr
    dq taskExit
