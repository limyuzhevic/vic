# Command Line Parser
add_library(nlm_commandline
    core/CommandLine/CommandLineParser.cpp
)

target_include_directories(nlm_commandline PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/CommandLine
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_link_libraries(nlm_commandline PUBLIC nlm_core)
