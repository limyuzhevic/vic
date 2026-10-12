# Debug Tools Library
add_library(nlm_debugtools
    core/DebugTools/DebugTools.cpp
)

target_include_directories(nlm_debugtools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/DebugTools
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(nlm_debugtools PUBLIC nlm_core)
