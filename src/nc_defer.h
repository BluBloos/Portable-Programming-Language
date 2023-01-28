// taken from https://www.gingerbill.org/article/2015/08/19/defer-in-cpp/

// How does code work?
// call defer macro to evaluate our code as a defer statement.
// code into defer macro can be an exp or a statement
// defer statement is defining a var (of type privDefer) with some unique autogen name
// code gets executed when var goes out of scope because code is slotted to execute
//    in destructor of privDefer type.

template <typename F>
struct privDefer {
	F f;
	privDefer(F f) : f(f) {}
	~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
	return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define defer(code)   auto DEFER_3(_defer_) = defer_func([&](){code;})