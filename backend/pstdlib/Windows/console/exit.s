; .extern p_dcel void ppl_exit(int32)

extern ExitProcess

global ppl_exit
ppl_exit:
    mov rcx, DWORD [rsp + 8]    
    call ExitProcess
