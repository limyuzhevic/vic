cmake_minimum_required(VERSION 3.20)

# Python bindings for NLM

project(pynlm_bindings
    LANGUAGES CXX
    DESCRIPTION "Python bindings for NLM neural simulation framework"
)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Use NLM's configuration
add_subdirectory(.. NLMProject)

# Include pybind11 macros
set(pybind11_DIR "")
if(DEFINED ENV{pybind11_DIR})
    set(pybind11_DIR "$ENV{pybind11_DIR}")
endif()

# Configure pybind11
include(pybind11)

# Create Python module
pybind11_add_module(pynlm
    bindings.cpp
)

# Link with NLM libraries
target_link_libraries(pynlm
    PRIVATE
        NLM::nlm_core
        NLM::nlm_agent
        NLM::nlm_world
)

target_include_directories(pynlm
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Platform-specific settings
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Python module installation
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_ARCHIVE}
    RUNTIME DESTINATION ${Python_RUNTIME}
)

# Installation configuration files
install(EXPORT NLMPythonTargets
    FILE NLMPythonTargets.cmake
    NAMESPACE NLM::Python::
    DESTINATION ${Python_SITEARCH}/cmake/NLM
)

# Generate and install Python configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMPythonConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMPythonConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMPythonConfig.cmake"
    INSTALL_DESTINATION ${Python_SITEARCH}/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMPythonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMPythonConfigVersion.cmake"
    DESTINATION ${Python_SITEARCH}/cmake/NLM
)

# Generate Python-specific build information
function(generate_python_build_info target_name)
    set(BUILD_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/python_build_info/${target_name}_build_info.txt")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python_build_info")
    
    file(WRITE ${BUILD_INFO_FILE}
        "NLM Python Bindings Build Information\n"
        "====================================\n\n"
        "Project: NLM\n"
        "Python version: ${Python_VERSION}\n"
        "pybind11 version: ${pybind11_VERSION}\n"
        "Build type: ${CMAKE_BUILD_TYPE}\n"
        "Compiler: ${CMAKE_CXX_COMPILER_ID}\n"
        "Build time: ${CMAKE_TIMESTAMP}\n\n"
        "Options:\n"
        "  NLM_BUILD_PYTHON: ${NLM_BUILD_PYTHON}\n"
        "  NLM_ENABLE_EXPERIMENTS: ${NLM_ENABLE_EXPERIMENTS}\n"
        "  BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}\n"
    )
    
    add_custom_target(${target_name}_python_build_info ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Python build info generated: ${BUILD_INFO_FILE}"
        COMMENT "Generating Python build information for ${target_name}"
    )
endfunction()

generate_python_build_info("pynlm")

# Custom target for building Python bindings
target_compile_features(pynlm PRIVATE cxx_std_20)

# Python compatibility
if(Python_VERSION_MAJOR EQUAL 2)
    target_compile_definitions(pynlm PRIVATE PYBIND11_DECLARE_MODULE=pynlm)
elseif(Python_VERSION_MAJOR EQUAL 3)
    target_compile_definitions(pynlm PRIVATE PYBIND11_DECLARE_MODULE=pynlm)
endif()

# Add test for Python bindings if enabled
if(NLM_BUILD_TESTS)
    enable_testing()
    add_test(NAME test_pynlm
        COMMAND ${Python_EXECUTABLE} -c "import pynlm"
    )
endif()
