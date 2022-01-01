#include <ppl/console>

int main()
{
    float a = 10.2;
    ->int p = (->int)&a;
    // NOTE(Noah): Right now we are not going to implement implicit dereference.
    // It might be better to always know that you are dealing with a pointer.
    // To prevent you from forgetting that you are in fact dealing with a pointer. 
    ppl::print("%d\n", *p);
    ppl::print("%f\n", *(->float)p);
}