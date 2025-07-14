cmake_minimum_required(VERSION 3.28)

project(vEDR_DLL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
        logger/logger.cpp
        logger/logger.h)
target_include_directories(${PROJECT_NAME} PRIVATE logger)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_include_directories(${PROJECT_NAME} PRIVATE hooks)

add_subdirectory(lib/minhook)

target_link_libraries(${PROJECT_NAME} PRIVATE minhook)
target_include_directories(${PROJECT_NAME} PRIVATE lib/minhook/include)