#include "eventloop.hpp"

EventLoop& EventLoop::get()
{
    static EventLoop ev;

    return ev;
}

EventLoop::EventLoop() = default;
EventLoop::~EventLoop() = default;

int EventLoop::run()
{
    while (true) {
        std::unique_lock<std::recursive_mutex> g(mut);
        if (!callbacks.empty()) {
            callbacks.front()();
            callbacks.pop();
        }
    }
}

void EventLoop::post(std::function<void()> callback)
{
    std::unique_lock<std::recursive_mutex> g(mut);
    callbacks.push(callback);
}
