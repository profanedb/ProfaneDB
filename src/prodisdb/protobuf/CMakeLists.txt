file(GLOB Proto "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_CC PROTO_H ${Proto})
PROTOBUF_GENERATE_GRPC_CPP(GRPC_CC GRPC_H ${Proto})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)

add_library(prodisdb_protobuf ${PROTO_CC} ${PROTO_H} ${GRPC_CC} ${GRPC_H})
target_link_libraries(prodisdb_protobuf ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})
