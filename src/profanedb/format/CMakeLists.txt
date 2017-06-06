add_definitions(-DBOOST_LOG_DYN_LINK) # This is for the Boost Log library to load the proper target namespace for the current platform
find_package(Boost REQUIRED COMPONENTS log filesystem) # Boost Filesystem scans the .proto directory

add_library(profanedb_format STATIC protobuf/marshaller.cpp protobuf/loader.cpp)
target_link_libraries(profanedb_format profanedb_protobuf ${Boost_LIBRARIES})

if(BUILD_SHARED_LIBS)
    set_target_properties(profanedb_format PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
