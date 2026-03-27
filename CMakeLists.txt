cmake_minimum_required(VERSION 3.20)
project(my_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Собирать динамическую библиотеку" OFF)
option(BUILD_TESTS "Собирать тесты" ON)

add_subdirectory(lib)
add_subdirectory(gui)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

