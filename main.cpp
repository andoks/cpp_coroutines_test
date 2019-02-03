///
/// Based on https://blogs.msdn.microsoft.com/vcblog/2017/05/19/using-c-coroutines-with-boost-c-libraries/
///

#include "eventloop.hpp"
#include "server.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    Server srv{"Server"};
    srv.startAsync();

    std::cout << "starting event loop\n";
    auto ret = EventLoop::get().run();

    return ret;
}
