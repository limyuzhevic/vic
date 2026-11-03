# NLM Examples CMake Configuration
# This file provides build configurations for all NLM examples

cmake_minimum_required(VERSION 3.16)
project(NLM_Examples LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find NLM library
find_package(nlm REQUIRED)

# Enable testing
enable_testing()

# Set target properties for examples
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create example executables
file(GLOB EXAMPLE_SOURCES "*.cpp")

foreach(example ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example} NAME_WE)
    
    # Create executable
    add_executable(${example_name} ${example})
    
    # Link with NLM libraries
    target_link_libraries(${example_name} PRIVATE nlm::nlm_core)
    
    # Set C++ standard properties
    target_compile_features(${example_name} PRIVATE cxx_std_20)
    
    # Add to tests if testing is enabled
    if (NLM_ENABLE_TESTS)
        add_test(NAME ${example_name}
            COMMAND ${example_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
    
    # Install executable
    install(TARGETS ${example_name} DESTINATION bin)
endforeach()

# Create examples package
install(DIRECTORY . DESTINATION share/nlm/examples
    FILES_MATCHING PATTERN "*.md"
    PATTERN "*.txt"
    PATTERN "CMakeLists.txt"
)

# Documentation
install(DIRECTORY . DESTINATION share/doc/nlm/examples
    FILES_MATCHING PATTERN "*.md"
)

# Summary
message(STATUS "=== NLM Examples Configuration ===")
message(STATUS "Examples configured: ${PROJECT_NAME}")
message(STATUS "Total example executables: ${CMAKE_PROJECT_NAME}")
message(STATUS "================================")