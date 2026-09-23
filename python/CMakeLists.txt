cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Python bindings should use the existing NLM libraries from the main build
# We find the main NLM libraries instead of recreating them

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Find the NLM libraries that were built in the main project
# This allows us to reuse the existing optimized build
find_library(NLM_CORE_LIB nlm_core PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../build ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH)
find_library(NLM_AGENT_LIB nlm_agent PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../build ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH)
find_library(NLM_WORLD_LIB nlm_world PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../build ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH)

if(NOT NLM_CORE_LIB OR NOT NLM_AGENT_LIB OR NOT NLM_WORLD_LIB)
    message(FATAL_ERROR "NLM libraries not found. Please build the main NLM project first.")
endif()

# Create Python bindings module that links to existing NLM libraries
pybind11_add_module(pynlm python/bindings.cpp)

target_link_libraries(pynlm PRIVATE ${NLM_CORE_LIB} ${NLM_AGENT_LIB} ${NLM_WORLD_LIB})
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add Python-specific optimizations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Install Python module to standard Python site-packages
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    RUNTIME DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
)

# Additional Python packaging support
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../python/__init__.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm/__init__.py
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pynlm/__init__.py
    DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
)