

///
/// Based on https://blogs.msdn.microsoft.com/vcblog/2017/05/19/using-c-coroutines-with-boost-c-libraries/
///

#include <iostream>
    using std::cout;

#include <future>
    using std::future;
    using std::promise;

#include <chrono>
    using namespace std::chrono_literals;
#include <thread>

#include <experimental/coroutine>

#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>

class EventLoop {
private:
    std::queue<std::function<void()>> callbacks;
    std::recursive_mutex mut;
public:
    int run()
    {
        using namespace std::chrono_literals;

        while(true)
        {
            bool doSleep = false;
            {
                std::unique_lock<std::recursive_mutex> g(mut);
                if(callbacks.empty())
                {
                    doSleep = true;
                }
                else
                {
                    callbacks.front()();
                    callbacks.pop();
                }
            }
            if(doSleep)
            {
                // means that we will check for new events 1000 times per second
                // if queue is empty
                std::this_thread::sleep_for(1ms);
            }
        }
    }

    void post(std::function<void()> callback)
    {
        std::unique_lock<std::recursive_mutex> g(mut);
        callbacks.push(callback);
    }
};

static EventLoop ev;

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

// awaiter
template <typename R> auto operator co_await(future<R>&& f)
{
    struct Awaiter {
        future<R>&& input;
        future<R> output;

        bool await_ready() { return false; /* never finish immediately */}
        auto await_resume() { return output.get(); }
        void await_suspend(std::experimental::coroutine_handle<> coro) {
            // apparently boost::future::then() is better, but this seems to work
            std::thread waiter([this, coro]() mutable {
                    input.wait();
                    this->output = std::move(input);
                    ev.post([coro]() mutable {
                            coro.resume();
                            });
                });
            waiter.detach();
        }
    };

    return Awaiter{static_cast<future<R>&&>(f)};
}


future<void> async_sleep(std::chrono::milliseconds t)
{
    std::promise<void> p;
    auto fut = p.get_future();
    std::thread worker ([promise = std::move(p), t]() mutable {
        std::this_thread::sleep_for(t);
        promise.set_value();
    });

    worker.detach();

    return fut;
}

future<int> async_add(int a, int b)
{
    co_return a+b;
}

future<void> async_hi()
{
    cout << "my first coroutine asynchronously returned: " << co_await async_add(20, 22) << '\n';
    co_return;
}

future<void> async_loop()
{
    int count = 0;
    while(true)
    {
        cout << "tick " << ++count << "\n";
        co_await async_sleep(5000ms);
    }

    co_return;
}

int main(int argc, char* argv[])
{
    cout << "Hello World!\n";

    async_hi();
    
    cout << "creating first loop\n";
    future<void> fut = async_loop();

    cout << "creating second loop\n";
    auto fut2 = async_loop();

    cout << "starting event loop\n";
    auto ret = ev.run();

    return ret;
}
