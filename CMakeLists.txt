cmake_minimum_required(VERSION 3.0)
project(ProfaneDB)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(PROTOBUF_IMPORT_DIRS "${CMAKE_SOURCE_DIR}/src")

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src/profanedb/protobuf)
add_subdirectory(src/profanedb/storage)
add_subdirectory(src/profanedb/server)
