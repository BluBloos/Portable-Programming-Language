; #include <stdio.h>
;
; int fib(int n)
; {
;     if (n <= 1)
;         return n;
;     return fib(n - 1) + fib(n - 2);
; }
;
; int main()
; {
;   return fib(7); // should return 13
; }

.extern p_dcel void ppl_exit(int32)

.section code

; NOTE(Noah): Internal function calls such as this
; use the pdecl calling convention.
; In pdcel, r0 and r1 are reserved.
;     Edit: What are they reserved for? For now,
;     the compiler.
; to avoid the botching of registers that I can forsee, we do the
; following. We still always return in r2. So everyone agrees on this. 
; but when passing parameters, things should always happen via the stack.
.def int32 fib(int32 n)

save [r3] ; If we do not specify the size of reg, it is 64.

label_fib_if:
bgt n, 1, fib_else
br fib_end

label_fib_else:
sub n, 1
call fib(n)
; We must specify what size of registers.
mov r3_32, r2_32
sub n, 1
call fib(n)
add r2_32, r3_32
br fib_end

label_fib_end:
restore [r3]
ret

.def int32 main()
call fib(7)
call ppl_exit(r2_32)


