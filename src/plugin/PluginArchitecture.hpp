// PluginArchitecture.hpp
#pragma once

// Core plugin infrastructure
#include "PluginBase.hpp"
#include "PluginManager.hpp"
#include "PluginRegistry.hpp"
#include "PluginLoader.hpp"
#include "PluginFactory.hpp"

// Plugin interfaces
#include "interfaces/NeuronPluginInterface.hpp"
#include "interfaces/SynapsePluginInterface.hpp"
#include "interfaces/PlasticityRulePluginInterface.hpp"
#include "interfaces/NeuromodulatorPluginInterface.hpp"
#include "interfaces/DevelopmentPluginInterface.hpp"
#include "interfaces/MemoryPluginInterface.hpp"
#include "interfaces/SensoryPluginInterface.hpp"
#include "interfaces/MotorPluginInterface.hpp"
#include "interfaces/EnvironmentPluginInterface.hpp"

// Example plugins
#include "examples/CustomNeuronPlugin.hpp"
#include "examples/CustomSynapsePlugin.hpp"
#include "examples/CustomPlasticityPlugin.hpp"

// Usage example
namespace nlm {

class PluginArchitectureExample {
public:
    static void demonstratePluginArchitecture() {
        // Example 1: Create plugin manager and registry
        PluginManager manager;
        PluginRegistry registry;

        // Example 2: Register custom neuron plugin
        registry.registerPlugin<NeuronPluginInterface>(
            "CustomNeuronPlugin",
            "1.0.0",
            "Extended neuron model with custom dynamics",
            "NLM Team",
            []() { return std::make_unique<CustomNeuronPlugin>(); },
            {} // No dependencies
        );

        // Example 3: Initialize plugins
        registry.initializeAllPlugins();

        // Example 4: Get plugin information
        std::vector<std::string> pluginNames = registry.getPluginNames();

        // Example 5: Create plugins using factory
        PluginFactory factory;
        // Factory would be populated during registration

        std::cout << "Plugin Architecture Example Complete" << std::endl;
    }
};

} // namespace nlm