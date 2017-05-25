add_library(profanedb_format protobuf/marshaller.cpp protobuf/loader.cpp)
target_link_libraries(profanedb_format profanedb_protobuf ${Boost_LIBRARIES})
