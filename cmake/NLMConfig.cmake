# CMake configuration for NLM package

# This file provides package configuration for CMake-based projects
# that want to use NLM as a dependency.

include("${CMAKE_CURRENT_LIST_DIR}/NLMConfigVersion.cmake")

# Import targets
include("${CMAKE_CURRENT_LIST_DIR}/NLMTargets.cmake")

# Additional NLM-specific configuration

# Check if Python bindings are available
set(NLM_PYTHON_BINDINGS_AVAILABLE FALSE CACHE BOOL "Whether Python bindings are built")

if(TARGET pynlm)
    set(NLM_PYTHON_BINDINGS_AVAILABLE TRUE)
endif()

# Set up Python module search path
if(TARGET pynlm)
    # Python module is built
    set(PYTHONPATH "${PYTHONPATH}:${CMAKE_CURRENT_BINARY_DIR}/python" CACHE STRING "Python path for NLM" FORCE)
    set(NLM_PYTHON_MODULE "${CMAKE_CURRENT_BINARY_DIR}/python/pynlm.so")
endif()

# Find Python bindings if needed
find_package(Python3 COMPONENTS Interpreter Development QUIET)

if(FIND_PYTHON_BINDINGS)
    if(Python3_FOUND)
        # Try to locate Python module
        find_path(NLM_PYTHON_INCLUDE_DIR
            NAMES pynlm.h
            PATHS "${CMAKE_INSTALL_PREFIX}/include" "${CMAKE_INSTALL_PREFIX}/include/nlm"
            NO_DEFAULT_PATH
        )
        
        find_library(NLM_PYTHON_LIBRARY
            NAMES pynlm
            PATHS "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/lib/python*"
            NO_DEFAULT_PATH
        )
        
        if(NLM_PYTHON_INCLUDE_DIR AND NLM_PYTHON_LIBRARY)
            set(NLM_PYTHON_BINDINGS_FOUND TRUE)
        endif()
    endif()
endif()

# Macro to find NLM package
define_property(TARGET NLM_TARGETS
    BRIEF_DOCS "NLM library targets"
    FULL_DOCS "Targets exported by NLM package"
    PROPERTY ABSTRACT FALSE
    OWNER GLOBAL
)

# Provide find_package() configuration
if(NOT TARGET NLM::nlm_core)
    add_library(NLM::nlm_core ALIAS nlm_core)
endif()

if(NOT TARGET NLM::nlm_agent)
    add_library(NLM::nlm_agent ALIAS nlm_agent)
endif()

if(NOT TARGET NLM::nlm_world)
    add_library(NLM::nlm_world ALIAS nlm_world)
endif()

if(NOT TARGET NLM::nlm_phase3)
    add_library(NLM::nlm_phase3 ALIAS nlm_phase3)
endif()

if(NOT TARGET NLM::nlm_phase4)
    add_library(NLM::nlm_phase4 ALIAS nlm_phase4)
endif()

if(NOT TARGET NLM::nlm)
    add_executable(NLM::nlm ALIAS nlm)
endif()

if(NLM_PYTHON_BINDINGS_AVAILABLE)
    if(NOT TARGET NLM::pynlm)
        add_library(NLM::pynlm ALIAS pynlm)
    endif()
endif()
