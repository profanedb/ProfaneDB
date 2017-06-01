include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Will still need profanedb/protobuf/options.proto, and also schema
set(PROTOBUF_IMPORT_DIRS ${PROTOBUF_IMPORT_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(schema)

add_definitions(-DBOOST_TEST_DYN_LINK) # This is for Boost Test to use the shared library
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

# file(GLOB Tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.test.cpp")
set(Tests marshaller.test.cpp)

foreach(Test ${Tests})
    get_filename_component(Name ${Test} NAME_WE)
    
    add_executable(${Name} ${Test})
    target_link_libraries(${Name} ${Boost_LIBRARIES} profanedb profanedb_test_protobuf_schema)
    
    add_test(NAME ${Name} COMMAND ${Name})
endforeach()
