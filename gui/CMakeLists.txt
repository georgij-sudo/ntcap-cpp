set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
add_executable(ntcap-gui
    main.cpp
    CellularCanvas.cpp
    AutomatonController.cpp
    KeyboardController.cpp
    MouseController.cpp
    Automaton.cpp
    MainWindow.cpp
    PlaneInitializer.cpp
    converters/PlanesConverter.cpp
    converters/CsvPlanesConverter.cpp
    converters/RlePlanesConverter.cpp
    menu_bar/LoadPlanesAction.cpp
    menu_bar/SavePlanesAction.cpp
    menu_bar/SimulationActions.cpp
    menu_bar/InitializeActions.cpp
    menu_bar/RandomInitDialog.cpp
    menu_bar/SquareInitDialog.cpp
    menu_bar/CircleInitDialog.cpp
    menu_bar/ColorMapFromFileAction.cpp
    menu_bar/ColorMapPresetsAction.cpp
    menu_bar/RecipeActions.cpp
    menu_bar/RecipeChangeDialog.cpp
    menu_bar/RecipeInfoDialog.cpp
    recipes/Life.cpp
    recipes/LifeHistory.cpp
    recipes/Parity.cpp
    recipes/TubeWorms.cpp
    recipes/StableTubeWorms.cpp
    recipes/TimeTunnel.cpp
    recipes/CyclicRank.cpp
    recipes/RecipeRegistry.cpp
    recipes/RecipeParameterWidget.cpp
    recipes/CyclicRankParameterWidget.cpp
    color_map/ColorMapPresets.cpp
    color_map/ColorMapPresetsDialog.cpp
    SinglePlaneStrategy.cpp
    BitfieldStrategy.cpp
    SaturatingSumStrategy.cpp
    NormalizedSumStrategy.cpp
    AntiflickerPostProcessor.cpp
    GrayscalePostProcessor.cpp
    TrailPostProcessor.cpp
    StatusBarManager.cpp
    recipes/RecipePresetRegistry.cpp    #(NEW)
    recipes/RecipePresetDialog.cpp      #(NEW)
)
target_include_directories(ntcap-gui PRIVATE .)
target_include_directories(ntcap-gui PRIVATE converters)
target_include_directories(ntcap-gui PRIVATE menu_bar)
target_include_directories(ntcap-gui PRIVATE recipes)
target_include_directories(ntcap-gui PRIVATE color_map)
target_link_libraries(ntcap-gui
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        ntcap
)