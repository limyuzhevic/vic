# NLM CLI CMake Configuration
# This file creates the nlm_cli executable that provides command-line interface
# to the NLM brain simulation system

cmake_minimum_required(VERSION 3.15)
project(nlm_cli VERSION 1.0.0 LANGUAGES CXX)

# Enable modern C++ features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required dependencies
find_package(Threads REQUIRED)

# Include the main source files
add_executable(nlm_cli
    main.cpp
)

# Target properties
target_compile_features(nlm_cli PRIVATE cxx_std_17)

# Link libraries
target_link_libraries(nlm_cli PRIVATE
    nlm_core
    ${CMAKE_THREAD_LIBS_INIT}
)

# Include directories
target_include_directories(nlm_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add dependencies
add_dependencies(nlm_cli nlm_core)

# Install target
install(TARGETS nlm_cli
    RUNTIME DESTINATION bin
    NAMELINK_SKIP
)

# Create a target for the library (this will be defined by the parent project)
# The parent CMakeLists.txt should define the nlm_core library

# Add aliases for easier usage
add_executable(nlm_cli_legacy nlm_cli)  # For backward compatibility

# Add test target if testing is enabled
if(BUILD_TESTING)
    enable_testing()
    add_test(NAME nlm_cli_test COMMAND nlm_cli --help)
endif()
