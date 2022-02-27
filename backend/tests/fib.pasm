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
; r2 is used for function returns.
; parameters are passed with general purpose registers, starting with r2.
; but this is only true for the first four parameters. Any more, and things
; are passed via the stack.
.def int32 fib(int32 n)

save [r3]

label_fib_if:
bgt n, 1, fib_else
br fib_end

label_fib_else:
sub n, 1
call fib(n)
mov r3, r2
sub n, 1
call fib(n)
add r2, r3
br fib_end

label_fib_end:
restore [r3]
ret

.def int32 main()
call fib(7)
call ppl_exit(r2)

; so maybe to avoid the botching of registers that I can forsee, we do the
; following. We still always return in r2. So everyone agrees on this. 
; but when passing parameters, things should always happen via the stack.
