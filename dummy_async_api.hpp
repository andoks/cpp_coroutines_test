#pragma once

#include <chrono>
#include <future>
#include <memory>

class Client;

std::future<void> sleepAsync(std::chrono::milliseconds t);

class DummyAcceptor {
private:
    int count = 0;

public:
    auto acceptAsync() -> std::future<std::unique_ptr<Client>>;
};
