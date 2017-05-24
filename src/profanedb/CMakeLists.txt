add_subdirectory(protobuf) # Generated Protobuf (& gRPC if neeeded) code

add_subdirectory(boot)
add_subdirectory(vault)

add_library(profanedb db.cpp)
target_link_libraries(profanedb profanedb_protobuf)

# Nothing should depend on code in here, so it can be skipped entirely
if(BUILD_PROFANEDB_SERVER)
    add_subdirectory(server)
endif()
