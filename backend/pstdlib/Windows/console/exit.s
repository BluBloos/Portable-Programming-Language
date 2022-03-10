extern ExitProcess
global ppl_exit
section .text
ppl_exit:
    xor rcx, rcx 
    mov ecx, DWORD [rsp + 8]    
    call ExitProcess
