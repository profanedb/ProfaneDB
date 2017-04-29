cmake_minimum_required(VERSION 3.0)
project(ProdisDB)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(PROTOBUF_IMPORT_DIRS "${CMAKE_SOURCE_DIR}/src")

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

add_subdirectory(src/prodisdb/protobuf)
add_subdirectory(src/prodisdb/server)
