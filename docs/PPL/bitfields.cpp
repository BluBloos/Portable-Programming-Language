// support for bitfields.
MyStruct: struct {
    a : int : 3; // how this works is that int:3 is a type. it is a modified int type sort of deal.
    b : int : 5;
    c : int : 4;
};

// bitfields just restrict the range of possible values for our var.
// and if we pair bitfields with the pack attribute, we can get really small structs.
// otherwise, the compiler by default will still organize the struct for maximum speed,
// with the assumption that memory is more readily available.

// notice that pack can take parameters. this is an alignment - default is 1 bit.
MyPackedStruct : [[pack(1)]] struct {
    a : int : 3;
    b : int : 5;
    c : int : 4;
};

// functionally less bits. this means that arithmetic operations on bitfields
// are not allowed, unless they have the same width.

// bitfields are of course allowed outside of structs. as normal vars.
// bitfields overall cannot have a width larger than the type they are declared as.
a :short : 17; /* Illegal! */
y :int   : 33; /* Illegal! */

// thus, the bool type is actually an alias for uint8_t : 1!
myBitfield : bool;
