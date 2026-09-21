# CMake configuration for NLM package management
set(NLM_VERSION "0.1.0")
set(NLM_BUILD_TYPE "Release")
set(NLM_PYTHON_BINDINGS TRUE)
set(NLM_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(NLM_LIB_DIR "${NLM_INSTALL_PREFIX}/lib")
set(NLM_INCLUDE_DIR "${NLM_INSTALL_PREFIX}/include")

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Find pybind11
find_package(pybind11 CONFIG REQUIRED)

# Package configuration
include(CMakePackageConfigHelpers)

# Generate package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NLM
)

# Generate version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${NLM_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Export targets
export(EXPORT NLMTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake"
)

# Install package config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

# Install exported targets
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    DESTINATION lib/cmake/NLM
)

# Find and configure Python module
if(NLM_PYTHON_BINDINGS)
    # Add Python module build
    add_subdirectory(python)
endif()