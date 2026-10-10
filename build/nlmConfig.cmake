include(CMakeFindDependencyMacro)

# Find Python dependencies
find_package(Python ${PYTHON_VERSION} REQUIRED COMPONENTS Interpreter Development)

# Find pybind11
find_package(pybind11 REQUIRED)

# Import NLM targets
include("${CMAKE_CURRENT_LIST_DIR}/nlmTargets.cmake")

# Create convenience targets for Python bindings
if(TARGET pynlm_core)
    add_custom_target(pynlm_bind_all
        DEPENDS pynlm_core
        COMMENT "Build all Python bindings"
    )
endif()

# Add Python module search path
set(PYTHON_MODULE_PATH ${Python_SITEARCH}/pynlm)

# Setup Python package metadata
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm.py
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm.py
    DESTINATION ${PYTHON_MODULE_PATH}
)

# Provide find_package for Python bindings
export(PACKAGE pynlm)

# Create package configuration for Python bindings
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake
    INSTALL_DESTINATION ${PYTHON_MODULE_PATH}/cmake
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake
    DESTINATION ${PYTHON_MODULE_PATH}/cmake
)
