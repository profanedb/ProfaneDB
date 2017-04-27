cmake_minimum_required(VERSION 2.6)
project(ProdisDB)
set(ProdisDB_VERSION_MAJOR 0)
set(ProdisDB_VERSION_MINOR 1)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(ProdisDB
    src/prodisdb/server/main.cpp
    src/prodisdb/server/prodis_error_collector.cpp
    src/prodisdb/protobuf/options.pb.cc)
target_link_libraries(ProdisDB protobuf)
