#pragma once

#include "eventloop.hpp"

#include "future_wrapper.hpp"

#include <experimental/coroutine>

// for void async functions
template <typename... Args>
struct std::experimental::coroutine_traits<future<void>, Args...> {
    struct promise_type {
        promise<void> p;

        auto get_return_object() { return p.get_future(); }
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void unhandled_exception(){};
        void set_exception(exception_ptr e) { p.set_exception(std::move(e)); }
        void return_void() { p.set_value(); }
    };
};

// for non-void async functions
template <typename R, typename... Args>
struct std::experimental::coroutine_traits<future<R>, Args...> {
    struct promise_type {
        promise<R> p;

        auto get_return_object() { return p.get_future(); }
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void set_exception(exception_ptr e) { p.set_exception(std::move(e)); }
        void unhandled_exception(){};
        template <typename U>
        void return_value(U&& u)
        {
            p.set_value(std::forward<U>(u));
        }
    };
};

// awaiter
template <typename R>
auto operator co_await(future<R>&& f)
{
    struct Awaiter {
        future<R>&& input;
        future<R> output;

        bool await_ready() { return false; /* never finish immediately */ }

        void await_suspend(std::experimental::coroutine_handle<> coro)
        {
            input.then([coro, this](future<R> f) {
                    this->input = std::move(f);
                EventLoop::get().post(std::move([coro, this]() mutable {
                    this->output = std::move(this->input);
                    coro.resume();
                }));
            });
        }

        auto await_resume() { return output.get(); }
    };

    return Awaiter{ std::move(static_cast<future<R>&&>(f)) };
}
