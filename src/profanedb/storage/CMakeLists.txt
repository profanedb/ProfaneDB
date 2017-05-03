add_library(profanedb_storage db.cpp parser.cpp config.cpp)
target_link_libraries(profanedb_storage profanedb_protobuf ${ROCKSDB_LIBRARIES} ${Boost_LIBRARIES})

# libprofanedb_storage is used statically by the server,
# built as shared lib here to allow other software to embed it
if(BUILD_LIBPROFANEDB)
    add_library(profanedb SHARED db.cpp parser.cpp config.cpp)
    target_link_libraries(profanedb profanedb_protobuf ${ROCKSDB_LIBRARIES} ${Boost_LIBRARIES})
endif()
