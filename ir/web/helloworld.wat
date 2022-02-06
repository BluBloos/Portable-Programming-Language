;; this document is in reference to https://developer.mozilla.org/en-US/docs/WebAssembly/Understanding_the_text_format

;; webassembly is built as an S-expression. Represents a tree.
;; we enclose a tree in (), the leftmost thing is the root, and any subsequent
;; () would be children of that tree.

;; So in fact, the code below is the simplest possible webassembly module.
;; (module)

;; uhm, all code in webassembly is grouped into functions.
;; ( func <signature> <locals> <body> )
;; and we note that the signature is 0 or more param nodes with 0 or 1 result node.

;; wasm has the following types
;; i32: 32-bit integer
;; i64: 64-bit integer
;; f32: 32-bit float
;; f64: 64-bit float
;; NOTE(Noah): Nice :)

(module
    ;; we can even go ahead and import a Javascript function into webasm if we desire.
    ;; NOTE: This function takes in the linear offset of some string in memory, along with the
    ;; length of the string.
    (import "console" "log" (func $log (param i32) (param i32)))
    ;; Here we are creating the program memory in javascript and importing it into
    ;; webassembly. Note that the 1 defines that the imported memory must have at least
    ;; 1 page of memory (64 KB).
    (import "js" "mem" (memory 1))
    (data (i32.const 0) "Hello, World!") ;; this puts the Hi in memory at linear offset 0
    ;; We note that using local.get requires the numeric index of the local.
    ;; params are locals and take up the first indices, then followed by an local
    ;; definitions that we make, it seems.
    (func (param i32) (param i32) (local i32)
        local.get 0
        local.get 1
        local.set 2
        local.set 2
    )
    ;; Now, we notice that using local.get by index sucks. We can name things like so
    (func $myFunc (param $param1 i32) (param $param2 i32) (result i32)
        ;; NOTE(Noah): As I read more and more into Webassembly, it just gets wierder and wierder!!
        ;; So there exists this concept of the stack machine. Any instruction either push or pop to it.
        local.get $param1 ;; pushes to stack
        local.get $param2 ;; pushes to stack
        i32.add ;; this will pop the last two instructions from stack, compute result, and push result.
        ;; and we note that the return value of a function is just the final value left on the stack. 
    )
    (func $myFunc2 (param $a i32) (result i32)
        local.get $a
        i32.const 3
        i32.sub
        ;; function should return a - 3
    )
    ;; Here is an example of calling another function from the same module.
    (func $myFunc3 (result i32)
        i32.const 5
        call $myFunc2
    )
    ;; Like an ES2015 module, we need to export our functions.
    (export "myFunc" (func $myFunc))
    (func (export "logIt")
        i32.const 0
        i32.const 13 
        call $log
    )
    ;; TODO(Noah): Understand WebAssembly globals
    ;; TODO(Noah): Learn the rest of WebAssembly via https://developer.mozilla.org/en-US/docs/WebAssembly/Understanding_the_text_format
)