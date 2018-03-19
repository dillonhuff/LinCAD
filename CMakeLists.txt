cmake_minimum_required(VERSION 2.6)

project(LinCAD)

SET(EXTRA_CXX_COMPILE_FLAGS "-std=c++11 -I./src -I./test -I/opt/local/include -O2 -Werror -Wall")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_COMPILE_FLAGS}")

SET(CXX_GMP_LINK_FLAGS "-lgmpxx -lgmp")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${CXX_GMP_LINK_FLAGS}")

INCLUDE_DIRECTORIES(./src/)

SET(LQE_CPPS src/rational.cpp
             src/context.cpp)

add_library(LinCAD ${LQE_CPPS})
target_link_libraries(LinCAD GMP)

SET(TEST_FILES ./test/test_context.cpp
               ./test/test_sat.cpp)

add_executable(all-tests ${TEST_FILES})
target_link_libraries(all-tests LinCAD)
