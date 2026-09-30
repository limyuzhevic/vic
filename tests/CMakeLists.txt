# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Advanced
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Performance
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Debug
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Experiment
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Validation
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Output
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine/Parser
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine/Options
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine/Validation
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine/Help
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/CommandLine/Utils
)