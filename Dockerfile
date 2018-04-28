FROM buildpack-deps AS rocksdb

ENV INSTALL_PATH /opt/rocksdb
RUN wget https://github.com/facebook/rocksdb/archive/v5.12.4.tar.gz &&  \
    tar xzvf v5.12.4.tar.gz &&                                          \
    make -C rocksdb-5.12.4 -j$(nproc) static_lib &&                     \
    make -C rocksdb-5.12.4 install


# USER profanedb
