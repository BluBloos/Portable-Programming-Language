struct A {
    int a;
    int b;
    char c;
    float aa;
};

int b = 7; // global variable!! :)

int main(int argc, A largeLad) {

    // Lmao note that the . operator simply is not implemented. So sad!
    if (5 > argc) {
        printf("Mans is large");
    }

    // really need to implement the j++ thing...
    for (int j; j < argc; j = j + 1) {
        argc = j + 1; // this should be fun.
        while (true); // also fun.
    }

    {
        // simple scope
        int a = 4;
    }

}