# Plugin System CMakeLists.txt

# Plugin directories
file(GLOB PLUGIN_SOURCES 
    "src/plugin/PluginRegistry.cpp"
    "src/plugin/PluginManager.cpp"
)

# Create plugin library
add_library(nlm_plugin STATIC ${PLUGIN_SOURCES})

target_include_directories(nlm_plugin PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(nlm_plugin PUBLIC nlm_core)

# Example memory plugin
file(GLOB MEMORY_PLUGIN_SOURCES "src/plugin/examples/MemoryPlugin.cpp")
if(MEMORY_PLUGIN_SOURCES)
    add_executable(memory_plugin ${MEMORY_PLUGIN_SOURCES})
    target_link_libraries(memory_plugin nlm_plugin nlm_core)
    install(TARGETS memory_plugin DESTINATION bin)
endif()

# Example plasticity plugin
file(GLOB PLASTICITY_PLUGIN_SOURCES "src/plugin/examples/PlasticityPlugin.cpp")
if(PLASTICITY_PLUGIN_SOURCES)
    add_executable(plasticity_plugin ${PLASTICITY_PLUGIN_SOURCES})
    target_link_libraries(plasticity_plugin nlm_plugin nlm_core)
    install(TARGETS plasticity_plugin DESTINATION bin)
endif()

# Plugin utilities
add_executable(plugin_utils "src/plugin/utils/PluginLoader.cpp")
add_executable(plugin_utils "src/plugin/utils/PluginConfig.cpp")

target_link_libraries(plugin_utils nlm_plugin nlm_core)
install(TARGETS plugin_utils DESTINATION bin)