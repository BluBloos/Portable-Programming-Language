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

    ; NOTE(Noah): lpNumberOfBytesWritten must not be NULL when lpOverlapped is NULL.

    mov rcx, QWORD [rel PPL_STDOUT]
    mov rdx, QWORD [rbp + 16]
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