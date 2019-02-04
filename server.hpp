#pragma once

#include "dummy_async_api.hpp"

#include "future_wrapper.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class Server {
private:
    std::string m_name;
    std::unordered_map<std::string, std::unique_ptr<Client>> clients;
    future<void> mainLoopFuture;
    DummyAcceptor acceptor;

public:
    Server(std::string name);
    ~Server();
    void startAsync();
};
