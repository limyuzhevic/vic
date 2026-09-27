# Plugin System CMake Configuration

# Define the plugin system as a separate component
add_library(plugin_system STATIC
    include/nlm/plugin/PluginBase.hpp
    include/nlm/plugin/PluginManager.hpp
    include/nlm/plugin/PluginLoader.hpp
    include/nlm/plugin/PluginRegistry.hpp
    include/nlm/plugin/PluginFactory.hpp
    
    src/plugin/examples/NeuronPlugin.hpp
    src/plugin/examples/NeuronPlugin.cpp
    src/plugin/examples/SynapsePlugin.hpp
    src/plugin/examples/SynapsePlugin.cpp
    src/plugin/examples/MemoryPlugin.hpp
    src/plugin/examples/MemoryPlugin.cpp
    src/plugin/examples/PlasticityPlugin.hpp
    src/plugin/examples/PlasticityPlugin.cpp
    src/plugin/examples/WorldPlugin.hpp
    src/plugin/examples/WorldPlugin.cpp
    
    src/plugin/test/PluginTestSuite.hpp
    src/plugin/test/PluginTestSuite.cpp
    src/plugin/test/PluginValidation.hpp
    src/plugin/test/PluginValidation.cpp
)

# Target include directories
target_include_directories(plugin_system PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard
target_compile_features(plugin_system PUBLIC cxx_std_17)

# Plugin system properties
set_target_properties(plugin_system PROPERTIES
    OUTPUT_NAME "nlm_plugin_system"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Installation rules
install(TARGETS plugin_system
    EXPORT nlmPluginSystemTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/nlm/plugin/
    DESTINATION include/nlm/plugin
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/plugin/examples/
    DESTINATION include/nlm/plugin/examples
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/plugin/test/
    DESTINATION include/nlm/plugin/test
    FILES_MATCHING PATTERN "*.hpp"
)

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmPluginSystemConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlmPluginSystemConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlmPluginSystemConfig.cmake
    INSTALL_DESTINATION lib/cmake/nlmPluginSystem
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmPluginSystemConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlmPluginSystemConfigVersion.cmake
    DESTINATION lib/cmake/nlmPluginSystem
)

# Export targets
export(EXPORT nlmPluginSystemTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/nlmPluginSystemTargets.cmake
)

install(EXPORT nlmPluginSystemTargets
    FILE nlmPluginSystemTargets.cmake
    DESTINATION lib/cmake/nlmPluginSystem
)

# Plugin discovery
file(GLOB PLUGIN_TEMPLATES "cmake/plugin_templates/*.cmake")

# Plugin testing
enable_testing()

add_test(NAME plugin_system_test
    COMMAND plugin_system_test_runner
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Plugin examples
add_executable(neuron_plugin_example
    examples/neuron_plugin_example.cpp
)

target_link_libraries(neuron_plugin_example PRIVATE plugin_system)

add_executable(synapse_plugin_example
    examples/synapse_plugin_example.cpp
)

target_link_libraries(synapse_plugin_example PRIVATE plugin_system)

add_executable(memory_plugin_example
    examples/memory_plugin_example.cpp
)

target_link_libraries(memory_plugin_example PRIVATE plugin_system)

add_executable(plasticity_plugin_example
    examples/plasticity_plugin_example.cpp
)

target_link_libraries(plasticity_plugin_example PRIVATE plugin_system)

add_executable(world_plugin_example
    examples/world_plugin_example.cpp
)

target_link_libraries(world_plugin_example PRIVATE plugin_system)

# Plugin documentation
set(PLUGIN_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/plugin)
file(MAKE_DIRECTORY ${PLUGIN_DOC_DIR})

# Create plugin system documentation
add_custom_target(generate_plugin_docs
    COMMAND doxygen Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating plugin system documentation"
)

# Create plugin examples documentation
add_custom_target(generate_plugin_examples_docs
    COMMAND ${CMAKE_COMMAND} -E echo "Plugin examples documentation"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating plugin examples documentation"
)

# Install documentation
install(DIRECTORY ${PLUGIN_DOC_DIR}/
    DESTINATION doc/plugin
    FILES_MATCHING PATTERN "*.html"
    FILES_MATCHING PATTERN "*.css"
    FILES_MATCHING PATTERN "*.js"
)

# Create plugin configuration templates
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/plugin_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/plugin_config.cmake
    @ONLY
)

# Create plugin testing templates
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/plugin_test.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/plugin_test.cmake
    @ONLY
)

# Plugin development tools
add_executable(plugin_builder
    tools/plugin_builder.cpp
)

target_link_libraries(plugin_builder PRIVATE plugin_system)

add_executable(plugin_validator
    tools/plugin_validator.cpp
)

target_link_libraries(plugin_validator PRIVATE plugin_system)

add_executable(plugin_monitor
    tools/plugin_monitor.cpp
)

target_link_libraries(plugin_monitor PRIVATE plugin_system)

# Plugin packaging
add_custom_target(package_plugin
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/package_plugin.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Packaging plugin system"
)

# Summary
message(STATUS "Plugin system configured")
message(STATUS "  Plugin library: ${CMAKE_CURRENT_BINARY_DIR}/libplugin_system.a")
message(STATUS "  Include directory: ${CMAKE_CURRENT_SOURCE_DIR}/include/nlm/plugin")
message(STATUS "  Plugin examples: ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/examples")
message(STATUS "  Plugin tests: ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/test")