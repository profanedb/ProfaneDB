add_subdirectory(protobuf) # Generated Protobuf (& gRPC if neeeded) code

add_subdirectory(format)
add_subdirectory(vault)

# TODO Maybe we could build for google::protobuf::Message somewhere
add_library(profanedb)
target_link_libraries(profanedb profanedb_format profanedb_vault profanedb_protobuf)

if(BUILD_PROFANEDB_SERVER)
    add_subdirectory(server)
endif()
