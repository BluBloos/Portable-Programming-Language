// NOTE(NOAH): Pointers not here yet, so char **argv is going to have to wait...
main : (argc:int, argv:char) ->int {
    for (i:int = 0; i < argc; i = i + 1)
        printf("parameter %s", argv); // Also note the lack of [] operator...
}