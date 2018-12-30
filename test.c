//--------------LEXER-----------------
//TODO(Noah): Currently, our lexer is a little too good because it ignores spaces when analyzing tokens
//TODO(Noah): Either refactor the lexer or switch to using an external lexer.
//If we choose to refactor, maybe we can explore regular expressions?
//Remember, speed is important, if not the most important concern when lexing.
//Imagine giving python millions of characters. It should lex FAST. Because we want our compile times to be
//SUPER FAST
//Maybe we can migrate the compiler code base to C?
//TODO(Noah): Add mutlti-line comments!

//------------OPTIMIZATION-----------------
//TODO(Noah): Add an optimization pass to remove redudant code, or just don't generate it in the first place (faster option).

//-------------SYNTAX---------------
//TODO(Noah): add compound operators like += and -= and such (somewhat challenging)

int _WinMain@16()
{
  int wassup = 4;
  {
    int gotem = wassup;
    int wassup = 5;

    //int hello = gotem;
    //hello = 2 * (4 + 5) / 4 - 3 == (3 >= hello);
    //if (hello == 0) hello = 4; //else if(hello == 2) hello = 3; else hello = 5;
  }
  return wassup;
}
