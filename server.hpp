#pragma once

#include "dummy_async_api.hpp"

#include <unordered_map>
#include <string>
#include <future>
#include <memory>

class Server
{
    private:
        std::string m_name;
        std::unordered_map<std::string, std::unique_ptr<Client>> clients;
        std::future<void> mainLoopFuture;
        DummyAcceptor acceptor;

    public:
        Server(std::string name);
        ~Server();
        void startAsync();
};
