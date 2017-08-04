cmake_minimum_required(VERSION 3.1)
project(ProfaneDB)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_STANDARD 14)

option(BUILD_SHARED_LIBS "Build libprofanedb.so, to embed ProfaneDB" ON)
option(BUILD_PROFANEDB_SERVER "Build ProfaneDB gRPC server" OFF)
option(BUILD_TESTS "Build ProfaneDB tests" OFF)

# Even tests will need to include file from here
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src)
# same with profanedb/protobuf objects
set(PROTOBUF_IMPORT_DIRS ${PROTOBUF_IMPORT_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()
