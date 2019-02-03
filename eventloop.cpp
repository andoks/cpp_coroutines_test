#include "eventloop.hpp"

#include <chrono>
#include <thread>

EventLoop& EventLoop::get() {
    static EventLoop ev;

    return ev;
}

EventLoop::EventLoop() = default;
EventLoop::~EventLoop() = default;

int EventLoop::run() {
  using namespace std::chrono_literals;

  while (true) {
    bool doSleep = false;
    {
      std::unique_lock<std::recursive_mutex> g(mut);
      if (callbacks.empty()) {
        doSleep = true;
      } else {
        callbacks.front()();
        callbacks.pop();
      }
    }
    if (doSleep) {
      // means that we will check for new events 1000 times per second
      // if queue is empty
      std::this_thread::sleep_for(1ms);
    }
  }
}

void EventLoop::post(std::function<void()> callback) {
  std::unique_lock<std::recursive_mutex> g(mut);
  callbacks.push(callback);
}
