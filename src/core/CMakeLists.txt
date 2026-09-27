# NLM Core Layer - Fundamental types and utilities

# Core types
add_library(nlm_types STATIC
    src/core/Types/Types.cpp
)

target_include_directories(nlm_types PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Types>
    $<INSTALL_INTERFACE:include/core/types>
)

# Configuration system
add_library(nlm_config STATIC
    src/core/Config/Config.cpp
)

target_include_directories(nlm_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config>
    $<INSTALL_INTERFACE:include/core/config>
)

# Random number generation
add_library(nlm_random STATIC
    src/core/Random/Random.cpp
)

target_include_directories(nlm_random PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Random>
    $<INSTALL_INTERFACE:include/core/random>
)

# Logging system
add_library(nlm_logger STATIC
    src/core/Logger/Logger.cpp
)

target_include_directories(nlm_logger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Logger>
    $<INSTALL_INTERFACE:include/core/logger>
)

# Simulation clock
add_library(nlm_simulation_clock STATIC
    src/core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_simulation_clock PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/core/SimulationClock>
    $<INSTALL_INTERFACE:include/core/simulation_clock>
)

# Core interface library
add_library(nlm_core INTERFACE)
target_link_libraries(nlm_core INTERFACE
    nlm_types
    nlm_config
    nlm_random
    nlm_logger
    nlm_simulation_clock
)

# Core installation
install(TARGETS nlm_types nlm_config nlm_random nlm_logger nlm_simulation_clock
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Types
    DESTINATION include/core/types
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config
    DESTINATION include/core/config
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Random
    DESTINATION include/core/random
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Logger
    DESTINATION include/core/logger
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/SimulationClock
    DESTINATION include/core/simulation_clock
    FILES_MATCHING PATTERN "*.hpp"
)

# Core interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/include/nlm_core.hpp
    DESTINATION include/core
)