

///
/// Based on https://blogs.msdn.microsoft.com/vcblog/2017/05/19/using-c-coroutines-with-boost-c-libraries/
///

#include <iostream>
    using std::cout;

#include <future>
    using std::future;
    using std::promise;

#include <experimental/coroutine>

// for void async functions
template <typename... Args>
struct std::experimental::coroutine_traits<future<void>, Args...> {
    struct promise_type {
        promise<void> p;

        auto get_return_object() {return p.get_future();}
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void unhandled_exception() {};
        void set_exception(exception_ptr e) { p.set_exception(std::move(e));}
        void return_void() { p.set_value();}
    };
};

// for non-void async functions
template <typename R, typename... Args>
struct std::experimental::coroutine_traits<future<R>, Args...> {
    struct promise_type {
        promise<R> p;

        auto get_return_object() {return p.get_future();}
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void set_exception(exception_ptr e) { p.set_exception(std::move(e));}
        void unhandled_exception() {};
        template <typename U>
        void return_value(U&& u) { p.set_value(std::forward<U>(u));}
    };
};


future<int> async_add(int a, int b)
{
    co_return a+b;
}

future<void> async_hi()
{
    cout << "my first coroutine asynchronously returned: " << async_add(20, 22).get() << '\n';
    co_return;
}

int main(int argc, char* argv[])
{
    cout << "Hello World!\n";

    async_hi().get();
}
