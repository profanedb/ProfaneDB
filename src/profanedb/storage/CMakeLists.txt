add_library(profanedb_storage db.cpp parser.cpp config.cpp normalizer.cpp rootsourcetree.cpp rocks.cpp)
target_link_libraries(profanedb_storage profanedb_protobuf ${ROCKSDB_LIBRARIES} ${Boost_LIBRARIES})
