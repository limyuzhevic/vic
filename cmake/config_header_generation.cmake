# Configuration header generation script

# Parse command line arguments
if (DEFINED CONFIG_HEADER_SOURCE)
    set(NLM_CONFIG_HEADER_SOURCE "${CONFIG_HEADER_SOURCE}")
else()
    set(NLM_CONFIG_HEADER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/config/nlm_config.h.in")
endif()

if (DEFINED CONFIG_HEADER_OUTPUT)
    set(NLM_CONFIG_HEADER "${CONFIG_HEADER_OUTPUT}")
else()
    set(NLM_CONFIG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h")
endif()

# Ensure output directory exists
get_filename_component(NLM_CONFIG_HEADER_DIR "${NLM_CONFIG_HEADER}" DIRECTORY)
file(MAKE_DIRECTORY "${NLM_CONFIG_HEADER_DIR}")

# Generate configuration header
if (EXISTS "${NLM_CONFIG_HEADER_SOURCE}")
    configure_file(
        "${NLM_CONFIG_HEADER_SOURCE}"
        "${NLM_CONFIG_HEADER}"
        @ONLY
    )
    
    message(STATUS "Configuration header generated: ${NLM_CONFIG_HEADER}")
else()
    message(WARNING "Configuration header source not found: ${NLM_CONFIG_HEADER_SOURCE}")
endif()

# Generate version information
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nlm_version.h"
    @ONLY
)

# Generate build information
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/BUILD_INFO.txt"
    "NLM Build Information\n"
    "=====================\n\n"
    "Project: NLM ${PROJECT_VERSION}\n"
    "Build Date: ${BUILD_DATE}\n"
    "Build Time: ${BUILD_TIME}\n"
    "Build Type: ${CMAKE_BUILD_TYPE}\n"
    "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\n"
    "Architecture: ${CMAKE_SYSTEM_PROCESSOR}\n"
    "Build Directory: ${CMAKE_CURRENT_BINARY_DIR}\n"
)

# Generate CMake cache information
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CACHE_INFO.txt"
    "CMake Cache Information\n"
    "======================\n\n"
)

# Read important cache variables
foreach(var "CMAKE_CXX_FLAGS" "CMAKE_EXE_LINKER_FLAGS" "CMAKE_SHARED_LINKER_FLAGS")
    get_property(var_value CACHE ${var} GET_VALUE)
    if (var_value)
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/CACHE_INFO.txt"
            "${var}: ${var_value}\n"
        )
    endif()
endforeach()

# Create final status message
message(STATUS "Configuration generation completed successfully")
message(STATUS "Generated files:")
message(STATUS "  - ${NLM_CONFIG_HEADER}")
message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/nlm_version.h")
message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/BUILD_INFO.txt")
message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/CACHE_INFO.txt")