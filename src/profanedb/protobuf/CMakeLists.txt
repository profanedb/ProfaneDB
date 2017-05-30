find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED) # Protobuf does the serialisation for the DB

# Protobuf messages should always be generated
file(GLOB Proto "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_CC PROTO_H ${Proto})

add_library(profanedb_protobuf STATIC ${PROTO_CC} ${PROTO_H} )
target_link_libraries(profanedb_protobuf ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES})

# If building the shared library, libprofanedb_protobuf.a needs to be PIC to be included
if(BUILD_SHARED_LIBS)
    set_target_properties(profanedb_protobuf PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# libprofanedb_grpc.a has the service definitions, it's not required by the shared lib
if(BUILD_PROFANEDB_SERVER)
    find_package(gRPC REQUIRED)

    PROTOBUF_GENERATE_GRPC_CPP(GRPC_CC GRPC_H ${Proto})
    add_library(profanedb_grpc STATIC ${GRPC_CC} ${GRPC_H})
    target_link_libraries(profanedb_grpc profanedb_protobuf ${GRPC_LIBRARIES})
endif()

