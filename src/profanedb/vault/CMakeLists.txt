find_package(RocksDB REQUIRED) # RocksDB is used as storage

add_library(profanedb_vault STATIC rocksdb/storage.cpp memory/storage.cpp)
target_link_libraries(profanedb_vault profanedb_protobuf ${ROCKSDB_LIBRARIES})

if(BUILD_SHARED_LIBS)
    set_target_properties(profanedb_vault PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
