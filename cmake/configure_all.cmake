# Main configuration orchestration script
# This script generates all necessary configuration files for NLM project

# Include CMake modules
include(cmake/modern_cmake.cmake)
include(cmake/dependency_management.cmake)
include(cmake/testing.cmake)
include(cmake/installation.cmake)

# Set up CMake project if not already set up
if (NOT DEFINED NLM_CONFIGURED)
    cmake_minimum_required(VERSION 3.16)
    project(NLM VERSION 0.1.0 LANGUAGES CXX)
    
    set(NLM_CONFIGURED TRUE)
endif()

# Generate configuration headers
if (NOT TARGET config_generation)
    add_custom_target(config_generation
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config_header_generation.cmake
        COMMENT "Generate configuration headers"
        VERBATIM
    )
endif()

# Generate package configuration
if (NOT TARGET package_generation)
    add_custom_target(package_generation
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/package_config.cmake
        COMMENT "Generate package configuration"
        VERBATIM
    )
endif()

# Generate documentation
if (NOT TARGET docs_generation)
    add_custom_target(docs_generation
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs_generation.cmake
        COMMENT "Generate documentation"
        VERBATIM
    )
endif()

# Generate test configuration
if (NOT TARGET test_configuration)
    add_custom_target(test_configuration
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_configuration.cmake
        COMMENT "Generate test configuration"
        VERBATIM
    )
endif()

# Generate build analysis
if (NOT TARGET build_analysis)
    add_custom_target(build_analysis
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_analysis.cmake
        COMMENT "Generate build analysis"
        VERBATIM
    )
endif()

# Main configuration target
add_custom_target(configure_all
    COMMAND echo "Configuring NLM project..."
    COMMAND ${CMAKE_COMMAND} -D CONFIG_HEADER_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}/config/nlm_config.h.in
            -D CONFIG_HEADER_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config_header_generation.cmake
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/package_config.cmake
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs_generation.cmake
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_configuration.cmake
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_analysis.cmake
    COMMENT "Generate complete configuration for NLM project"
    VERBATIM
)

# Print configuration summary
function(print_configuration_summary)
    message(STATUS "\nNLM Build Configuration Summary")
    message(STATUS "================================")
    message(STATUS "Project: NLM ${PROJECT_VERSION}")
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "System: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "Build Directory: ${CMAKE_CURRENT_BINARY_DIR}")
    message(STATUS "\nGenerated Targets:")
    message(STATUS "  - config_generation (header generation)")
    message(STATUS "  - package_generation (package configuration)")
    message(STATUS "  - docs_generation (documentation)")
    message(STATUS "  - test_configuration (test setup)")
    message(STATUS "  - build_analysis (build analysis)")
    message(STATUS "\nConfiguration files generated:")
    message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h")
    message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/nlm_version.h")
    message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/BUILD_INFO.txt")
    message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/CACHE_INFO.txt")
    message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake")
    message(STATUS "  - ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake")
endfunction()

# Execute configuration summary
print_configuration_summary()

# Display next steps
message(STATUS "\nNext Steps:")
message(STATUS "1. Run 'cmake --build . --target all' to build the project")
message(STATUS "2. Run 'cmake --build . --target config_generation' to regenerate headers")
message(STATUS "3. Run 'cmake --build . --target package_generation' to regenerate package config")
message(STATUS "4. Run 'cmake --build . --target docs_generation' to regenerate documentation")
message(STATUS "5. Run 'cmake --build . --target test_configuration' to regenerate test config")
message(STATUS "6. Run 'cmake --build . --target build_analysis' to generate build analysis")
message(STATUS "\nConfiguration completed successfully!")
