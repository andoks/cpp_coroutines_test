#include "client.hpp"

#include "coroutines_boilerplate.hpp"
#include "dummy_async_api.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

Client::Client(std::string name, int iterations)
    : m_name(name)
    , iterations(iterations)
{
}

Client::~Client() = default;

void Client::startAsync()
{
    mainLoopFuture = [this]() -> std::future<void> {
        for (int i = 0; i < iterations; i++) {
            co_await sleepAsync(5000ms);
            std::cout << "client " << m_name << ": iteration " << i
                      << " completed \n";
        }

        std::string copy = m_name;
        callAtExit(copy);
        co_return;
    }();
}

void Client::onDone(std::function<void(std::string)> slot)
{
    callAtExit = slot;
}

std::string Client::name() { return m_name; }
