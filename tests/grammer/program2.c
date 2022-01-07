struct A {
    float b;
    float c;
};

int main() {
    A myStruct = ['3', "Hello!", 3, 45.7]; // initializer list.
    A.b = 3;
    A.c = 6 * A.b.c.d;
    &A.c("Epic func call");
}