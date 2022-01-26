// TODO(Noah): Note that this implementation of memory arena is VERY unsafe.
// We must check if allocation requests are valid, and fail in the case that they
// are not.
class ConstMemoryArena {
    public:
    ConstMemoryArena(unsigned int bytes) {
        _base = malloc(bytes);
        dataPtr = (char *)_base;
        totalDataBytes = bytes;
    }
    ~ConstMemoryArena() {
        free(_base);
    }
    void *_base;
    char *dataPtr;
    unsigned int totalDataBytes;
    void *_Alloc(void *data, unsigned int dataSize) {
        void *result = memcpy(dataPtr, data, dataSize);
        dataPtr += dataSize;
        return result;
    }
    char *StdStringAlloc(std::string str) {
        unsigned int stringSize = (str.size() + 1) * (sizeof(char)); // Includes null-terminator.
        return (char *)_Alloc((void *)str.c_str(), stringSize);
    }
    char *StringAlloc(char *str) {
        unsigned int stringSize = 0;
        for (char *pStr = str; *pStr != 0; pStr++) {
            stringSize++;
        }
        stringSize = (stringSize + 1) * sizeof(char); // Includes null-terminator.
        return (char *)_Alloc(str, stringSize);
    }
};