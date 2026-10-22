pybind11_build = "1.13.0"
pybind11_language_level = 3

# Project metadata
project_name = "pynlm"
project_version = "0.1.0"
project_description = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Configuration for Python-specific enhancements

# Compiler optimization flags for Python bindings
if MSVC
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# Python binding specific settings
set_target_properties(pynlm PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
)

# Include directories for Python bindings
target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${Python_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Create Python package structure
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/python/pynlm")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py.in"
    "${CMAKE_BINARY_DIR}/python/pynlm/__init__.py"
    @ONLY
)

# Add test target for Python bindings
enable_testing()
add_test(NAME pynlm_python_test
    COMMAND ${Python_EXECUTABLE} -c "import sys; sys.path.insert(0, '${CMAKE_BINARY_DIR}/python'); import pynlm; print('Python bindings test passed')"
)
