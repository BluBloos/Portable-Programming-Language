pc : #import "ppl.console"

main : () -> int
{
    a:float = 10.2;
    p : ^int = (^int)&a;
    // NOTE(Noah): Right now we are not going to implement implicit dereference.
    // It might be better to always know that you are dealing with a pointer.
    // To prevent you from forgetting that you are in fact dealing with a pointer. 
    pc.print("%d\n", ^p);
    pc.print("%f\n", ^(^float)p);
}