# This file is kept for backward compatibility but has been superseded by the main CMakeLists.txt
# The actual Python bindings are now built directly in the main CMakeLists.txt using pybind11 integration
# Do not build separate targets here - they are included in the main build

# Placeholder to avoid CMake configuration errors
project(pynlm_backward_compatibility LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
