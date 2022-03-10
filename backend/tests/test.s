extern ppl_exit
global start
section .text
start:
push QWORD 10
call ppl_exit
add rsp, 8