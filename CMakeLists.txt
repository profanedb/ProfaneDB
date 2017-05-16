cmake_minimum_required(VERSION 3.1)
project(ProfaneDB)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_STANDARD 14)
set(PROTOBUF_IMPORT_DIRS "${CMAKE_SOURCE_DIR}/src")

option(BUILD_SHARED_LIBS "Build libprofanedb.so, to embed ProfaneDB" ON)
option(BUILD_PROFANEDB_SERVER "Build ProfaneDB gRPC server" OFF)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED) # Protobuf does the serialisation for the DB
find_package(RocksDB REQUIRED) # RocksDB is used as storage
find_package(Boost REQUIRED COMPONENTS filesystem) # Boost Filesystem scans the .proto directory

# gRPC is only required by ProfaneDB server
if(BUILD_PROFANEDB_SERVER)
    find_package(gRPC REQUIRED)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src/profanedb/protobuf) # Generated Protobuf (& gRPC if neeeded) code
add_subdirectory(src/profanedb/storage) # libprofanedb shared lib, and common interface

# Nothing should depend on code in here, so it can be skipped entirely
if(BUILD_PROFANEDB_SERVER)
    add_subdirectory(src/profanedb/server)
endif()
