#pragma once

#include <functional>
#include <mutex>
#include <queue>

class EventLoop {
private:
  std::queue<std::function<void()>> callbacks;
  std::recursive_mutex mut;

public:
  static EventLoop& get();

public:
  EventLoop();
  ~EventLoop();

  int run();
  void post(std::function<void()> callback);
};
