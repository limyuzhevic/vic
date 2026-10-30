cmake_minimum_required(VERSION 3.16)

# Core module CMakeLists.txt
# Contains: Types, Config, Random, Logger, SimulationClock

# Core library
add_library(nlm_core STATIC
    core/Types/Types.cpp
    core/Config/Config.cpp
    core/Random/Random.cpp
    core/Logger/Logger.cpp
    core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Install targets
install(TARGETS nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY core/
    DESTINATION include/core
    FILES_MATCHING PATTERN "*.hpp"
)

# Configuration options
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
