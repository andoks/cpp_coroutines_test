FROM ubuntu:bionic-20190122

RUN apt-get update          \
    && apt-get install -y   \
    apt-utils               \
    gnupg2                  \
    wget
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -
COPY llvm.list /etc/apt/sources.list.d/llvm.list

RUN apt-get update          \
    && apt-get install -y   \
        clang-8             \
        libc++-8-dev        \
        libc++-helpers      \
        vim                 \
        cmake               \
        ninja-build         \
        git

# ensure clang tools work
RUN update-alternatives --install /usr/lib/llvm-8/lib/libc++abi.so libc++abi /usr/lib/llvm-8/lib/libc++abi.so.1  1
RUN update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-8/bin/clang++ 1

WORKDIR /workdir
COPY ./ src
WORKDIR src
ENV LD_LIBRARY_PATH=/usr/lib/llvm-8/lib
RUN ["./build.bash"]
CMD ["./build.dir/main"]
