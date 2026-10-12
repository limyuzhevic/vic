# NLM Python Examples CMake Configuration
# Modern CMake setup for Python examples

cmake_minimum_required(VERSION 3.16)
project(nlm_python_examples VERSION 0.1.0 LANGUAGES CXX)

# Enable modern CMake features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Python and pybind11
find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Configure build options
option(BUILD_EXAMPLES "Build Python examples" ON)
option(BUILD_TESTS "Build example tests" OFF)

# Example source files
file(GLOB EXAMPLE_SOURCES "*.py")

# Create Python example targets
foreach(example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)
    
    # Skip non-example files
    if(example_name MATCHES "^test" OR example_name MATCHES "\.in$" OR example_name MATCHES "^pynlm")
        continue()
    endif()
    
    # Create Python example target
    add_custom_target(${example_name}
        COMMAND ${Python3_EXECUTABLE} ${example_source}
        DEPENDS pynlm
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running example: ${example_name}"
        VERBATIM
    )
    
    # Add dependency to all target
    add_custom_target(example_all DEPENDS ${example_name})
endforeach()

# Add example documentation target
add_custom_target(document_examples
    COMMAND ${Python3_EXECUTABLE} -c "
import sys
import os
sys.path.insert(0, '.')
print('NLM Python Examples Documentation')
print('=' * 60)
for example in ${EXAMPLE_SOURCES}:
    if not example.startswith('test') and not example.endswith('.in') and not example.startswith('pynlm'):
        print(f'  {example}: Python example script')
        print(f'    - Demonstrates NLM functionality')
        print(f'    - Can be run with: python {example}')
"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generate example documentation"
    VERBATIM
)

# Add example testing if requested
if(BUILD_TESTS)
    enable_testing()
    
    # Add test for each example
    foreach(example_source ${EXAMPLE_SOURCES})
        get_filename_component(example_name ${example_source} NAME_WE)
        
        if(example_name MATCHES "^test" OR example_name MATCHES "\.in$" OR example_name MATCHES "^pynlm")
            continue()
        endif()
        
        add_test(NAME test_example_${example_name}
            COMMAND ${Python3_EXECUTABLE} ${example_source}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()
endif()

# Install examples if requested
install(DIRECTORY . DESTINATION examples
    FILES_MATCHING PATTERN "*.py"
    PATTERN "^test.*\.py$" EXCLUDE
    PATTERN "^pynlm.*\.py$" EXCLUDE
)

message(STATUS "Found Python ${Python3_VERSION}")
message(STATUS "Found pybind11 ${pybind11_VERSION}")
message(STATUS "Examples configured: ${EXAMPLE_SOURCES}")