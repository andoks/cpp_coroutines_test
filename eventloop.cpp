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
        // wait for new callbacks if queue is empty to avoid busylooping
        std::unique_lock<std::mutex> lock(mut);
        if (callbacks.empty()) {
            state = LoopState::Waiting;
            cv.wait(lock, [&]() { return state == LoopState::CallbackPosted; });
        }

        // take all callbacks to run
        QueueT toBeRun;
        std::swap(toBeRun, callbacks);
        // unlock mutex so that callbacks run in loop may push to callback-queue
        lock.unlock();

        // drain callback queue
        while (!toBeRun.empty()) {
            toBeRun.front()();
            toBeRun.pop();
        }
    }

    std::unique_lock<std::mutex> lock(mut);
    return callbacks.size();
}

void EventLoop::post(std::function<void()> callback)
{
    std::unique_lock<std::mutex> lock(mut);
    state = LoopState::CallbackPosted;
    callbacks.push(callback);
    lock.unlock();

    // wake up run()
    cv.notify_one();
}
