'''
# TODO(Noah):
- No need for forward decl of functions.
- add the alloc keyword as an expression (kind of like var assignment/func decl).
- add the free keyword as a statement.
- >> operator for piping function calls to object instances
    - I'll have it in the syntax parser as an expression, because '>>' is an operator.

# TODO(Noah): To add many more operators.
- The attribute of a variable . (also put implicit derefencing in there)
- function call on a variable ()
- indexing on a variable []
- +=, -=, and this whole family
# https://en.cppreference.com/w/c/language/operator_precedence

TODO(Noah): Check out the things below,
- Consider branching and Phi nodes for compiler optimization.
- Check out what vectorizin is.
- Look into quantization (making ops smaller size)?
- What is a GPR?
'''

'''
TODO(Noah):
We want to get the unix console target working. Along with linking with a standard library.
What do we need to do to get this example working?

Terminal things
- ppl::get_attached_term()
- ppl::terminal
- ppl::terminal::get_attr()
- ppl::terminal_attr
- ppl::terminal::set_atr(ppl::terminal_attr)
- ppl::terminal::set_default_attr()

Other standard libs
- ppl::seed_rand()
- ppl::time()
- ppl::rand()
- ppl::system(string str);
- ppl::flush()
- ppl::block_until_fready(ppl::STDIN, 0)
- ppl::getchar()
- ppl::sleep(100000);

Language Features:
- const modifier on types.
- initializer lists
    Ex) const []char FOOD_SYMBOLS = {'%', '$', '&', '@', '+'};
- dynamic arrays
    Ex) [dynamic]snake_body snake;
    snake.append(body);
- accessor operator
    Ex) body.x
- ++ op
    Ex) for (int i(0); i < 5; i++)
- [] operator
    Ex) snake[0].x
- len operator on arrays (static or dyanmic)
    Ex) for (int i = 1; i < len(snake); i++) {
- bool datatype
- modulus operator 
    Ex) ppl::rand() % (MAP_WIDTH - 3);
- sizeof operator
    Ex) ppl::rand() % sizeof(FOOD_SYMBOLS);
- double variable initialiazation
    Ex) int last_x, last_y;
- replacement of default with case:
    Ex) case: break; // NOTE(Noah): Same as default:
- += op
    Ex) SCORE += 7;
- bitwise OR, &= (bitwise and equals), ~ (whatever this unary op is)
    Ex) attr.local_modes &= ~( ICANON | ECHO );
'''

'''TODO
- Need to implement proper targeting of platforms.
    - Right now it is really difficult to look forward and be like: 'Yes, this is how
    we mght implement the other platforms and therefore we should prepare by architecting
    our code like ___.
    Not going to work. Let's just keep it in mind that this is the end goal. In the meantime
    everything is to target the console target.
'''

'''
# NOTE(Noah): utf8 things.
string we recieve as input from the user are based on the locale set in the system.
anything we pass to printf is also supposed to be in this encoding.
wide characters are NOT utf8. They are consistent width, 32 bit per character encodings.
wprintf and %ls do these wide character things.
we want multibyte encoding.
'''


'''
NOTE: Things I like from the Odin programming language.
https://odin-lang.org/

Note that I do not like the entire language, and hence the reason for a new one.
Like, it's not digestible. It is TOO different from C...
Need something easily digested.
Odin is literally backwards.
like...
    a : int 
that shit is backwards.

TODO: List of things I like that we should implement.

    - 0b01010101 (binary literals)
    - 0xFF (Hexadecimal literals)
    - Complex numbers, 2i + 3
    - 1.0e9 (scientific notation)
    - 1_000_000_000 (underscores for better readability)
    - 64 bit floating point numbers
    - a, b = 3 (multi-variable assignment)

    - removed while loops and make them a for loop...

    (range-based for-loop)
    - for int j in 0..<10 
        // Certain built-in types can be iterated over
        some_string := "Hello, 世界"
        for character in some_string { // Strings are assumed to be UTF-8
            fmt.println(character)
        }
        for value in some_array {
            fmt.println(value)
        }

    - (indices in range-based for-loops)
        for char character, int index in some_string {
            fmt.println(index, character)
        }

    - (range-based for-loop, iterate by reference)
        for ->char character, int index in some_string {
            fmt.println(index, character)
        }

    - when statement. Constant expression evaluated at compile time. I see this as a replacement
    for #ifdef except we can do expressions as well.

        when ODIN_ARCH == "386" {
			fmt.println("32 bit")
		} else when ODIN_ARCH == "amd64" {
			fmt.println("64 bit")
		} else {
			fmt.println("Unsupported architecture")
		}
    
    - implicit dereference has the benefit of when you need to refactor code to use
    a pointer versus a non-pointer, you don't have to go around and do all these
    '.' vs '->' op changes.
    # NOTE(Noah): Still need to think about this. It's GOOD to know that you are working w/ a pointer.

    - struct literal
    v = Vector3{1, 4, 9}

    - make Quaternion a default type.

    - # NOTE(Noah): Consider polymorphism.

    - functions defined inside the scope of a another function definition.
        - NOTE(Noah): This might be one of my favourite features :)
    
    - lambda functions.
        - NOTE(Noah): I also really like this one. Not sure if from Odin, but from Python.

    - list initializers 
        i.e. []int myArray = {3,4,5,6,7};

    - add a map type (like a hash map)
        - NOTE(Noah): I love these :)

    - list slicing
        - NOTE(Noah): Fun :) One of my personal favs from Python

    - big numbers and precision mathematics.

    - put vectors and matrices in the language.
        - kind of like numpy. Make it good for that type of shit.
'''