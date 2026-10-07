# CMake configuration for Python bindings

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings)

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Find pybind11
find_package(pybind11 REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Python module
pybind11_add_module(pynlm python/bindings.cpp)

# Target includes
target_include_directories(pynlm PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries (should be added after core library is built)
# target_link_libraries(pynlm PRIVATE nlm_core)

# Installation
install(TARGETS pynlm DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)

# Testing
enable_testing()
add_test(NAME test_pynlm_bindings
    COMMAND ${Python_EXECUTABLE} -c "import pynlm; print('Python bindings import successful')"
)