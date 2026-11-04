cmake_minimum_required(VERSION 3.20)
project(my_project VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
# ── generated version header ─────────────────────────
configure_file(
    core/Version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Version.h
    @ONLY
)
option(BUILD_TESTS "Собирать тесты" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
# ── ntcap-gui executable ─────────────────────────────
add_executable(ntcap-gui
    # core
    core/main.cpp
    core/Planes.cpp
    core/Automaton.cpp
    core/AutomatonController.cpp
    core/PlaneInitializer.cpp
    core/MainWindow.cpp
    # recipes/infra
    recipes/infra/TorusTopology.cpp
    recipes/infra/MooreN.cpp
    recipes/infra/VonNeumannN.cpp
    recipes/infra/RecipeRegistry.cpp
    recipes/infra/RecipePresetRegistry.cpp
    recipes/infra/RecipeParameterWidget.cpp
    recipes/infra/RecipePresetDialog.cpp
    # recipes/impl
    recipes/impl/Life.cpp
    recipes/impl/LifeHistory.cpp
    recipes/impl/Parity.cpp
    recipes/impl/TubeWorms.cpp
    recipes/impl/StableTubeWorms.cpp
    recipes/impl/TimeTunnel.cpp
    recipes/impl/CyclicRank.cpp
    recipes/impl/CyclicRankParameterWidget.cpp
    recipes/impl/BrianBrain.cpp
    recipes/impl/Histogram.cpp
    recipes/impl/Scarves.cpp
    # rendering
    rendering/SinglePlaneStrategy.cpp
    rendering/BitfieldStrategy.cpp
    rendering/SaturatingSumStrategy.cpp
    rendering/NormalizedSumStrategy.cpp
    rendering/TrailPostProcessor.cpp
    rendering/AntiflickerPostProcessor.cpp
    rendering/GrayscalePostProcessor.cpp
    rendering/CellularCanvas.cpp
    rendering/color_map/ColorMapPresets.cpp
    rendering/color_map/ColorMapPresetsDialog.cpp
    # converters
    converters/PlanesConverter.cpp
    converters/CsvPlanesConverter.cpp
    converters/RlePlanesConverter.cpp
    converters/LifeRlePlanesConverter.cpp
    converters/PlaintextPlanesConverter.cpp
    converters/PlanesCsvConverter.cpp
    converters/PlanesRleConverter.cpp
    # shell
    shell/SavePlanesAction.cpp
    shell/LoadPlanesAction.cpp
    shell/SimulationActions.cpp
    shell/InitializeActions.cpp
    shell/RandomInitDialog.cpp
    shell/SquareInitDialog.cpp
    shell/CircleInitDialog.cpp
    shell/SeedInputDialog.cpp
    shell/ColorMapFromFileAction.cpp
    shell/ColorMapPresetsAction.cpp
    shell/RecipeActions.cpp
    shell/RecipeChangeDialog.cpp
    shell/RecipeInfoDialog.cpp
    shell/KeyMappingsData.cpp
    shell/KeyMappingsDialog.cpp
    shell/AboutDialog.cpp
    shell/HelpActions.cpp
    shell/KeyboardController.cpp
    shell/MouseController.cpp
    shell/StatusBarManager.cpp
)
target_include_directories(ntcap-gui PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    core
    recipes/infra
    recipes/impl
    rendering
    rendering/color_map
    converters
    shell
)
target_link_libraries(ntcap-gui
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)
# ── tests ─────────────────────────────────────────────
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