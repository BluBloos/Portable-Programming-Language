// NOTE(NOAH): Pointers not here yet, so char **argv is going to have to wait...
main := ( argc : int, argv : ^^char) -> int {
    while i : int = 0; i < argc; i += 1 
        do printf("parameter %s", argv[i]  );
}