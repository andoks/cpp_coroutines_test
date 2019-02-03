#pragma once

#include <string>
#include <functional>
#include <future>


class Client
{
    private:
        std::string m_name;
        int iterations;
        std::function<void(std::string)> callAtExit;
        std::future<void> mainLoopFuture;
    public:
        Client(std::string name, int iterations);
        ~Client();
        void startAsync();
        void onDone(std::function<void(std::string)> slot);
        std::string name();
};

