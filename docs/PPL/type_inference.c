// so, I didn't like type inference because it wasn't explicit.

// however, there IS a version that does the explicit thing.

// consider the following,

// everything below is unambiguous but terser than the explicitly-typed version.

a := 10.f;   // this is unambiguously a float.
b := 10.0;   // this is a double.
c := 100u;   // u = uint32.
// ...

d := c; // this is OK too.


