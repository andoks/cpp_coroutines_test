#include "dummy_async_api.hpp"

#include "coroutines_boilerplate.hpp"
#include "client.hpp"

#include <sstream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

std::future<void> sleepAsync(std::chrono::milliseconds t)
{
    std::promise<void> p;
    auto fut = p.get_future();
    std::thread worker ([promise = std::move(p), t]() mutable {
        std::this_thread::sleep_for(t);
        promise.set_value();
    });

    worker.detach();

    return fut;
}

auto DummyAcceptor::acceptAsync() -> std::future<std::unique_ptr<Client>>
{
    co_await sleepAsync(10s);
    std::stringstream s;
    s << " #" << ++count;

    co_return std::make_unique<Client>(s.str(), count);
}

