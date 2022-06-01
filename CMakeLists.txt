cmake_minimum_required(VERSION 3.22.1)

# set the project name
project(BlockTreesExperiments)
set (CMAKE_CXX_STANDARD 11)
# add the executable
add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(external/tlx)
add_subdirectory(naiveBlocktree)
add_subdirectory(external/BlockTreesCmpr)
#target_include_directories(${PROJECT_NAME} PUBILC external/tlx/tlx naiveBlocktree)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/tlx naiveBlocktree
)

target_link_libraries(${PROJECT_NAME} tlx naiveBlocktree)