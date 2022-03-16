; TODO(Noah): We are going to make ppl_console_print
;   take in variadic arguments (like the c printf func).
;   in doing this, we begin with %d.
;   Algo for converting integer to string:
;     first check for the sign of the integer, print '-' accordingly.
;     then do a while loop where we continue to divide by 10, and use 
;     the remainder as the value to print. Push the remainder (plus '0') on the stack.
;     The dividing by 10 part reduces the number by 1 place value.
;     Once we are done we can pop all the characters from the stack to be placed
;     into the buffer for printing.

; NOTE(Noah): We note that the names of these functions
;   mimic what the qualified name would be in PPL source.
;   so if the name in PPL source is ppl::console::print, 
;   then the name in asm is ppl_console_print. Cool, right?

; NOTE(Noah): ppl_console_print works by printing to stdout.

; BOOL WriteFile([in] HANDLE hFile, [in] LPCVOID lpBuffer, [in] DWORD nNumberOfBytesToWrite, 
;   [out, optional] LPDWORD lpNumberOfBytesWritten, [in, out, optional] LPOVERLAPPED lpOverlapped);
extern WriteFile
extern PPL_STDOUT

; TODO(Noah): Add in the saving of registers so that things are consistent between calls.
global ppl_console_print
; .extern p_decl void ppl_console_print(int64)
section .text
ppl_console_print:

    push rbp ; save the stack frame
    mov rbp, rsp ; save the stack pointer.

    ; Intialize the pointer to the memory.
    mov rcx, ppl_console_print_storage

    ; Check the sign of the integer.
    ; ppl_console_print_if:
    ; mov rdx, QWORD [rbp + 16]
    ; cmp rdx, 0
    ; jge ppl_console_print_endif
    ; neg rdx
    ; mov [rcx + 0], '-'
    ; add rcx, 1
    ; ppl_console_print_else:
    ; ppl_console_print_endif:

    ; Get the length of the string while going thru each character, checking for 
    ; the '%' sign, and outputting the characters to the buffer.
    xor r8, r8 ; let r8 be the counter.
    mov rdx, QWORD [rbp + 16]
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
            add r8, 1
            mov BYTE [rcx], 'D'
            inc rcx
            ppl_console_print_endif3:

        jmp ppl_console_print_endif2

        ppl_console_print_else2:
        add r8, 1
        mov [rcx], r9b
        inc rcx
        ppl_console_print_endif2:

        jmp ppl_console_print_while

    ppl_console_print_endwhile:

    ; NOTE(Noah): lpNumberOfBytesWritten must not be NULL when lpOverlapped is NULL.

    mov rcx, QWORD [rel PPL_STDOUT]
    mov rdx, ppl_console_print_storage
    mov r8, r8 ; nNumberOfBytesToWrite
    push QWORD 0 ; allocate storage on stack for lpNumberOfBytesWritten
    mov r9, rbp
    add r9, -8 ; point lpNumberOfBytesWritten to space alloced on stack
    ; here we set lpOverlapped to NULL
    ; Also note that we make the safe bet of assuming we must push to the stack
    ; a full-blown 64 bit value.
    push QWORD 0

    ; for an explanation of the "register homes", see https://docs.microsoft.com/en-us/cpp/build/stack-usage?view=msvc-170
    sub rsp, 32 ; r9, r8, rdx, and rcx homes

    call WriteFile

    ; return value for WriteFile is true if func succeeds.
    ; otherwise is zero (false).
    ; We can use GetLastError to get extended error information.

    mov rsp, rbp ; restore the stack pointer
    pop rbp ; restore the stack frame

    ret

section .data
ppl_console_print_storage: resb 256