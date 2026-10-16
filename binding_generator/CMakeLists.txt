# CMakeLists.txt for NLM Python Binding Generator

# This CMakeLists.txt configures the NLM Python Binding Generator project
# which generates pybind11 bindings from NLM class definitions.

cmake_minimum_required(VERSION 3.16)
project(NLMBindingGenerator VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific configurations
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find required packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Output directories
set(BINDING_GENERATOR_OUTPUT_DIR ${CMAKE_BINARY_DIR}/generated_bindings)
set(BINDING_GENERATOR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/binding_generator/include)

# Source files
set(BINDING_GENERATOR_SOURCES
    src/main.cpp
    src/BindingGenerator.cpp
    src/BindingGeneratorIntegration.cpp
)

# Generated bindings source
add_library(binding_generator STATIC ${BINDING_GENERATOR_SOURCES})

# Include directories
target_include_directories(binding_generator PUBLIC
    $<BUILD_INTERFACE:${BINDING_GENERATOR_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(binding_generator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link with pybind11
find_package(pybind11 REQUIRED)
target_link_libraries(binding_generator PRIVATE pybind11::module)

# Python module configuration
pybind11_add_module(pynlm_bindings bindings.cpp)
target_link_libraries(pynlm_bindings PRIVATE binding_generator)

# Copy generated bindings to python directory
target_include_directories(pynlm_bindings PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Install targets
install(TARGETS binding_generator
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS pynlm_bindings
    LIBRARY DESTINATION python/pynlm
    ARCHIVE DESTINATION python/pynlm
    RUNTIME DESTINATION python/pynlm
)

# Installation of headers
install(DIRECTORY ${BINDING_GENERATOR_INCLUDE_DIR}/
    DESTINATION include/NLM/BindingGenerator
)

# Add tests
enable_testing()
add_subdirectory(tests)

# Custom targets for common operations
add_custom_target(generate_bindings
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BINDING_GENERATOR_OUTPUT_DIR}
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating Python bindings for NLM"
    VERBATIM
)

add_custom_target(test_bindings
    COMMAND ${CMAKE_COMMAND} -E echo "Testing generated bindings..."
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Testing NLM Python bindings"
    VERBATIM
)

add_custom_target(install_bindings
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDING_GENERATOR_OUTPUT_DIR} ${PYTHON_SITE_PACKAGES}/pynlm/generated_bindings
    COMMENT "Installing generated bindings"
    VERBATIM
)

# Print configuration summary
message(STATUS "NLM Python Binding Generator Configuration")
message(STATUS "  C++ Standard: C++20")
message(STATUS "  Python Interpreter: ${Python_EXECUTABLE}")
message(STATUS "  pybind11: ${pybind11_VERSION}")
message(STATUS "  Output Directory: ${BINDING_GENERATOR_OUTPUT_DIR}")
message(STATUS "  Include Directory: ${BINDING_GENERATOR_INCLUDE_DIR}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  make generate_bindings  - Generate Python bindings")
message(STATUS "  make test_bindings      - Test generated bindings")
message(STATUS "  make install_bindings   - Install generated bindings")
message(STATUS "")
message(STATUS "To build and install:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake ..")
message(STATUS "  make -j$(nproc)")
message(STATUS "  sudo make install")