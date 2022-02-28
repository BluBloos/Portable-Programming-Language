; What is the C calling convention again?
; Things are passed on the stack, right?
;   Well...I mean. We can do whatever the fuck we want
;   anyways :)

; So let's go for a fast-calling convention.
; First four registers are used to pass shit, then subsequent
; params are passed on the stack.
global ppl_console_print
ppl_console_print:
    ; rcx = msg (which is a null-terminated string).

    push rbp ; save the stack frame
    mov rbp, rsp ; save the stack pointer.
    push rdx
    push r8
    push r9
    push rdi
    push rsi
    push rax

    ; Get the length of the string
    mov rdx, 0
    mov r8, rcx
    p_while:
        mov r9, [r8]
        add r8, 1
        cmp r9, 0 
        je p_while_end
        add rdx, 1
        jmp p_while
    p_while_end:

    ; user_ssize_t write(int fd, user_addr_t cbuf, user_size_t nbyte);
    mov rax, 0x2000004
    mov rdi, 1 ; stdout
    mov rsi, rcx
    syscall

    pop rax
    pop rsi
    pop rdi
    pop r9
    pop r8
    pop rdx
    mov rsp, rbp ; restore the stack pointer
    pop rbp ; restore the stack frame

    ret