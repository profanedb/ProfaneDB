add_library(profanedb_storage db.cpp parser.cpp loader.cpp normalizer.cpp storage.cpp rocks.cpp)
target_link_libraries(profanedb_storage profanedb_protobuf ${ROCKSDB_LIBRARIES} ${Boost_LIBRARIES})
