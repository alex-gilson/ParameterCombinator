
cmake_minimum_required(VERSION 3.20)

project(ParameterCombinator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(ParameterCombinator "")

set(PARAMETER_COMBINATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(PARAMETER_COMBINATOR_SRC_DIR ${PARAMETER_COMBINATOR_DIR}/src/)
set(PARAMETER_COMBINATOR_TEST_DIR ${PARAMETER_COMBINATOR_DIR}/test/)

add_subdirectory(src)
add_subdirectory(test)