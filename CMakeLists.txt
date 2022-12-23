cmake_minimum_required(VERSION 3.16.3)

# set the project name
project(BlockTreesExperiments)
set (CMAKE_CXX_STANDARD 20)
# add the executable
#add_executable(${PROJECT_NAME} main.cpp)
#add_executable(CompBench compbench.cpp)
#add_executable(fingerprinttest fptest.cpp)
#add_executable(template templatetest.cpp)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fopenmp -ldl -lm -lstdc++")
#-fsanitize=address
add_subdirectory(external/tlx)
add_subdirectory(external/BlockTreesCmpr)
add_subdirectory(external/block_tree)
add_subdirectory(external/wavelet_tree)

include_directories(external/BlockTreesCmpr/includes)
include_directories(external/BlockTreesCmprexternal/sdsl-lite/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/malloc_count)

set(benches
        main
        compbench
        calclz
        accessonly
        parallelbench
        )
foreach(bench ${benches})
    add_executable(${bench} ${bench}.cpp)
    target_link_directories(${bench}
            PRIVATE external/BlockTreesCmpr/external/sdsl-lite
            PRIVATE external/tlx
            PRIVATE external/BlockTreesCmpr/src
            INTERFACE external/wavelet_tree/
            )

    target_include_directories(${bench}
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
    target_link_libraries(${bench} pasta_wavelet_tree)
    target_link_libraries(${bench} sais)
    target_link_libraries(${bench} sais64)
    target_link_libraries(${bench} pasta_bit_vector)
    target_link_libraries(${bench} tlx blocktree)
    target_link_libraries(${bench} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libsdsl.a )
    target_link_libraries(${bench} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libdivsufsort.a )
    target_link_libraries(${bench} ${CMAKE_SOURCE_DIR}/external/BlockTreesCmpr/external/sdsl-lite/lib/libdivsufsort64.a )
    #target_link_libraries(${bench} malloc_count)
    #    target_link_libraries(${bench} ${CMAKE_SOURCE_DIR}/extlib/malloc_count/malloc_count.o)
    #target_include_directories(${bench} PUBLIC ${CMAKE_SOURCE_DIR}/external/malloc_count)
endforeach()

