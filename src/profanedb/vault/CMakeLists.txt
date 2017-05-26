add_library(profanedb_vault rocksdb/storage.cpp)
target_link_libraries(profanedb_vault profanedb_protobuf ${ROCKSDB_LIBRARIES})
