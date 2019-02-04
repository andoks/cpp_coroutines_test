#pragma once

#include "future_wrapper.hpp"

#include <chrono>
#include <memory>

class Client;

future<void> sleepAsync(std::chrono::milliseconds t);

class DummyAcceptor {
private:
    int count = 0;

public:
    auto acceptAsync() -> future<std::unique_ptr<Client>>;
};
