cmake_minimum_required(VERSION 3.28)

project(SCLoader)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp)
