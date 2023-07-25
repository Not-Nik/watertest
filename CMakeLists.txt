cmake_minimum_required(VERSION 3.25)
project(watertest)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(raylib)

add_executable(watertest main.cpp)
target_link_libraries(watertest PUBLIC raylib)
