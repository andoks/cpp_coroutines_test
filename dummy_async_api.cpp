#include "dummy_async_api.hpp"

#include "client.hpp"
#include "coroutines_boilerplate.hpp"
#include "future_wrapper.hpp"

#include <chrono>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;

future<void> sleepAsync(std::chrono::milliseconds t)
{
    promise<void> p;
    auto fut = p.get_future();
    std::thread worker([promise = std::move(p), t]() mutable {
        std::this_thread::sleep_for(t);
        promise.set_value();
    });

    worker.detach();

    return fut;
}

auto DummyAcceptor::acceptAsync() -> future<std::unique_ptr<Client>>
{
    co_await sleepAsync(10s);
    std::stringstream s;
    s << " #" << ++count;

    co_return std::make_unique<Client>(s.str(), count);
}
