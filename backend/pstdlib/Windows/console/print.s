; NOTE(Noah): We note that the names of these functions
;   mimic what the qualified name would be in PPL source.
;   so if the name in PPL source is ppl::console::print, 
;   then the name in asm is ppl_console_print. Cool, right?


; NOTE(Noah): ppl_console_print works by printing to stdout.

; BOOL WriteFile([in] HANDLE hFile, [in] LPCVOID lpBuffer, [in] DWORD nNumberOfBytesToWrite, 
;   [out, optional] LPDWORD lpNumberOfBytesWritten, [in, out, optional] LPOVERLAPPED lpOverlapped);
extern WriteFile
extern PPL_STDOUT

global ppl_console_print
; .extern p_decl void ppl_console_print(int64)
section .text
ppl_console_print:

    push rbp ; save the stack frame
    mov rbp, rsp ; save the stack pointer.

    ; Get the length of the string
    xor r8, r8
    mov rdx, QWORD [rbp + 16]
    p_while:
        mov r9b, BYTE [rdx]
        add rdx, 1
        cmp r9b, 0 
        je p_while_end
        add r8, 1
        jmp p_while
    p_while_end:

    mov rcx, QWORD [rel PPL_STDOUT]
    mov rdx, QWORD [rbp + 16]
    mov r8, r8 ; nNumberOfBytesToWrite
    xor r9, r9 ; here we set lpNumberOfBytesWritten to NULL
    ; here we set lpOverlapped to NULL
    ; Also note that we make the safe bet of assuming we must push to the stack
    ; a full-blown 64 bit value.
    push QWORD 0 
    call WriteFile
    

    mov rsp, rbp ; restore the stack pointer
    pop rbp ; restore the stack frame

    ret