include_directories(${CMAKE_CURRENT_BINARY_DIR}/../.. ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(prodisdb_server main.cpp prodis_error_collector.cpp prodis_server.cpp)
target_link_libraries(prodisdb_server prodisdb_protobuf)
