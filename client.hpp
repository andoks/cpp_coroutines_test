#pragma once

#include "future_wrapper.hpp"

#include <functional>
#include <string>

class Client {
private:
    std::string m_name;
    int iterations;
    std::function<void(std::string)> callAtExit;
    future<void> mainLoopFuture;

public:
    Client(std::string name, int iterations);
    ~Client();
    void startAsync();
    void onDone(std::function<void(std::string)> slot);
    std::string name();
};
