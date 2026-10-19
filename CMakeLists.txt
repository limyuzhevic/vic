cmake_minimum_required(VERSION 3.16)
project(NLM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project version
project(NLM VERSION 2.0.0 LANGUAGES CXX)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source files for core libraries
file(GLOB CORE_SOURCES "src/core/*.cpp")
file(GLOB CORE_HEADERS "src/core/*.hpp")
file(GLOB CORE_DIRECTORIES "src/core/*/")

# Collect all core source files
foreach(dir ${CORE_DIRECTORIES})
    file(GLOB dir_sources "${dir}*.cpp")
    list(APPEND CORE_SOURCES ${dir_sources})
    file(GLOB dir_headers "${dir}*.hpp")
    list(APPEND CORE_HEADERS ${dir_headers})
endforeach()

# Collect CLI source files
file(GLOB CLI_SOURCES "src/core/CLI/*.cpp")
file(GLOB CLI_HEADERS "src/core/CLI/*.hpp")

# Collect EnhancedConfigManager sources
file(GLOB CONFIG_MANAGER_SOURCES "src/core/EnhancedConfigManager.cpp")

# Collect main sources
file(GLOB MAIN_SOURCES "src/main.cpp")

# Add executable
add_executable(nlm ${MAIN_SOURCES} ${CORE_SOURCES} ${CLI_SOURCES} ${CONFIG_MANAGER_SOURCES})

# Target properties
target_include_directories(nlm PRIVATE src)
target_compile_features(nlm PRIVATE cxx_std_20)

# Add test targets
enable_testing()

# Add unit tests if available
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS nlm DESTINATION bin)

# Export configuration for package management
set(NLM_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/NLM)
install(EXPORT NLMTargets EXPORT NLMTargets
    FILE NLMConfig.cmake
    NAMESPACE NLM::
    DESTINATION ${NLM_INSTALL_CMAKEDIR}
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${NLM_INSTALL_CMAKEDIR}
)