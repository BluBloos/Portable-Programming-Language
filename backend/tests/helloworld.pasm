.extern p_decl void ppl_console_print(int64)
.extern p_decl void ppl_exit(int32)

.section code
.def int32 main()
call ppl_console_print(msg)
call ppl_exit(0)

.section data
label_msg:
.db "Hello, world!"
.db 10
.db 0

; Ruleset that promotes easy parsing of pasm:
;   everything is one line.
;   anything that is not a command starts with .
;   labels start with label_, end with :
;   things are whitespace sensitive.
;   there are 32 general purpose registers named r0 - r31.
