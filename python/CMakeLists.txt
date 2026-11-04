# Python module build for enhanced NLM bindings

cmake_minimum_required(VERSION 3.16)
project(nlm_python_bindings)

# Python bindings (Phase 5: enhanced version)
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" ON)

if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    # The module is shared, so the static libs it links need PIC.
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    
    # Enhanced Python bindings (includes both basic and enhanced APIs)
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    
    # Add enhanced bindings if available
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/python/EnhancedPythonBindings.cpp)
        target_sources(pynlm PRIVATE python/EnhancedPythonBindings.cpp)
        target_sources(pynlm PRIVATE python/EnhancedPythonBindings_Impl.cpp)
        target_sources(pynlm PRIVATE python/EnhancedPythonBindings_Fwd.hpp)
    endif()
    
    install(TARGETS pynlm LIBRARY DESTINATION .)
endif()