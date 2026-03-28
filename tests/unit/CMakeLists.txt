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

# --- Создание тестового исполняемого файла ---
add_executable(unit_tests
    main.cpp
    table_rule.cpp
    torus.cpp
    # test_another.cpp     # добавляйте файлы по мере роста
)

target_link_libraries(unit_tests
    PRIVATE
        ntcap               # тестируемая библиотека
        GTest::gtest
        GTest::gmock        # если нужны моки
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
