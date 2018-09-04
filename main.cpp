

///
/// Based on https://blogs.msdn.microsoft.com/vcblog/2017/05/19/using-c-coroutines-with-boost-c-libraries/
///

#include <iostream>
    using std::cout;

#include <future>
    using std::future;
    using std::promise;

#include <chrono>
    using namespace std::chrono_literals;
#include <thread>

#include <experimental/coroutine>

#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>

class EventLoop {
private:
    std::queue<std::function<void()>> callbacks;
    std::recursive_mutex mut;
public:
    int run()
    {
        using namespace std::chrono_literals;

        while(true)
        {
            bool doSleep = false;
            {
                std::unique_lock<std::recursive_mutex> g(mut);
                if(callbacks.empty())
                {
                    doSleep = true;
                }
                else
                {
                    callbacks.front()();
                    callbacks.pop();
                }
            }
            if(doSleep)
            {
                // means that we will check for new events 1000 times per second
                // if queue is empty
                std::this_thread::sleep_for(1ms);
            }
        }
    }

    void post(std::function<void()> callback)
    {
        std::unique_lock<std::recursive_mutex> g(mut);
        callbacks.push(callback);
    }
};

static EventLoop ev;

// for void async functions
template <typename... Args>
struct std::experimental::coroutine_traits<future<void>, Args...> {
    struct promise_type {
        promise<void> p;

        auto get_return_object() {return p.get_future();}
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void unhandled_exception() {};
        void set_exception(exception_ptr e) { p.set_exception(std::move(e));}
        void return_void() { p.set_value();}
    };
};

// for non-void async functions
template <typename R, typename... Args>
struct std::experimental::coroutine_traits<future<R>, Args...> {
    struct promise_type {
        promise<R> p;

        auto get_return_object() {return p.get_future();}
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void set_exception(exception_ptr e) { p.set_exception(std::move(e));}
        void unhandled_exception() {};
        template <typename U>
        void return_value(U&& u) { p.set_value(std::forward<U>(u));}
    };
};

// awaiter
template <typename R> auto operator co_await(future<R>&& f)
{
    struct Awaiter {
        future<R>&& input;
        future<R> output;

        bool await_ready() { return false; /* never finish immediately */}
        auto await_resume() { return output.get(); }
        void await_suspend(std::experimental::coroutine_handle<> coro) {
            // apparently boost::future::then() is better, but this seems to work
            std::thread waiter([this, coro]() mutable {
                    input.wait();
                    this->output = std::move(input);
                    ev.post([coro]() mutable {
                            coro.resume();
                            });
                });
            waiter.detach();
        }
    };

    return Awaiter{static_cast<future<R>&&>(f)};
}


future<void> sleepAsync(std::chrono::milliseconds t)
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


class Client
{
    private:
        std::string m_name;
        int iterations;
        std::function<void(std::string)> callAtExit;
        std::future<void> mainLoopFuture;
    public:
        Client(std::string name, int iterations)
            : m_name(name)
            , iterations(iterations)
        {
        }
        void startAsync()
        {
            mainLoopFuture = [this]() -> std::future<void> {
                for(int i = 0; i < iterations; i++)
                {
                    co_await sleepAsync(5000ms);
                    cout << "client " << m_name << ": iteration " << i << " completed \n";
                }

                std::string copy = m_name;
                callAtExit(copy);
                co_return;
            }();
        }
        void onDone(std::function<void(std::string)> slot)
        {
           callAtExit  = slot;
        }
        std::string name()
        {
            return m_name;
        }
};

#include <sstream>
auto acceptAsync = [count = 0] () mutable -> future<std::unique_ptr<Client>>
{
    co_await sleepAsync(10s);
    std::stringstream s;
    s << " #" << ++count;

    co_return std::make_unique<Client>(s.str(), count);
};

#include <unordered_map>
class Server
{
    private:
        std::string m_name;
        std::unordered_map<std::string, std::unique_ptr<Client>> clients;
        std::future<void> mainLoopFuture;
    public:
        Server(std::string name)
            : m_name(name)
        {
        }
        void startAsync()
        {
            mainLoopFuture = [this]() mutable -> std::future<void> {
                while(true)
                {
                    auto newClient = co_await acceptAsync();
                    cout << m_name << ": client \"" << newClient->name() << "\" connected...\n";
                    newClient->onDone([this](std::string name){
                            auto it = clients.find(name);
                            if(it != clients.end())
                                cout << m_name << ": removing client \"" << it->first << "\"\n";
                                clients.erase(it);
                            });

                    auto ptr = newClient.get();
                    auto name = newClient->name();
                    clients.insert({name, std::move(newClient)});

                    // do this after everything else
                    ptr->startAsync();
                }

                co_return;
            }();
        }
};

int main(int argc, char* argv[])
{
    cout << "Hello World!\n";

    Server srv{"Server"};
    srv.startAsync();

    cout << "starting event loop\n";
    auto ret = ev.run();

    return ret;
}
