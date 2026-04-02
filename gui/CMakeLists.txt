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
    converters/PlanesConverter.cpp
    converters/CsvPlanesConverter.cpp
    converters/RlePlanesConverter.cpp
    menu_bar/LoadPlanesAction.cpp
    menu_bar/SavePlanesAction.cpp
    menu_bar/SimulationActions.cpp
    recipes/Life.cpp
    recipes/LifeHistory.cpp
    recipes/Parity.cpp
)

target_include_directories(ntcap-gui PRIVATE .)
target_include_directories(ntcap-gui PRIVATE converters)
target_include_directories(ntcap-gui PRIVATE menu_bar)
target_include_directories(ntcap-gui PRIVATE recipes)

target_link_libraries(ntcap-gui
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        ntcap
)

