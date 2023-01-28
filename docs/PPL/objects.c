// objects are pretty basic.
// struct.
// with a vtable to point to all struct member or static funcs.
// member funcs take in the "this" pointer (implicit).

// TODO:
// the vtable allows for inheritance and polymorphism?

// the whole point of the vtable is to say that for any object instance
// of a particular struct, that the funcs in the table are NOT const.
// with this ability, we do in fact get polymorphism.
// because can have "reference" to type A, which is parent to type B.
// B overrides method in A. but caller of A has no idea, it just sees
// a consistent interface. and in fact, the reference to A can be an object of
// type B. because after all, inheritance is this purely additive kind of deal.
//
// so you just ensure all var members of B come after in mem of those in A.
// and same with vtable.
// so we can trivially truncate and look at the object B as if it was A.

// a struct that is NOT anonymous.
// this creates a type called MyBoy. 
struct MyBoy
{
    int m_a;
    int m_b;
    int Add()
    {
        return m_a + m_b;
    }
};

int main() {
    // this is an anonymous struct.
    // we define a var called point.
    // no type is defined.
    struct {
        int32_t x;
        int32_t y;
    } point;

    // this is both a definition of the type MyBoy2,
    // PLUS we create a var called myBoy.
    struct MyBoy2
    {
        float A;
        float B;
    } myBoy2;

    // both work.
    MyBoy2 c;
    myBoy2 d;

    // if we use a "typedef instruction"
    // this creates a type called myBoy3_t
    // and alias it to the anonymous struct.
    // this is functionally the same as just
    // defining struct with syntax as used for MyBoy.
    typedef struct
    {
        double A;
        double B;
    } myBoy3_t;

    // not allowed.
    typedef struct MyBoy3
    {
        double A;
        double B;
    } myBoy3_t;


}