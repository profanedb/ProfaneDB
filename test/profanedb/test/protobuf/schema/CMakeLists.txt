find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED) # Protobuf does the serialisation for the DB

# Protobuf messages should always be generated
file(GLOB Proto "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_CC PROTO_H ${Proto})

add_library(profanedb_test_protobuf_schema STATIC ${PROTO_CC} ${PROTO_H} )
target_link_libraries(profanedb_test_protobuf_schema ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES})
