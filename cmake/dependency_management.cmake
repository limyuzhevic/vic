# Dependency management for NLM project using modern CMake practices

include(FetchContent)

# Python bindings dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Optional performance libraries
if (NLM_ENABLE_LAPACK)
    find_package(LAPACK REQUIRED)
    find_package(BLAS REQUIRED)
endif()

# Test framework dependencies
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

# Configure test framework if enabled
if (NLM_ENABLE_TESTS)
    include(GoogleTest)
    enable_testing()
endif()

# External library management with modern CMake patterns
macro(nlm_add_external_dependency library_name)
    if (${library_name}_FOUND)
        message(STATUS "Using external ${library_name}")
    else()
        message(STATUS "Fetching ${library_name} via FetchContent")
        FetchContent_Declare(
            ${library_name}
            GIT_REPOSITORY ${library_name}_REPOSITORY
            GIT_TAG ${library_name}_VERSION
        )
        FetchContent_MakeAvailable(${library_name})
    endif()
endmacro()

# Dependency visualization
function(generate_dependency_graph)
    find_package(Graphviz QUIET)
    if (Graphviz_FOUND)
        add_custom_target(dependency_graph
            COMMAND cmake --graphviz=${CMAKE_BINARY_DIR}/dependency_graph.dot
                ${CMAKE_BINARY_DIR}/CMakeCache.txt
            COMMENT "Generate dependency graph"
            VERBATIM
        )
        message(STATUS "Dependency graph generation enabled")
    endif()
endfunction()

# Build analysis tools
function(add_build_analysis_targets)
    # Build time analysis
    add_custom_target(build_time_analysis
        COMMAND ${CMAKE_COMMAND} -E time cmake --build . --target all
        COMMENT "Analyze build performance"
        VERBATIM
    )
    
    # Memory usage analysis
    add_custom_target(memory_analysis
        COMMAND ${CMAKE_COMMAND} -E echo "Memory analysis requires external tools"
        COMMENT "Analyze memory usage"
        VERBATIM
    )
    
    # Parallel build analysis
    add_custom_target(parallel_build_analysis
        COMMAND ${CMAKE_COMMAND} -E echo "Parallel build analysis requires benchmarking tools"
        COMMENT "Analyze parallel build performance"
        VERBATIM
    )
endfunction()

# Package manager integration
function(setup_package_manager_integration)
    # For Ubuntu/Debian
    find_program(APT_GET apt-get)
    if (APT_GET)
        add_custom_target(install_deps_ubuntu
            COMMAND ${APT_GET} update
            COMMAND ${APT_GET} install -y libpython3-dev python3-dev
            COMMENT "Install dependencies via apt-get"
            VERBATIM
        )
    endif()
    
    # For CentOS/RHEL
    find_program(YUM yum)
    if (YUM)
        add_custom_target(install_deps_centos
            COMMAND ${YUM} install -y python3-devel
            COMMENT "Install dependencies via yum"
            VERBATIM
        )
    endif()
    
    # For macOS with Homebrew
    find_program(BREW brew)
    if (BREW)
        add_custom_target(install_deps_macos
            COMMAND ${BREW} install python3
            COMMENT "Install dependencies via Homebrew"
            VERBATIM
        )
    endif()
endfunction()

# Configuration file generation
function(generate_config_files)
    # Generate compile_commands.json for tooling
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    # Generate build information
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_info.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/build_info.h"
        @ONLY
    )
    
    # Add custom target for config generation
    add_custom_target(generate_config
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config_generation.cmake
        COMMENT "Generate configuration files"
        VERBATIM
    )
endfunction()

# Code generation support
function(add_code_generation_support)
    # Template configuration generation
    add_custom_target(generate_config
        COMMAND ${CMAKE_COMMAND} -D "CONFIG_OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/config"
                -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_config.cmake
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_config.cmake
        COMMENT "Generate configuration files"
        VERBATIM
    )
    
    # Documentation generation
    find_package(Doxygen QUIET)
    if (DOXYGEN_FOUND)
        doxygen_add_docs(docs
            ${CMAKE_CURRENT_SOURCE_DIR}/docs/
            COMMENT "Generate documentation"
        )
    endif()
endfunction()