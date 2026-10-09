# NLM Build Options Module
# Provides configuration file support and build options

# Create configuration file support
set(NLM_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../configs)

# Function to load external configuration
guard(NLM_LOAD_EXTERNAL_CONFIG)
    if(EXISTS "${NLM_CONFIG_DIR}/NLMConfig.cmake")
        include(${NLM_CONFIG_DIR}/NLMConfig.cmake)
    endif()
endguard()

# Function to load user configuration
guard(NLM_LOAD_USER_CONFIG)
    if(EXISTS "${CMAKE_HOME_DIRECTORY}/.nlmc/config.cmake")
        include(${CMAKE_HOME_DIRECTORY}/.nlmc/config.cmake)
    endif()
endguard()

# Create build options documentation
file(WRITE ${CMAKE_BINARY_DIR}/NLMBuildOptions.txt
"NLM Build Options:
===================

Build Types:
- Debug: Debug build with debugging symbols and no optimizations
- Release: Optimized build with maximum performance
- RelWithDebInfo: Release build with debugging symbols
- MinSizeRel: Optimized for minimal binary size

Feature Toggles:
- NLM_ENABLE_PYTHON_BINDINGS: Enable Python bindings (default: ON)
- NLM_ENABLE_TESTS: Enable tests (default: ON)
- NLM_ENABLE_DOCS: Enable documentation generation (default: OFF)
- NLM_ENABLE_COVERAGE: Enable code coverage reporting (default: OFF)
- NLM_ENABLE_SANITIZERS: Enable memory leak detection (default: OFF)
- NLM_ENABLE_PERFORMANCE_PROFILING: Enable performance profiling (default: OFF)
- NLM_ENABLE_DISTRIBUTED_BUILD: Enable distributed build support (default: OFF)
- NLM_BUILD_SHARED_LIBS: Build shared libraries (default: OFF)

Compiler Optimizations:
- Debug: -O0, -g, -DDEBUG
- Release: -O3, -DNDEBUG
- RelWithDebInfo: -O2, -g, -DNDEBUG
- MinSizeRel: -Os, -DNDEBUG

Platform Support:
- Windows: MSVC compiler with specific runtime libraries
- macOS: Clang compiler with Accelerate framework
- Linux: GCC/Clang with X11 support

Dependencies:
- Python: Required for Python bindings
- pybind11: Required for Python bindings
- Boost: System and thread components
- Eigen: Optional linear algebra
- FFmpeg: Optional multimedia support
- Graphviz: Optional dependency graph visualization
"
)

# Add custom target to display build options
add_custom_target(nlm_build_options
    COMMAND ${CMAKE_COMMAND} -E echo "NLM Build Options:"
    COMMAND ${CMAKE_COMMAND} -E echo "==================="
    COMMAND ${CMAKE_COMMAND} -E echo "Build Type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "Platform: ${CMAKE_SYSTEM_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "Compiler: ${CMAKE_CXX_COMPILER_ID}"
    COMMAND ${CMAKE_COMMAND} -E echo "Python Bindings: ${NLM_ENABLE_PYTHON_BINDINGS}"
    COMMAND ${CMAKE_COMMAND} -E echo "Tests: ${NLM_ENABLE_TESTS}"
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation: ${NLM_ENABLE_DOCS}"
    COMMAND ${CMAKE_COMMAND} -E echo "Coverage: ${NLM_ENABLE_COVERAGE}"
    COMMAND ${CMAKE_COMMAND} -E echo "Sanitizers: ${NLM_ENABLE_SANITIZERS}"
    COMMAND ${CMAKE_COMMAND} -E echo "Performance Profiling: ${NLM_ENABLE_PERFORMANCE_PROFILING}"
    COMMAND ${CMAKE_COMMAND} -E echo "Distributed Build: ${NLM_ENABLE_DISTRIBUTED_BUILD}"
    COMMAND ${CMAKE_COMMAND} -E echo "Shared Libraries: ${NLM_BUILD_SHARED_LIBS}"
    COMMENT "Displaying NLM build options"
    VERBATIM
)
