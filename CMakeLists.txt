cmake_minimum_required(VERSION 3.16.3)

# set the project name
project(BlockTreesExperiments)
set (CMAKE_CXX_STANDARD 20)
# add the executable
add_executable(${PROJECT_NAME} main.cpp)
#add_executable(fingerprinttest fptest.cpp)
#add_executable(template templatetest.cpp)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fopenmp -ldl -lm -lstdc++")
add_subdirectory(external/tlx)
add_subdirectory(external/BlockTreesCmpr)
add_subdirectory(external/block_tree)
add_subdirectory(external/wavelet_tree)
include_directories(external/BlockTreesCmpr/includes)
include_directories(external/BlockTreesCmprexternal/sdsl-lite/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/malloc_count)
target_include_directories(${PROJECT_NAME}
    PUBLIC external/BlockTreesCmpr/include/
    PUBLIC external/BlockTreesCmpr/external/sdsl-lite/include
    PUBLIC external/tlx/tlx
    PUBLIC ${CMAKE_SOURCE_DIR}/external/wavelet_tree/include
    PUBLIC ${CMAKE_SOURCE_DIR}/external/block_tree/includes/utils
    PUBLIC ${CMAKE_SOURCE_DIR}/external/block_tree/includes/bv_block_tree
    PUBLIC ${CMAKE_SOURCE_DIR}/external/block_tree/extlib/bit_vector/include/pasta/bit_vector
    PUBLIC ${CMAKE_SOURCE_DIR}/external/block_tree/extlib/bit_vector/include/pasta/bit_vector/support
    PUBLIC ${CMAKE_SOURCE_DIR}/external/block_tree/extlib/libsais_cmake/src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/BlockTreesCmpr/external/sdsl-lite
    PRIVATE external/tlx 
    PRIVATE external/BlockTreesCmpr/src
    INTERFACE external/wavelet_tree/
        )
target_link_libraries(${PROJECT_NAME} pasta_wavelet_tree)
target_link_libraries(${PROJECT_NAME} sais)
target_link_libraries(${PROJECT_NAME} sais64)
target_link_libraries(${PROJECT_NAME} pasta_bit_vector)
target_link_libraries(${PROJECT_NAME} tlx blocktree)
target_link_libraries( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libsdsl.a )
target_link_libraries( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libdivsufsort.a )
target_link_libraries( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libdivsufsort64.a )
