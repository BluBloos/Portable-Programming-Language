; .extern p_dcel void ppl_exit(int32)

global ppl_exit
ppl_exit:
    mov rax, 0x2000001 ; exit
    mov rdi, 0
    mov edi, DWORD [rsp + 8]
    syscall

    ; NOTE(Noah): I don't even think this function
    ; needs to return...
    ;
    ; hell yeahhhh