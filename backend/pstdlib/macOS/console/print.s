; TODO(Noah): Abstract out the common print functionality between print.s for macOS
; and print.s for Windows.

; .extern p_decl void ppl_console_print(int64)

global ppl_console_print
ppl_console_print:

    push rbp ; save the stack frame
    mov rbp, rsp ; save the stack pointer.

    push rcx
    push rdx
    push r8
    push r9
    push r10
    push rdi
    push rsi
    ; NOTE(Noah): no need to save rax and rbx as these are reserved for the compiler, so the callee will
    ; never use them. aka, they are volatile across func calls.

    ; Intialize the pointer to the memory.
    mov rcx, ppl_console_print_storage

    ; Get the length of the string while going thru each character, checking for 
    ; the '%' sign, and outputting the characters to the buffer.
    xor r8, r8 ; let r8 be the counter.
    mov r10, QWORD [rbp + 16] ; frame pointer
    mov rdx, QWORD [r10 + 0] ; the string
    ppl_console_print_while:

        mov r9b, BYTE [rdx]
        add rdx, 1 ; Get byte and move string pointer to next byte.
        
        cmp r9b, 0  
        je ppl_console_print_endwhile ; check for valid byte
        
        ; Check if the character is equal to '%'
        ppl_console_print_if2:
        cmp r9b, '%'
        jne ppl_console_print_else2

            mov r9b, BYTE [rdx] ; look at next byte
            inc rdx
            ; TODO(Noah): Is there a better way to do this? Maybe like, a jump table or something?
            ppl_console_print_if3: 
            cmp r9b, '%'
            jne ppl_console_print_elseif3
            jmp ppl_console_print_else2
            ppl_console_print_elseif3:
            cmp r9b, 'd' ; TODO(Noah): Do we need to execute the cmp instr twice?
            jne ppl_console_print_endif3
            
            ;;;;;;;;; Routine for printing an integer ;;;;;;;;;
            push rdx ; save rdx (the string)
            xor rax, rax
            xor rdx, rdx

            ; retrieve the integer.
            mov rax, [r10 - 8]
            sub r10, 8 ; iterate to the next variadiac argument. 

            ; Check the sign of the integer.
            ppl_console_print_if:
            cmp rax, 0
            jge ppl_console_print_endif
            neg rax
            add r8, 1
            mov BYTE [rcx + 0], '-'
            add rcx, 1
            ppl_console_print_else:
            ppl_console_print_endif:

            ; now that we have checked the sign, we can actually print the integer.
            ; note that the loop below is a do-while loop.
            xor r9, r9 ; init for counter.
            ppl_console_print_while2:
                
                ; unsigned integer divide
                ; the thing to be divided is stored in rdx:rax
                ; the quotient is stored in rax, and the remainder in rdx.
                xor rdx, rdx ; ensure we zero again.
                mov rbx, 10
                div rbx
                ; use the remainder as the digit
                mov rbx, rdx
                add rbx, '0'
                push rbx
                add r9, 1 ; counter += 1

                ; check the condition of the while loop.
                cmp rax, 0
                je ppl_console_print_endwhile2 ; we stop printing number once we hit 0
                jmp ppl_console_print_while2

            ppl_console_print_endwhile2:

            ; pull the string representation of the integer off the stack and into the
            ; destination for printing.
            ppl_console_print_for:
                pop rbx
                add r8, 1
                mov BYTE [rcx + 0], bl
                add rcx, 1    
                ; for-loop post condition.
                dec r9    
                ; check the condition of the for-loop.
                cmp r9, 0
                jg ppl_console_print_for
            ppl_console_print_endfor:

            pop rdx
            ;;;;;;;;; Routine for printing an integer ;;;;;;;;;
            
            ppl_console_print_endif3:

        jmp ppl_console_print_endif2

        ppl_console_print_else2:
        add r8, 1
        mov [rcx], r9b
        inc rcx
        ppl_console_print_endif2:

        jmp ppl_console_print_while

    ppl_console_print_endwhile:

    ; write null-termination byte
    mov BYTE [rcx], 0
    inc rcx

    ; user_ssize_t write(int fd, user_addr_t cbuf, user_size_t nbyte);
    mov rax, 0x2000004
    mov rdi, 1 ; stdout
    mov rsi, ppl_console_print_storage
    mov rdx, r8
    syscall

    pop rsi ; restore all the saved registers.
    pop rdi
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx

    mov rsp, rbp ; restore the stack pointer
    pop rbp ; restore the stack frame

    ret

section .data
ppl_console_print_storage: resb 256