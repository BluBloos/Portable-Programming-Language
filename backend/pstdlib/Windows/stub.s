extern GetStdHandle
global ppl__stub
global PPL_STDOUT
section .text
ppl__stub:
    ; see https://docs.microsoft.com/en-us/windows/console/getstdhandle
    mov rcx, 4294967285 ; STD_OUTPUT_HANDLE
    call GetStdHandle
    mov QWORD [PPL_STDOUT], rax
    ret
section .data
PPL_STDOUT:

