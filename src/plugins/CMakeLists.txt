# CMakeLists.txt for NLM Plugin System

cmake_minimum_required(VERSION 3.16)
project(NLM_Plugins VERSION 0.1.0 LANGUAGES CXX)

# Plugin system options
option(NLM_BUILD_PLUGINS "Build plugin system" ON)
option(NLM_BUILD_PYTHON_PLUGINS "Build Python plugin support" ON)
option(NLM_ENABLE_DYNAMIC_LOADING "Enable dynamic library loading" ON)

# Plugin directories
set(PLUGIN_INTERFACES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/interfaces")
set(PLUGIN_MANAGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/manager")
set(PLUGIN_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runtime")
set(PLUGIN_PYTHON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/python")

# Include directories for plugin system
include_directories(
    ${PLUGIN_INTERFACES_DIR}
    ${PLUGIN_MANAGER_DIR}
    ${PLUGIN_RUNTIME_DIR}
    ${PLUGIN_PYTHON_DIR}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Plugin interface library
if(NLM_BUILD_PLUGINS)
    add_library(nlm_plugin_interfaces STATIC
        ${PLUGIN_INTERFACES_DIR}/PluginInterface.h
    )
    
    target_compile_features(nlm_plugin_interfaces PUBLIC cxx_std_20)
    set_target_properties(nlm_plugin_interfaces PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# Plugin manager library
if(NLM_BUILD_PLUGINS)
    add_library(nlm_plugin_manager STATIC
        ${PLUGIN_MANAGER_DIR}/PluginManager.h
        ${PLUGIN_MANAGER_DIR}/PluginManager.cpp
    )
    
    target_compile_features(nlm_plugin_manager PUBLIC cxx_std_20)
    target_link_libraries(nlm_plugin_manager PUBLIC nlm_plugin_interfaces)
    set_target_properties(nlm_plugin_manager PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# Plugin runtime library
if(NLM_BUILD_PLUGINS)
    add_library(nlm_plugin_runtime STATIC
        ${PLUGIN_RUNTIME_DIR}/PluginFactory.h
        ${PLUGIN_RUNTIME_DIR}/PluginFactory.cpp
        ${PLUGIN_RUNTIME_DIR}/PluginRegistry.h
        ${PLUGIN_RUNTIME_DIR}/PluginRegistry.cpp
    )
    
    target_compile_features(nlm_plugin_runtime PUBLIC cxx_std_20)
    target_link_libraries(nlm_plugin_runtime PUBLIC nlm_plugin_interfaces nlm_plugin_manager)
    set_target_properties(nlm_plugin_runtime PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# Python plugin loader library
if(NLM_BUILD_PYTHON_PLUGINS AND PYTHON_EXECUTABLE)
    add_library(nlm_python_plugin_loader SHARED
        ${PLUGIN_PYTHON_DIR}/PluginLoader.h
        ${PLUGIN_PYTHON_DIR}/PluginLoader.cpp
        ${PLUGIN_PYTHON_DIR}/PythonPluginWrapper.cpp
    )
    
    target_compile_features(nlm_python_plugin_loader PUBLIC cxx_std_20)
    target_link_libraries(nlm_python_plugin_loader PUBLIC nlm_plugin_interfaces)
    
    # Python dependencies
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    target_link_libraries(nlm_python_plugin_loader PUBLIC Python3::Module)
    
    # Set output name to avoid conflicts
    set_target_properties(nlm_python_plugin_loader PROPERTIES
        OUTPUT_NAME "nlm_python_plugin"
        PREFIX ""
        SUFFIX ".so"
    )
endif()

# Plugin discovery and registration utility
add_executable(plugin_discovery_tool
    ${PLUGIN_RUNTIME_DIR}/PluginDiscoveryTool.cpp
)

if(NLM_BUILD_PLUGINS)
    target_link_libraries(plugin_discovery_tool PUBLIC nlm_plugin_interfaces nlm_plugin_manager)
endif()

# Plugin configuration tool
add_executable(plugin_config_tool
    ${PLUGIN_RUNTIME_DIR}/PluginConfigTool.cpp
)

if(NLM_BUILD_PLUGINS)
    target_link_libraries(plugin_config_tool PUBLIC nlm_plugin_interfaces nlm_plugin_manager)
endif()

# Installation rules
install(TARGETS nlm_plugin_interfaces nlm_plugin_manager nlm_plugin_runtime
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_BUILD_PYTHON_PLUGINS AND PYTHON_EXECUTABLE)
    install(TARGETS nlm_python_plugin_loader
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

install(TARGETS plugin_discovery_tool plugin_config_tool
    RUNTIME DESTINATION bin
)

# Install header files
install(DIRECTORY ${PLUGIN_INTERFACES_DIR}/ DESTINATION include/nlm/plugins/interfaces
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${PLUGIN_MANAGER_DIR}/ DESTINATION include/nlm/plugins/manager
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${PLUGIN_PYTHON_DIR}/ DESTINATION include/nlm/plugins/python
    FILES_MATCHING PATTERN "*.h"
)

# Plugin discovery script
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_discovery.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/plugin_discovery.cmake
    @ONLY
)

# Export configuration for downstream projects
export(EXPORT nlm_plugin_targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/nlm_plugin_targets.cmake"
)

# Plugin system documentation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION docs
)

# Plugin examples
add_subdirectory(examples)

# Testing
enable_testing()
add_subdirectory(tests)

# Summary
message(STATUS "NLM Plugin System Configuration:")
message(STATUS "  - Plugin interfaces: ${NLM_BUILD_PLUGINS}")
message(STATUS "  - Plugin manager: ${NLM_BUILD_PLUGINS}")
message(STATUS "  - Plugin runtime: ${NLM_BUILD_PLUGINS}")
message(STATUS "  - Python plugin support: ${NLM_BUILD_PYTHON_PLUGINS}")
message(STATUS "  - Dynamic loading: ${NLM_ENABLE_DYNAMIC_LOADING}")