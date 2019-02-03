FROM ubuntu:artful-20180412

RUN apt-get update          \
    && apt-get install -y   \
    wget
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -
COPY llvm.list /etc/apt/sources.list.d/llvm.list

RUN apt-get update          \
    && apt-get install -y   \
        clang-8             \
        libc++-8-dev        \
        vim                 \
        git
RUN ln /usr/lib/llvm-8/lib/libc++abi.so.1 /usr/lib/llvm-8/lib/libc++abi.so
WORKDIR /workdir
COPY ./ src
WORKDIR src
ENV LD_LIBRARY_PATH=/usr/lib/llvm-8/lib
RUN ["./build.bash"]
CMD ["./main"]
