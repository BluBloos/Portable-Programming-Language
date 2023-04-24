aFunc :: #save_code (a:int, b:int) -> int {
    return a + b;
}

aFunc(10, 20); // macro insertion.

appendCodes :: #save_code (a : Code, b : Code) -> Code {
    // TODO: something like this.
    #insert a;
    #insert b;
    // each #insert statement literally just takes the code AST and inserts it
    // as a stream.
    // so if we do two in a row here 
}

thing := appendCodes(a, Func); // call b with two ASTs. each of the nodes are identifier nodes.

thing(10, 30);

