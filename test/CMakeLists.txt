add_library(
    gtest_main
    gtest_main.cpp
)

target_link_libraries(
    gtest_main
    libgtest
)

add_executable(
    test_future
    test_future.cpp
)

target_link_libraries(
    test_future
    future
    gtest_main
)

add_test(
    test_future
    ${EXECUTABLE_OUTPUT_PATH}/test_future
)
