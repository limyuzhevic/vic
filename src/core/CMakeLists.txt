cmake_minimum_required(VERSION 3.16)
project(nlm_core)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core source files
set(NLM_CORE_SOURCES
    Types/Types.cpp
    Config/Config.cpp
    Configuration.cpp
    Random/Random.cpp
    Logger/Logger.cpp
    SimulationClock/SimulationClock.cpp
    Error/Error.cpp
)

# Create core library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

# Include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Compiler options
if(MSVC)
    target_compile_options(nlm_core PRIVATE /W4)
else()
    target_compile_options(nlm_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
