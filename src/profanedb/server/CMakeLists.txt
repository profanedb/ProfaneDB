add_executable(profanedb_server main.cpp server.cpp)


if(BUILD_LIBPROFANEDB)
    target_link_libraries(profanedb_server profanedb profanedb_grpc)
elseif()
    # profanedb_grpc contains profanedb_protobuf
    # profanedb_storage is linked statically for this executable
    target_link_libraries(profanedb_server profanedb_grpc profanedb_storage)
endif()
