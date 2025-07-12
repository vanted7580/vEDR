cmake_minimum_required(VERSION 3.28)

project(vEDR_DLL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(vEDR_DLL SHARED main.cpp)

set_target_properties(vEDR_DLL PROPERTIES PREFIX "")