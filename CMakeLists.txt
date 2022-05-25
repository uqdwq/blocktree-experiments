cmake_minimum_required(VERSION 3.22.1)

# set the project name
project(BlockTreesExperiments)

# add the executable
add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(external/tlx)

#target_include_directories(${PROJECT_NAME} PUBILC external/tlx/tlx)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/tlx
)

target_link_libraries(${PROJECT_NAME} tlx)