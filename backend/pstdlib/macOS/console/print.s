; What is the C calling convention again?
; Things are passed on the stack, right?
;   Well...I mean. We can do whatever the fuck we want
;   anyways :)

; So let's go for a fast-calling convention.
; First four registers are used to pass shit, then subsequent
; params are passed on the stack.

; TODO(Noah): Convert from Nios 2 to the proper x86_64 assembly.
ppl_console_print:
    ; rcx = msg (which is a null-terminated string).

    subi sp, sp, 48
    stdw rdx, 0(sp)
    stdw r8, 8(sp)
    stdw r9, 16(sp)
    stdw rdi, 24(sp)
    stdw rsi, 32(sp)
    stdw rax, 40(sp)

    ; Get the length of the string
    movi rdx, 0
    mov r8, rcx
    p_while:
        ldw r9, 0(r8)
        addi r8, r8, 1
        beq r9, 0, p_while_end
        addi rdx, rdx, 1
        br p_while
    p_while_end:

    ; user_ssize_t write(int fd, user_addr_t cbuf, user_size_t nbyte);
    mov rax, 0x2000004
    mov rdi, 1 ; stdout
    mov rsi, rcx
    syscall
    
    lddw rdx, 0(sp)
    lddw r8, 8(sp)
    lddw r9, 16(sp)
    lddw rdi, 24(sp)
    lddw rsi, 32(sp)
    lddw rax, 40(sp)
    addi sp, sp, 48

    ret