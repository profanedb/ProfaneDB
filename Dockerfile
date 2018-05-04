# RocksDB is the first dependency
FROM buildpack-deps AS rocksdb

ENV INSTALL_PATH /opt/rocksdb
RUN apt-get update -y &&                                                \
    apt-get install -y                                                  \
        libbz2-dev                                                      \
        libgflags-dev                                                   \
        liblz4-dev                                                      \
        libsnappy-dev                                                   \
        # libzstd-dev   Doesn't work during linking for some reason...
        zlib1g-dev &&                                                   \
    wget https://github.com/facebook/rocksdb/archive/v5.12.4.tar.gz &&  \
    tar xzvf v5.12.4.tar.gz &&                                          \
    make -C rocksdb-5.12.4 -j$(nproc) shared_lib &&                     \
    make -C rocksdb-5.12.4 install

# gRPC installs Protobuf too
FROM buildpack-deps AS grpc

ENV prefix /opt/grpc
RUN git clone -b v1.11.x https://github.com/grpc/grpc &&    \
    cd grpc &&                                              \
    git submodule update --init &&                          \
    make -j$(nproc) &&                                      \
    make install &&                                         \
    make -C third_party/protobuf install prefix=$prefix

# This last step builds ProfaneDB
FROM buildpack-deps AS builder

COPY --from=rocksdb /opt/rocksdb/   /usr/local
COPY --from=grpc    /opt/grpc/      /usr/local

ADD . /profanedb

RUN apt-get update -y &&                \
    apt-get install -y                  \
        cmake                           \
        libboost-dev                    \
        libboost-filesystem-dev         \
        libboost-log-dev                \
        libboost-program-options-dev    \
        libboost-random-dev             \
        libboost-test-dev               \
        libbz2-dev                      \
        libgflags-dev                   \
        liblz4-dev                      \
        libsnappy-dev                   \
        zlib1g-dev &&                   \
    mkdir /profanedb/build &&           \
    cd /profanedb/build &&              \
    cmake                               \
        -D BUILD_PROFANEDB_SERVER=ON    \
        -D BUILD_TESTS=ON               \
        .. &&                           \
    make -j$(nproc) &&                  \
    make install

VOLUME [ "/var/profanedb/schema" ]

CMD [ "profanedb_server", "-c /usr/local/etc/profanedb/server.conf" ]