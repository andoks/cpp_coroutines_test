#pragma once

#include "eventloop.hpp"

#include <experimental/coroutine>
#include <future>

// for void async functions
template <typename... Args>
struct std::experimental::coroutine_traits<std::future<void>, Args...> {
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
struct std::experimental::coroutine_traits<std::future<R>, Args...> {
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
template <typename R> auto operator co_await(std::future<R>&& f)
{
    struct Awaiter {
        std::future<R>&& input;
        std::future<R> output;

        bool await_ready() { return false; /* never finish immediately */}
        auto await_resume() { return output.get(); }
        void await_suspend(std::experimental::coroutine_handle<> coro) {
            // apparently boost::future::then() is better, but this seems to work
            std::thread waiter([this, coro]() mutable {
                    input.wait();
                    this->output = std::move(input);
                    EventLoop::get().post([coro]() mutable {
                            coro.resume();
                            });
                });
            waiter.detach();
        }
    };

    return Awaiter{static_cast<std::future<R>&&>(f)};
}
