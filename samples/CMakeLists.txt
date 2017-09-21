cmake_minimum_required(VERSION 3.5)

enable_testing()

find_package(GTest REQUIRED)

add_executable(runUnitTests
  sample1_unittest.cpp sample1.cpp
  stl_unittest.cpp
)

target_link_libraries(runUnitTests gtest gtest_main pthread)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
