cmake_minimum_required(VERSION 2.8)

project(model1)

add_subdirectory( libraries/singularity/ )

file(GLOB HEADERS "include/*.hpp")

set(SOURCE_EXE inputdataset.cpp base.cpp generator.cpp result_data_set.cpp input_data_set_handler.cpp)

if ( APPLE )
    set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -stdlib=libc++ -Wall " )
else ( APPLE )
    set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -Wall " )
endif ( APPLE )

include_directories( include )
include_directories( libraries/singularity/include/ )
include_directories( libraries/json/single_include/ )

add_executable( simulate_attack_1 programs/simulate_attack_1.cpp ${HEADERS} ${SOURCE_EXE} )
add_executable( input_data_set_handle programs/input_data_set_handle.cpp ${HEADERS} ${SOURCE_EXE} )
add_executable( stress_test programs/stress_test.cpp ${HEADERS} ${SOURCE_EXE} )
add_executable( test_1 programs/test_1.cpp ${HEADERS} ${SOURCE_EXE} )

target_link_libraries( simulate_attack_1 singularity boost_system)
target_link_libraries( input_data_set_handle singularity boost_system)
target_link_libraries( stress_test singularity boost_system)
target_link_libraries( test_1 singularity boost_system)
