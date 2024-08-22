[bits 64]
[default rel]

extern deallocateTaskCR3
extern queuePush
extern exitQueue
extern __nextTask

global taskExit

taskExit:

    call deallocateTaskCR3

    mov rdi, exitQueue
    mov rsi, rsp
    mov rax, 0xfffffffffffff000
    and rsi, rax
    call queuePush

    jmp __nextTask
