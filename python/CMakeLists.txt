cmake_minimum_required(VERSION 3.16)
project(nlm_enhanced VERSION 2.0.0 LANGUAGES CXX)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIRS})
include_directories(../src)

# Enhanced bindings source files
set(ENHANCED_BINDINGS_SRC
    enhanced_bindings.cpp
)

# Basic bindings source files (for reference)
set(BASIC_BINDINGS_SRC
    bindings.cpp
)

# Create enhanced module
pybind11_add_module(nlm_enhanced ${ENHANCED_BINDINGS_SRC})

# Link against pybind11 and Python
target_link_libraries(nlm_enhanced PRIVATE pybind11::module)

# Set module properties
set_target_properties(nlm_enhanced PROPERTIES
    PREFIX ""
    OUTPUT_NAME "nlm_enhanced"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Install target
install(TARGETS nlm_enhanced
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Create build directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

# Print build information
message(STATUS "Building NLM Enhanced Python bindings")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python include directories: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "pybind11 include directories: ${pybind11_INCLUDE_DIRS}")
message(STATUS "Enhanced bindings source: ${ENHANCED_BINDINGS_SRC}")
message(STATUS "Enhanced bindings target: nlm_enhanced")

# Add custom targets for easy building
add_custom_target(build_enhanced
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target nlm_enhanced
    COMMENT "Building NLM Enhanced Python bindings"
    VERBATIM
)

add_custom_target(install_enhanced
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix .
    COMMENT "Installing NLM Enhanced Python bindings"
    VERBATIM
)

# Print build instructions
message(STATUS "Build instructions:")
message(STATUS "  mkdir build")
message(STATUS "  cd build")
message(STATUS "  cmake ..")
message(STATUS "  make -j4")
message(STATUS "")
message(STATUS "After building, you can install the module:")
message(STATUS "  make install_enhanced")
message(STATUS "")
message(STATUS "The enhanced module will be available as:")
message(STATUS "  Python: from nlm_enhanced import *")
message(STATUS "  Module file: build/nlm_enhanced${PYTHON_MODULE_EXTENSION}")