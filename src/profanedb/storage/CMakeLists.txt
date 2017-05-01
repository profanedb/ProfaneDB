add_library(profanedb_storage db.cpp parser.cpp)
target_link_libraries(profanedb_storage profanedb_protobuf ${ROCKSDB_LIBRARIES})
