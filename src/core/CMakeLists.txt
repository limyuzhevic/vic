cmake_minimum_required(VERSION 3.16)

# Core utilities
add_library(nlm_core_types
    Types/Types.cpp
)

target_include_directories(nlm_core_types PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/core>
    $<INSTALL_INTERFACE:include/core>
)

add_library(nlm_core_config
    Config/Config.cpp
)

target_include_directories(nlm_core_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/core>
)

add_library(nlm_core_random
    Random/Random.cpp
)

target_include_directories(nlm_core_random PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/core>
)

add_library(nlm_core_logger
    Logger/Logger.cpp
)

target_include_directories(nlm_core_logger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/core>
)

add_library(nlm_core_simulation_clock
    SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core_simulation_clock PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/core>
)

# Aggregate core library for backward compatibility
add_library(nlm_core ALIAS nlm_core_types)

# Core dependencies
foreach(target nlm_core_types nlm_core_config nlm_core_random nlm_core_logger nlm_core_simulation_clock)
    if(TARGET ${target})
        # Core modules are independent, no internal dependencies
    endif()
endforeach()
