
# initially, install clang-8
# sudo aptitude install clang-8

# you'll then get an error regarding missing experimental/coroutine header,
# install libc++ for version 8 + dev
# sudo aptitude install libc++-8-dev

# then you'll get linker error, cannot find libc++
# execute  
# sudo ln /usr/lib/llvm-8/lib/libc++abi.so.1 /usr/lib/llvm-8/lib/libc++abi.so
# and
# export LD_LIBRARY_PATH=/usr/lib/llvm-8/lib

clang++-8 -stdlib=libc++ --std=c++2a -fcoroutines-ts main.cpp -o main
