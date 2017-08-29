cmake_minimum_required(VERSION 3.5)
project("useless-continuation")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Wall -Wpedantic -Werror)

set(CMAKE_ORIGINAL_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

enable_testing()

set(CMAKE_BINARY_DIR ${CMAKE_ORIGINAL_BINARY_DIR}/testbin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)

include(GTest)
add_subdirectory(test)
