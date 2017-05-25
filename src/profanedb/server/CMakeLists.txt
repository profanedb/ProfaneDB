add_executable(profanedb_server main.cpp server.cpp)
target_link_libraries(profanedb_server profanedb profanedb_grpc)
