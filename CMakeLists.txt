cmake_minimum_required(VERSION 3.22.1)

# set the project name
project(BlockTreesExperiments)
set (CMAKE_CXX_STANDARD 11)
# add the executable
add_executable(${PROJECT_NAME} main.cpp)
add_executable(fingerprinttest fptest.cpp)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ldl")
add_subdirectory(external/tlx)
add_subdirectory(naiveBlockTree)
add_subdirectory(external/BlockTreesCmpr)
include_directories(external/BlockTreesCmpr/includes)
include_directories(external/BlockTreesCmprexternal/sdsl-lite/include)
target_include_directories(${PROJECT_NAME}
    PUBLIC external/BlockTreesCmpr/include/
    PUBLIC external/BlockTreesCmpr/external/sdsl-lite/include
    PUBLIC external/malloc_count
)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/BlockTreesCmpr/external/sdsl-lite
    PRIVATE external/tlx 
    PRIVATE external/BlockTreesCmpr/src
    
)
target_link_libraries(${PROJECT_NAME} naive tlx blocktree)
target_link_libraries(
    ${PROJECT_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc_count.o
)
target_link_libraries( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libsdsl.a )
target_link_libraries( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libdivsufsort.a )
target_link_libraries( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libdivsufsort64.a )