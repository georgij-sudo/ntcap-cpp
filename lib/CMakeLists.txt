add_library(ntcap
    TorusTopology.cpp
)

set_target_properties(ntcap PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ntcap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

