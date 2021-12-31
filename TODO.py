'''
Need a language that is a superset to C.

Targets:
- Unity
- Unreal Engine
- Writing code for the web
- IOS App
- Android App
- Apple Watch
- Garmin Watches
- Operating Systems
    Windows, macOS, Linux
- Writing code for the Cloud
- Augmented Reality Experiences (OpenXR?)
- Writing code to run on Graphics Cards
- Binary, and for different architectures (for making Operating Systems)
- Arduino

Things to include in Standard Library:
- Blockchain
- Distributed Compute
- Bluetooth
- Wifi

PPL should also be able to insert code for running Javascript as well as Python interpreters.
Just be able to write these languages inline.
I also believe that it would be very nice to write assembly inline.
Maybe even support Go? I have never used Go...

There should exist a custom language server and appropriate Visual Studio Code extensions for this language.

Language should have a good profiler, a good debugger, and a good editor.
When we think about a good editor, we think about virtual whitespace + DEXTER copilot.

Potentially a drag and drop interface for creating GUIs.

Novel Target Platform:
- Custom web server
    - Serves and receives novel content models
        - Stream Video, Audio, User Input. Augmented reality experiences.
        - Think "Writing a program and sharing it in seconds"
            - Like, all other targets are transimissble...?
- Custom web client

Finally, think about device schronicity. Like the kind of ecosystem that Apple has, but we make this extremely cross-device.

When you include something from the standard library

'''

# Below is like actual TODO and not vision like above.

'''
# TODO(Noah): Language features to add.
- No need for forward decl of functions.
'''

'''
# TODO(Noah): To add many more operators.
- The attribute of a variable . (also put implicit derefencing in there)
- function call on a variable ()
- indexing on a variable []
- +=, -=, and this whole family
# https://en.cppreference.com/w/c/language/operator_precedence
'''

'''
TODO(Noah): Check out the things below,
- Consider branching and Phi nodes for compiler optimization.
- Check out what vectorizin is.
- Look into quantization (making ops smaller size)?
- What is a GPR?
'''

'''
# NOTE(Noah): Thoughts on what to write for the Lexer.

Logger stuff brainstorm

r"[(function_decl)(function_impl)(var_decl)(struct_decl)]*"
r"(type)(symbol)\(((lv),)*(lv)?\)[;(block)]"

((lv),)
Suppose we try to parse a group like this.
this is like parsing something on the regular.

like "(lv),"

so if we fail on the ,
we simply print that this is how we failed.

but if we fail on the (lv), this contains many sub-components.

r"(type)(symbol)"

can fail on either type or symbol.

type has sub-components.

r"[((symbol)::(symbol))(symbol)(keyword)]"

if type fails, (which is an Any).
- we simply say "Expected one of : ___, ___, ___"

if we fail on symbol we simply say "Expected symbol".

What happens if the components of the Any block are sufficiently advanced?
- Like we are looking at a program and it could have been either of the struct, program, or var_decl.

Maybe we have like a "best guess" type of thing. The options of the Any that got the most amount 
of tokens matched before failure. We store different error buffers for each child.

- Then we print the errors of this sub-component.

Okay now what about the modifiers???
- Gets quite complicated...

'''

'''
TODO(Noah):
We want to get the console target working. Along with linking with a standard library.
What do we need to do to get this example working?

- Parse \n as newline PLEASE :(

- basic ppl::scan implementation
- string data type
- utf8 string literals
- prints should work with utf8, scanf's too
- ppl::print_color implementation
- running system commands
- add to the grammer -> in the type grammer (pointer type).
- add the alloc keyword as an expression (kind of like var assignment/func decl).
- add the free keyword as a statement.
- enum for colors, Ex) PPL::RED
- >> operator for piping function calls to object instances
    - I'll have it in the syntax parser as an expression, because '>>' is an operator.
    - But I will NOT put any codegen to it because I don't even have objects...

- Need to implement proper targeting of platforms.
    - Right now it is really difficult to look forward and be like: 'Yes, this is how
    we mght implement the other platforms and therefore we should prepare by architecting
    our code like ___.
    Not going to work. Let's just keep it in mind that this is the end goal. In the meantime
    everything is to target the console target.

'''