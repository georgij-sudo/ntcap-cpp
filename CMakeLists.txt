cmake_minimum_required(VERSION 3.20)
project(my_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Собирать динамическую библиотеку" OFF)
option(BUILD_TESTS "Собирать тесты" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" ON)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g --no-inline)
        add_link_options(--coverage)
    else()
        message(FATAL_ERROR "Coverage requires GCC or Clang")
    endif()
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(lib)
add_subdirectory(gui)
