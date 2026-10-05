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

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX
    VERSION 0.1.0
    DESCRIPTION "Python bindings for NLM neural learning machine"
    HOMEPAGE_URL "https://github.com/yourorg/nlm"
)

# Python bindings specific settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set up source directory
file(GLOB PYTHON_BINDINGS_SOURCES "bindings*.cpp")

# Create main pybind11 module
pybind11_add_module(pynlm ${PYTHON_BINDINGS_SOURCES})

# Target include directories
target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Link with core NLM libraries
target_link_libraries(pynlm PRIVATE
    ${Python_LIBRARIES}
)

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
    target_compile_definitions(pynlm PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Python-specific compile definitions
if(PYBIND11_VERSION_MAJOR VERSION_GREATER 2)
    target_compile_definitions(pynlm PRIVATE PYBIND11_VERSION_MAJOR=${PYBIND11_VERSION_MAJOR})
endif()

# Add custom targets for Python-specific operations
add_custom_target(python_test
    COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_tests.py
    COMMENT "Run Python bindings tests"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_SITEARCH}
    RUNTIME DESTINATION ${Python_SITELIB}
)

# Install Python package data
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${Python_SITEARCH}/nlm
    FILES_MATCHING PATTERN "*.py"
    FILES_MATCHING PATTERN "*.pyi"
)

# Generate Python module documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    doxygen_add_docs(python_docs
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/
        COMMENT "Generate Python bindings documentation"
    )
endif()

set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
