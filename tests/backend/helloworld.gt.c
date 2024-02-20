// NOTE: this function should populate the sOut array;
// this is the printout of the program. it should also
// provide the return code.
int GENERATE_GROUND_TRUTH(std::vector<const char *> &sOut)
{
    sOut.push_back("Hello, world!");
    sOut.push_back("Number one: -2456, and number two: 67");
    return 0;
}