add_subdirectory(protobuf) # Generated Protobuf (& gRPC if neeeded) code

add_subdirectory(format)
add_subdirectory(vault)

add_library(profanedb)
target_link_libraries(profanedb profanedb_format profanedb_vault profanedb_protobuf)

install(TARGETS profanedb LIBRARY DESTINATION lib)

if(BUILD_PROFANEDB_SERVER)
    add_subdirectory(server)
endif()
