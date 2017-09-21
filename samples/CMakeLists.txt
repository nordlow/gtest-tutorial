cmake_minimum_required(VERSION 3.5)

enable_testing()

add_executable(runUnitTests
  sample1_unittest.cpp
)

target_link_libraries(runUnitTests gtest gtest_main)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
