# NLM Configuration
# Common build and runtime options

# Enable/disable features
set(NLM_ENABLE_PYTHON_BINDINGS ON CACHE BOOL "Enable Python bindings for NLM")
set(NLM_ENABLE_TESTS ON CACHE BOOL "Enable tests")
set(NLM_ENABLE_GUI OFF CACHE BOOL "Enable GUI visualization")

# Performance settings
set(NLM_THREADS_MAX "8" CACHE STRING "Maximum threads for parallel processing")
set(NLM_MEMORY_POOL_SIZE "64MB" CACHE STRING "Size of memory pools")

# Simulation defaults
set(NLM_DEFAULT_NEURONS "1000" CACHE STRING "Default number of neurons")
set(NLM_DEFAULT_REGIONS "1" CACHE STRING "Default number of regions")

# Debug settings
set(NLM_LOG_LEVEL "INFO" CACHE STRING "Default log level (DEBUG, INFO, WARN, ERROR)")
set(NLM_DEBUG_MODE "OFF" CACHE BOOL "Enable debug mode")

# Output settings
set(NLM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output" CACHE PATH "Output directory for results")

# Platform-specific settings
if(WIN32)
    set(NLM_TEMP_DIR "${CMAKE_BINARY_DIR}/tmp")
else()
    set(NLM_TEMP_DIR "/tmp/nlm")
endif()

# Configure log level
if(NLM_LOG_LEVEL STREQUAL "DEBUG")
    add_compile_definitions(NLM_DEBUG)
endif()

# Create output directory
file(MAKE_DIRECTORY ${NLM_OUTPUT_DIR})
