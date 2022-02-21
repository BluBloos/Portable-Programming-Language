.extern p_decl void ppl_console_print(int64)
.extern p_decl void ppl_exit()

.section code
label_main:
  call ppl_console_print(msg)
  call ppl_exit()

.section data
label_msg:
.db "Hello, world!", 10, 0

; everything is one line.
; anything that is not a command starts with .
; labels start with label_, end with :
