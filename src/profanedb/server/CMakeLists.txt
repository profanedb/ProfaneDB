find_package(gRPC REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)
add_definitions(-DBOOST_LOG_DYN_LINK) # This is for the Boost Log library to load the proper target namespace for the current platform

add_executable(profanedb_server main.cpp server.cpp)
target_link_libraries(profanedb_server ${Boost_LIBRARIES} profanedb_format profanedb_vault profanedb_grpc)
