# Python bindings CMakeLists.txt
# Links to existing targets created by the main CMakeLists.txt
# Does NOT recreate core targets to avoid conflicts

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Python bindings module - links to existing targets
pybind11_add_module(pynlm bindings.cpp)

# Link to targets created by the main CMakeLists.txt
# Note: This assumes nlm_core, nlm_agent, nlm_world are defined in the main CMakeLists.txt
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Include source directory for headers
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
