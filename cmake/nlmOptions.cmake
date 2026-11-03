# nlm package options configuration

# Python bindings option
set(NLM_BUILD_PYTHON_BINDINGS ON CACHE BOOL "Build Python bindings")

# Testing options
set(NLM_ENABLE_TESTS ON CACHE BOOL "Enable unit tests")
set(NLM_ENABLE_INTEGRATION_TESTS ON CACHE BOOL "Enable integration tests")

# Performance options
set(NLM_ENABLE_SIMD ON CACHE BOOL "Enable SIMD optimizations")
set(NLM_ENABLE_OPENMP ON CACHE BOOL "Enable OpenMP parallel processing")
set(NLM_ENABLE_TBB ON CACHE BOOL "Enable Intel TBB parallel processing")

# Memory management options
set(NLM_USE_MEMORY_POOLS ON CACHE BOOL "Use memory pools for efficiency")
set(NLM_ENABLE_CHECKPOINTING ON CACHE BOOL "Enable checkpoint system")

# Debug options
set(NLM_DEBUG_MODE OFF CACHE BOOL "Enable debug mode")
set(NLM_ENABLE_ASSERTIONS ON CACHE BOOL "Enable runtime assertions")

# Logging options
set(NLM_LOG_LEVEL "INFO" CACHE STRING "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)")
set(NLM_LOG_TO_FILE ON CACHE BOOL "Log to file")
set(NLM_LOG_FILENAME "nlm.log" CACHE STRING "Log filename")

# Visualization options
set(NLM_ENABLE_VISUALIZATION OFF CACHE BOOL "Enable visualization")
set(NLM_VISUALIZATION_UPDATE_RATE 30.0 CACHE DOUBLE "Visualization update rate (Hz)")

# Validation of options
if(NLM_LOG_LEVEL NOT IN_LIST ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"])
    message(FATAL_ERROR "Invalid log level: ${NLM_LOG_LEVEL}. Valid options are: DEBUG, INFO, WARNING, ERROR, CRITICAL")
endif()

if(NLM_VISUALIZATION_UPDATE_RATE <= 0.0)
    message(FATAL_ERROR "Visualization update rate must be positive")
endif()

# Handle feature dependencies
if(NLM_ENABLE_TBB)
    find_package(TBB QUIET)
    if(TBB_FOUND)
        message(STATUS "TBB parallel processing enabled")
    else()
        message(WARNING "TBB not found, disabling TBB support")
        set(NLM_ENABLE_TBB OFF CACHE BOOL "Enable Intel TBB parallel processing" FORCE)
    endif()
endif()

if(NLM_ENABLE_OPENMP)
    find_package(OpenMP QUIET)
    if(OpenMP_Found)
        message(STATUS "OpenMP parallel processing enabled")
    else()
        message(WARNING "OpenMP not found, disabling OpenMP support")
        set(NLM_ENABLE_OPENMP OFF CACHE BOOL "Enable OpenMP parallel processing" FORCE)
    endif()
endif()

if(NLM_ENABLE_SIMD)
    # Check for SIMD support
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-msse4.2 HAS_SSE42)
    check_cxx_compiler_flag(-mavx2 HAS_AVX2)
    check_cxx_compiler_flag(-mfma HAS_FMA)
    
    if(NOT HAS_SSE42)
        message(WARNING "Compiler does not support SSE4.2, disabling SIMD optimizations")
        set(NLM_ENABLE_SIMD OFF CACHE BOOL "Enable SIMD optimizations" FORCE)
    endif()
endif()

# Set optimization flags based on options
if(NLM_ENABLE_SIMD AND HAS_SSE42)
    add_compile_options(-msse4.2)
endif()

if(NLM_ENABLE_SIMD AND HAS_AVX2)
    add_compile_options(-mavx2)
endif()

if(NLM_ENABLE_SIMD AND HAS_FMA)
    add_compile_options(-mfma)
endif()

# Set compile definitions based on options
if(NLM_ENABLE_ASSERTIONS)
    add_compile_definitions(NLM_ASSERTIONS)
endif()

if(NLM_DEBUG_MODE)
    add_compile_definitions(NLM_DEBUG_MODE)
endif()

if(NLM_LOG_TO_FILE)
    add_compile_definitions(NLM_LOG_FILENAME=${NLM_LOG_FILENAME})
endif()

# Configure visualization
if(NLM_ENABLE_VISUALIZATION)
    add_compile_definitions(NLM_ENABLE_VISUALIZATION)
endif()

# Message summary
message(STATUS "=== NLM Configuration Summary ===")
message(STATUS "Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "Unit tests: ${NLM_ENABLE_TESTS}")
message(STATUS "Integration tests: ${NLM_ENABLE_INTEGRATION_TESTS}")
message(STATUS "SIMD optimizations: ${NLM_ENABLE_SIMD}")
message(STATUS "OpenMP: ${NLM_ENABLE_OPENMP}")
message(STATUS "TBB: ${NLM_ENABLE_TBB}")
message(STATUS "Memory pools: ${NLM_USE_MEMORY_POOLS}")
message(STATUS "Checkpointing: ${NLM_ENABLE_CHECKPOINTING}")
message(STATUS "Debug mode: ${NLM_DEBUG_MODE}")
message(STATUS "Assertions: ${NLM_ENABLE_ASSERTIONS}")
message(STATUS "Log level: ${NLM_LOG_LEVEL}")
message(STATUS "Visualization: ${NLM_ENABLE_VISUALIZATION}")
message(STATUS "===============================")