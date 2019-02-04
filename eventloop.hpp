#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

class EventLoop {
    enum class LoopState {
        CallbackPosted,
        Waiting,
    };

private:
    using QueueT = std::queue<std::function<void()>>;
    QueueT callbacks;
    std::mutex mut;
    std::condition_variable cv;
    LoopState state = LoopState::Waiting;

public:
    static EventLoop& get();

public:
    EventLoop();
    ~EventLoop();

    int run();
    void post(std::function<void()> callback);
};
