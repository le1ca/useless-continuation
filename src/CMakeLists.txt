set(lib_name "future")

add_library(
    ${lib_name}
    SHARED
    future.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
