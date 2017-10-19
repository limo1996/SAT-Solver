cmake_minimum_required(VERSION 3.5.1)
project(algolab)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/cnfparser.cpp src/CNF.cpp src/clause.cpp src/dpll.cpp src/exceptions.cpp src/variable.cpp)
add_executable(main ${SOURCE_FILES})
