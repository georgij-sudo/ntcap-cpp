include(FetchContent)

# Скачиваем GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Для Windows: предотвращаем переопределение настроек runtime
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Включаем модуль GoogleTest для автообнаружения тестов
include(GoogleTest)

add_executable(unit_tests
    ${CMAKE_SOURCE_DIR}/core/Planes.cpp
    ${CMAKE_SOURCE_DIR}/recipes/infra/TorusTopology.cpp
    ${CMAKE_SOURCE_DIR}/recipes/infra/MooreN.cpp
    ${CMAKE_SOURCE_DIR}/recipes/infra/VonNeumannN.cpp
    ${CMAKE_SOURCE_DIR}/converters/PlanesCsvConverter.cpp
    ${CMAKE_SOURCE_DIR}/converters/PlanesRleConverter.cpp

    main.cpp
    planes.cpp
    torus.cpp
    moore.cpp
    von_neumann.cpp
    planes_csv.cpp
    planes_rle.cpp
    table_rule.cpp
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/recipes/infra
    ${CMAKE_SOURCE_DIR}/converters
)

target_link_libraries(unit_tests
    PRIVATE
        GTest::gtest
        GTest::gmock
)

target_compile_features(unit_tests PRIVATE cxx_std_20)

# Автоматическая регистрация тестов в CTest
gtest_discover_tests(unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    PROPERTIES
        LABELS "unit"
)

add_custom_command(
    TARGET unit_tests
    POST_BUILD
    COMMAND unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running unit tests..."
)
