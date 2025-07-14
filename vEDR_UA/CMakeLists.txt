cmake_minimum_required(VERSION 3.28)

project(vEDR_UA)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(lib/spdlog)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_include_directories(${PROJECT_NAME} PRIVATE lib/spdlog/include/spdlog)