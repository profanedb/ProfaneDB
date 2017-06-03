find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED) # Protobuf does the serialisation for the DB

# Proto are no longer generated in build,
# but come from https://gitlab.com/ProfaneDB/protobuf
set(PROTO_CC db.pb.cc options.pb.cc storage.pb.cc)
set(PROTO_H db.pb.h options.pb.h storage.pb.h)

add_library(profanedb_protobuf STATIC ${PROTO_CC} ${PROTO_H} )
target_link_libraries(profanedb_protobuf ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES})

# If building the shared library, libprofanedb_protobuf.a needs to be PIC to be included
if(BUILD_SHARED_LIBS)
    set_target_properties(profanedb_protobuf PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# libprofanedb_grpc.a has the service definitions, it's not required by the shared lib
if(BUILD_PROFANEDB_SERVER)
    find_package(gRPC REQUIRED)
    
    set(GRPC_CC db.grpc.pb.cc)
    set(GRPC_H db.grpc.pb.h)

    add_library(profanedb_grpc STATIC ${GRPC_CC} ${GRPC_H})
    target_link_libraries(profanedb_grpc profanedb_protobuf ${GRPC_LIBRARIES})
endif()

