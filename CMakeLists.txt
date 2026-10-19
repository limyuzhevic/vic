cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Function to create source file lists using glob patterns
nlm_create_source_list(NAME nlm_core
    PATTERN "src/**/*.cpp"
    EXCLUDE_PATTERN "*test*" "*Test*"
)

nlm_create_source_list(NAME nlm_agent
    PATTERN "src/agent/*.cpp"
)

nlm_create_source_list(NAME nlm_world
    PATTERN "src/world/*.cpp"
)

nlm_create_source_list(NAME nlm_phase3
    PATTERN "src/experiments/Phase3*.cpp"
)

nlm_create_source_list(NAME nlm_phase4
    PATTERN "src/experiments/Phase4*.cpp"
)

# Function implementation for creating source lists
function(nlm_create_source_list NAME)
    set(PATTERN "${ARGV0}")
    set(EXCLUDE_PATTERNS "${ARGV1}")
    
    file(GLOB_RECURSE SOURCES ${PATTERN})
    
    foreach(EXCLUDE_PATTERN ${EXCLUDE_PATTERNS})
        list(FILTER SOURCES EXCLUDE REGEX ${EXCLUDE_PATTERN})
    endforeach()
    
    set(${NAME}_SOURCES ${SOURCES} PARENT_SCOPE)
endfunction()

# Core library
add_library(nlm_core STATIC ${nlm_core_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC ${nlm_agent_SOURCES})
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC ${nlm_world_SOURCES})
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC ${nlm_phase3_SOURCES})
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC ${nlm_phase4_SOURCES})
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
