find_package(Boost 1.15
             REQUIRED
             COMPONENTS random)
include_directories (${Boost_INCLUDE_DIRS})

add_library(profanedb_util randomgenerator.cpp)
target_link_libraries(profanedb_util ${Boost_LIBRARIES})
