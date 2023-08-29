.extern p_decl void ppl_console_print(int64, []int64)
.extern p_decl void ppl_exit(int32)

.section code

.def int64 main()

mov r2, 0
bgt r2, 0, main_tern_0
mov r2, msg2
call ppl_console_print(r2)
br main_tern_1

label_main_tern_0:

mov r2, msg3
call ppl_console_print(r2)

label_main_tern_1:

br main_end

label_main_end:

call ppl_exit(r2_32)

.section data

label_b:
.db 7
label_c:
.db 8
label_d:
.db 0
label_msg2:
.db "Hellohi"
.db 0
label_msg3:
.db "Hello"
.db 0