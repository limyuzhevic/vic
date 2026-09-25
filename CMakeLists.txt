cmake_minimum_required(VERSION 3.16)
project(NLM Enhanced Error Handling and Diagnostics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(
    core/ErrorHandling
    core/EnhancedErrorHandling
    core/Diagnostics
    core/PerformanceAnalysis
)

# Source files for core modules
set(CORE_SOURCES
    core/ErrorHandling/ErrorHandling.cpp
    core/EnhancedErrorHandling/EnhancedErrorHandling.cpp
    core/Diagnostics/Diagnostics.cpp
    core/PerformanceAnalysis/PerformanceAnalysis.cpp
)

# Header files for core modules
set(CORE_HEADERS
    core/ErrorHandling/ErrorHandling.hpp
    core/EnhancedErrorHandling/EnhancedErrorHandling.hpp
    core/Diagnostics/Diagnostics.hpp
    core/PerformanceAnalysis/PerformanceAnalysis.hpp
)

# Create NLM core library
add_library(nlm_core ${CORE_SOURCES} ${CORE_HEADERS})

# Create demo executable
add_executable(nlm_demo demo.cpp)
target_link_libraries(nlm_demo nlm_core)

# Print configuration summary
message(STATUS "NLM Enhanced Error Handling Configuration:")
message(STATUS "  - Core library: nlm_core")
message(STATUS "  - Error handling framework: Enhanced")
message(STATUS "  - Diagnostics system: Enabled")
message(STATUS "  - Performance analysis: Enabled")
message(STATUS "  - Error recovery: Enabled")
message(STATUS "  - Error severity levels: Debug, Info, Warning, Error, Critical, Fatal")
message(STATUS "  - Error categories: System, Configuration, Memory, Neural, Plasticity, Sensory, Motor, Integration, Development, External")
