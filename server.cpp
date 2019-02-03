#include "server.hpp"

#include "coroutines_boilerplate.hpp"
#include "client.hpp"

#include <iostream>

Server::Server(std::string name)
    : m_name(name)
      , acceptor()
{
}

Server::~Server() = default;

void Server::startAsync()
{
    mainLoopFuture = [this]() mutable -> std::future<void> {
        while(true)
        {
            auto newClient = co_await acceptor.acceptAsync();
            std::cout << m_name << ": client \"" << newClient->name() << "\" connected...\n";
            newClient->onDone([this](std::string name){
                    auto it = clients.find(name);
                    if(it != clients.end())
                        std:: cout << m_name << ": removing client \"" << it->first << "\"\n";
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
