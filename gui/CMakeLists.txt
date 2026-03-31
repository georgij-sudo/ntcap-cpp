set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(ntcap-gui
    main.cpp
    CellularCanvas.cpp
    AutomatonController.cpp
    KeyboardController.cpp
)

target_link_libraries(ntcap-gui
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        ntcap
)

