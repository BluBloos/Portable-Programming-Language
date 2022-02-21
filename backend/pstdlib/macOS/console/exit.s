ppl_exit: 
    ; rcx = return value
    mov rax, 0x2000001 ; exit
    mov rdi, rcx
    syscall

    ; NOTE(Noah): I don't even think this function
    ; needs to return...