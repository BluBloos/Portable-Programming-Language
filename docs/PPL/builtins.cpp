sizeof(int);
struct C { int x; };
C c = {};
offsetof(c.x);
countof(someEnum);
cprintf("hello %s", "world"); // with constexpr args

// consider: offsetof(c.x);
// to get the above to work seems we need it to take a reference to a member, right?