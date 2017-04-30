file(GLOB Proto "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_CC PROTO_H ${Proto})
PROTOBUF_GENERATE_GRPC_CPP(GRPC_CC GRPC_H ${Proto})

add_library(profanedb_protobuf ${PROTO_CC} ${PROTO_H})
add_library(profanedb_grpc ${GRPC_CC} ${GRPC_H})

target_link_libraries(profanedb_protobuf ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES})
target_link_libraries(profanedb_grpc profanedb_protobuf ${GRPC_LIBRARIES})
