cmake_minimum_required(VERSION 3.10)

project(cpp_coroutines_test CXX)

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
