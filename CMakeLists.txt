cmake_minimum_required(VERSION 3.12) 

set(PROJECT_NAME cjp)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(${PROJECT_NAME})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-g")

file(GLOB SRC_FILES "src/*.c")
include_directories("include/")

add_executable(${PROJECT_NAME} ${SRC_FILES})
