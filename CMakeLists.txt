cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "NLM — 熙然 (Neural Learning Machine)"
    HOMEPAGE_URL "https://github.com/yourorg/nlm"
)

# C++20 standard with target-specific properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create CMake modules directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# Include modern CMake patterns
include(cmake/modern_cmake.cmake)
include(cmake/dependency_management.cmake)
include(cmake/testing.cmake)
include(cmake/installation.cmake)

# Configure build types with compiler flags
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
    add_compile_options(-O0 -g -DDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELEASE")
    add_compile_options(-O3 -DNDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
    add_compile_options(-Os -DNDEBUG)
endif()

# Add source subdirectories using target-based approach
add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(tests)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
