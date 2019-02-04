# clang++-libc++ is a wrapper to clang++(8) but using libc++ instead of libstdc++
set(CMAKE_CXX_COMPILER /usr/bin/clang++-libc++)
set(CMAKE_C_COMPILER /usr/bin/clang-8)

# enable warnings as errors and more warnings to catch errors while compiling
# enable prelimenary c++20 support and coroutines-ts to be able to use coroutines
# force them into cache due to cmake hoobligook: https://stackoverflow.com/a/11536633
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Werror -Wall --std=c++2a -fcoroutines-ts" CACHE STRING "" FORCE)

