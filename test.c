//--------------LEXER-----------------
//TODO(Noah): Currently, our lexer is a little too good because it ignores spaces when analyzing tokens
//TODO(Noah): Either refactor the lexer or switch to using an external lexer.
//If we choose to refactor, maybe we can explore regular expressions?
//Remember, speed is important, if not the most important concern when lexing.
//Imagine giving python millions of characters. It should lex FAST. Because we want our compile times to be
//SUPER FAST
//Maybe we can migrate the compiler code base to C?
//TODO(Noah): Add mutlti-line comments!

//-------------CODE GEN-----------------
//TODO(Noah): Make the asm output more pretty. It's pretty hard to read the output.
//TODO(Noah): We have to make sure that functions return on all code paths.

//------------OPTIMIZATION-----------------
//TODO(Noah): Add an optimization pass to remove redudant code, or just don't generate it in the first place (faster option).

//-------------SYNTAX---------------
//TODO(Noah): add compound operators like += and -= and such (somewhat challenging)
//TODO(Noah): add increment and decrement operators
//TODO(Noah): Add the calling of functions
  //make sure that the caller provides enough params.
  //functions are expressions?
  //But they can also be statements, like just calling a function on one line.
  //so grammar looks like this
  //<function call>:: symbol (params)
  //<expression>::<function call> | <exp>
  //<statement>::<function call> ; | <exp>
//TODO(Noah): Add switch statements, but they shouldn't require the break keyword
//TODO(Noah): Function parameters can have default values
//TODO(Noah): Add structs
//TODO(Noah): Add different types (int, short, char, float, double)
//TODO(Noah): Add type modifiers (unsigned, long)
//TODO(Noah): Add pointers
//TODO(Noah): Add lists (slices if ethical xP, and maybe dictionaries)
//TODO(Noah): Make function pointers super elegant, please....
//TODO(Noah): Add unions
//TODO(Noah): Implement bit sets like in Odin
//TODO(Noah): Add the modulo boolean operator
//TODO(Noah): Implement do-while loops!

//--------------------PREPROCESSING---------------------------
//Add #define
//Add #include

int Sum(int a, int b)
{
  return a + b;
}

int _WinMain@16()
{
  int gotem = Sum(4, 5);
  //int gotem = 5;
  return gotem;
}
