add_library(profanedb_storage parser.cpp db.cpp)
target_link_libraries(profanedb_storage profanedb_protobuf ${ROCKSDB_LIBRARIES})
