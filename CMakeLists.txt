cmake_minimum_required(VERSION 3.10)

include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.95.tar.gz"
    SHA1 "bba77397e6c5e08733e45c451e9ae868088d20e7"
)

project(cpp_coroutines_test CXX C)


find_package(Threads)

add_executable(main
    main.cpp
    eventloop.cpp
    dummy_async_api.cpp
    client.cpp
    server.cpp
)
target_link_libraries(main 
    ${CMAKE_THREAD_LIBS_INIT})
