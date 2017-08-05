find_package(gRPC REQUIRED)

add_definitions(-DBOOST_LOG_DYN_LINK) # This is for the Boost Log library to load the proper target namespace for the current platform
find_package(Boost REQUIRED COMPONENTS log filesystem program_options)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(profanedb_server main.cpp server.cpp config.cpp)
target_link_libraries(profanedb_server ${Boost_LIBRARIES} profanedb_format profanedb_vault profanedb_grpc)
