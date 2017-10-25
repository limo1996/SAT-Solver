cmake_minimum_required(VERSION 2.8.12.2)
project(sat_solver)
macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 11)
    endif ()
endmacro(use_cxx11)

use_cxx11()

set(SOURCE_FILES src/sequential_main.cpp src/cnfparser.cpp src/CNF.cpp src/clause.cpp src/dpll.cpp src/exceptions.cpp src/variable.cpp)
add_executable(sequential_main ${SOURCE_FILES})

set(SOURCE_FILES src/parallel_main.cpp src/cnfparser.cpp src/CNF.cpp src/clause.cpp src/dpll.cpp src/exceptions.cpp src/variable.cpp src/worker.cpp)
add_executable(parallel_main ${SOURCE_FILES})
