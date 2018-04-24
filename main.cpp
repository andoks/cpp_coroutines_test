#include <iostream>
    using std::cout;

#include <future>
    using std::future;

#include <experimental/coroutine>

struct Result {
    struct promise_type {
        auto get_return_object() { return Result{}; }
        auto return_value(int ignore) { return std::experimental::suspend_always{}; }
        auto final_suspend() { return std::experimental::suspend_always{}; }
        auto initial_suspend() { return std::experimental::suspend_always{}; }
        void unhandled_exception() {}
    };
};

Result async_add(int a, int b)
{
    co_return a+b;
}

int main(int argc, char* argv[])
{
    cout << "Hello World!";
}
