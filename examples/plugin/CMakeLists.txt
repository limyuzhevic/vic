cmake_minimum_required(VERSION 3.16)

# Plugin examples project
project(nlm_plugin_examples VERSION 1.0.0 LANGUAGES CXX)

# Create plugin examples
add_library(plugin_custom_neuron SHARED
    plugin/CustomNeuronPlugin.cpp
)

target_link_libraries(plugin_custom_neuron
    nlm_core
    nlm_neural
)

set_target_properties(plugin_custom_neuron PROPERTIES
    OUTPUT_NAME "nlm_custom_neuron_plugin"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

add_library(plugin_custom_memory SHARED
    plugin/CustomMemoryPlugin.cpp
)

target_link_libraries(plugin_custom_memory
    nlm_core
    nlm_memory
)

set_target_properties(plugin_custom_memory PROPERTIES
    OUTPUT_NAME "nlm_custom_memory_plugin"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

add_library(plugin_custom_visualization SHARED
    plugin/CustomVisualizationPlugin.cpp
)

target_link_libraries(plugin_custom_visualization
    nlm_core
    nlm_visualization
)

set_target_properties(plugin_custom_visualization PROPERTIES
    OUTPUT_NAME "nlm_custom_visualization_plugin"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Installation targets
install(TARGETS plugin_custom_neuron plugin_custom_memory plugin_custom_visualization
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES
    plugin/CustomNeuronPlugin.hpp
    plugin/CustomMemoryPlugin.hpp
    plugin/CustomVisualizationPlugin.hpp
    DESTINATION include/plugin
)

# Documentation
add_custom_target(plugin_docs
    COMMAND ${CMAKE_COMMAND} -E echo "Plugin documentation not implemented yet"
    COMMENT "Building plugin documentation"
)