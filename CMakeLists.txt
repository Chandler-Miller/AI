cmake_minimum_required(VERSION 3.10)
project(ai)

set(CMAKE_CXX_STANDARD 11)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} Include)


set(SOURCES
    Source/main.cpp
    Source/node.cpp
)

set(TEST_SOURCES
    Tests/node_test.cpp
)

set (HEADERS
    Include/node.h
)

add_executable(ai ${SOURCES} ${HEADERS})

add_executable(node_tests ${TEST_SOURCES} ${HEADERS})

target_link_libraries(ai pthread ${GTEST_LIBRARIES})

target_link_libraries(node_tests pthread ${GTEST_LIBRARIES})