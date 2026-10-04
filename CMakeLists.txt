# CMakeLists.txt for NLM Plugin System

# Plugin architecture add-on to main CMakeLists.txt

# Plugin library
add_library(nlm_plugin STATIC
    src/plugin/PluginBase.cpp
    src/plugin/PluginManager.cpp
    src/plugin/PluginRegistry.cpp
    src/plugin/PluginLoader.cpp
    src/plugin/PluginFactory.cpp
    
    # Interface implementations
    src/plugin/interfaces/NeuronPluginInterface.cpp
    src/plugin/interfaces/SynapsePluginInterface.cpp
    src/plugin/interfaces/PlasticityRulePluginInterface.cpp
    src/plugin/interfaces/NeuromodulatorPluginInterface.cpp
    src/plugin/interfaces/DevelopmentPluginInterface.cpp
    src/plugin/interfaces/MemoryPluginInterface.cpp
    src/plugin/interfaces/SensoryPluginInterface.cpp
    src/plugin/interfaces/MotorPluginInterface.cpp
    src/plugin/interfaces/EnvironmentPluginInterface.cpp
    
    # Example plugins
    src/plugin/examples/CustomNeuronPlugin.cpp
    src/plugin/examples/CustomSynapsePlugin.cpp
    src/plugin/examples/CustomPlasticityPlugin.cpp
)

target_include_directories(nlm_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugin>
    $<INSTALL_INTERFACE:include/plugin>
)

target_link_libraries(nlm_plugin PUBLIC nlm_core)

# Plugin demo executable
add_executable(nlm_plugin_demo
    test_plugin_architecture.cpp
)

target_link_libraries(nlm_plugin_demo PUBLIC nlm_plugin nlm_core)

# Install plugin targets
install(TARGETS nlm_plugin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/plugin/interfaces/ DESTINATION include/plugin/interfaces
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/plugin/examples/ DESTINATION include/plugin/examples
    FILES_MATCHING PATTERN "*.hpp"
)

install(FILES src/plugin/PluginArchitecture.hpp
    DESTINATION include/plugin
)