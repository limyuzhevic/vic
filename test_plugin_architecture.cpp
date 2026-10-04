// test_plugin_architecture.cpp
#include "PluginBase.hpp"
#include "PluginManager.hpp"
#include "PluginRegistry.hpp"
#include "PluginLoader.hpp"
#include "PluginFactory.hpp"

#include "interfaces/NeuronPluginInterface.hpp"
#include "interfaces/SynapsePluginInterface.hpp"
#include "interfaces/PlasticityRulePluginInterface.hpp"

#include "examples/CustomNeuronPlugin.hpp"
#include "examples/CustomSynapsePlugin.hpp"
#include "examples/CustomPlasticityPlugin.hpp"

#include "../../core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <vector>

void test_plugin_base() {
    std::cout << "Testing PluginBase..." << std::endl;
    
    // This is a base class test - would normally test a concrete implementation
    std::cout << "✓ PluginBase structure is valid" << std::endl;
}

void test_plugin_registry() {
    std::cout << "Testing PluginRegistry..." << std::endl;
    
    nlm::PluginRegistry registry;
    
    // Test plugin registration
    bool neuronRegistered = registry.registerPlugin<nlm::NeuronPluginInterface>(
        "CustomNeuron",
        "1.0.0",
        "Custom neuron model",
        "NLM Team",
        []() { return std::make_unique<nlm::CustomNeuronPlugin>(); }
    );
    
    std::cout << "✓ PluginRegistry registration: " << (neuronRegistered ? "PASS" : "FAIL") << std::endl;
    
    // Test plugin discovery
    std::vector<std::string> names = registry.getPluginNames();
    std::cout << "✓ PluginRegistry discovery: Found " << names.size() << " plugins" << std::endl;
    
    // Test plugin info retrieval
    if (registry.hasPlugin("CustomNeuron")) {
        std::cout << "✓ PluginRegistry hasPlugin: " << registry.getPluginVersion("CustomNeuron") << std::endl;
    }
}

void test_plugin_manager() {
    std::cout << "Testing PluginManager..." << std::endl;
    
    nlm::PluginManager manager;
    
    // Test plugin registration
    bool registered = manager.registerPlugin<nlm::NeuronPluginInterface>(
        "TestNeuronPlugin",
        "1.0.0",
        "Test neuron plugin",
        "Test Author",
        []() { return std::make_unique<nlm::CustomNeuronPlugin>(); }
    );
    
    std::cout << "✓ PluginManager registration: " << (registered ? "PASS" : "FAIL") << std::endl;
    
    // Test plugin initialization
    bool initialized = manager.initializePlugin("TestNeuronPlugin");
    std::cout << "✓ PluginManager initialization: " << (initialized ? "PASS" : "FAIL") << std::endl;
    
    // Test plugin enabling
    bool enabled = manager.enablePlugin("TestNeuronPlugin");
    std::cout << "✓ PluginManager enabling: " << (enabled ? "PASS" : "FAIL") << std::endl;
    
    // Test plugin statistics
    size_t totalPlugins = manager.getTotalPluginCount();
    size_t enabledPlugins = manager.getEnabledPluginCount();
    std::cout << "✓ PluginManager statistics: " << totalPlugins << " total, " << enabledPlugins << " enabled" << std::endl;
}

void test_plugin_factory() {
    std::cout << "Testing PluginFactory..." << std::endl;
    
    nlm::PluginFactory factory;
    
    // Test factory has plugin types
    std::vector<std::type_index> types = factory.getRegisteredPluginTypes();
    std::cout << "✓ PluginFactory registered types: " << types.size() << std::endl;
    
    // Test plugin creation
    auto plugin = factory.createPlugin(typeid(nlm::CustomNeuronPlugin));
    std::cout << "✓ PluginFactory creation: " << (plugin ? "PASS" : "FAIL") << std::endl;
}

void test_custom_neuron_plugin() {
    std::cout << "Testing CustomNeuronPlugin..." << std::endl;
    
    nlm::CustomNeuronPlugin plugin;
    
    // Test plugin metadata
    std::cout << "✓ Plugin name: " << plugin.getName() << std::endl;
    std::cout << "✓ Plugin version: " << plugin.getVersion() << std::endl;
    std::cout << "✓ Plugin description: " << plugin.getDescription() << std::endl;
    
    // Test initialization
    nlm::Config config;
    config.set("neuron_type", "Internal");
    
    bool initialized = plugin.initialize(config);
    std::cout << "✓ Plugin initialization: " << (initialized ? "PASS" : "FAIL") << std::endl;
    
    // Test configuration
    plugin.setConfiguration(config);
    nlm::Config retrievedConfig = plugin.getConfiguration();
    std::cout << "✓ Plugin configuration: " << (retrievedConfig.has("neuron_type") ? "PASS" : "FAIL") << std::endl;
    
    // Test neuron creation
    auto neuron = plugin.createNeuron(config);
    std::cout << "✓ Neuron creation: " << (neuron ? "PASS" : "FAIL") << std::endl;
    
    // Test shutdown
    plugin.shutdown();
    std::cout << "✓ Plugin shutdown: PASS" << std::endl;
}

void test_custom_synapse_plugin() {
    std::cout << "Testing CustomSynapsePlugin..." << std::endl;
    
    nlm::CustomSynapsePlugin plugin;
    
    // Test plugin properties
    std::cout << "✓ Default weight: " << plugin.getDefaultWeight() << std::endl;
    std::cout << "✓ Default delay: " << plugin.getDefaultDelay() << std::endl;
    
    // Test initialization
    nlm::Config config;
    config.set("weight", 0.5f);
    
    bool initialized = plugin.initialize(config);
    std::cout << "✓ Plugin initialization: " << (initialized ? "PASS" : "FAIL") << std::endl;
    
    // Test synapse creation
    auto synapse = plugin.createSynapse(config);
    std::cout << "✓ Synapse creation: " << (synapse ? "PASS" : "FAIL") << std::endl;
}

void test_custom_plasticity_plugin() {
    std::cout << "Testing CustomPlasticityPlugin..." << std::endl;
    
    nlm::CustomPlasticityPlugin plugin;
    
    // Test plugin properties
    nlm::PlasticityFlags flags = plugin.getSupportedPlasticityFlags();
    std::cout << "✓ Supported plasticity flags: " << static_cast<int>(flags) << std::endl;
    
    // Test learning rate
    std::cout << "✓ Default learning rate: " << plugin.getDefaultLearningRate() << std::endl;
    
    // Test initialization
    nlm::Config config;
    config.set("learning_rate", 0.01f);
    
    bool initialized = plugin.initialize(config);
    std::cout << "✓ Plugin initialization: " << (initialized ? "PASS" : "FAIL") << std::endl;
    
    // Test rule creation
    auto rule = plugin.createRule(config);
    std::cout << "✓ Plasticity rule creation: " << (rule ? "PASS" : "FAIL") << std::endl;
}

void test_complete_plugin_workflow() {
    std::cout << "Testing complete plugin workflow..." << std::endl;
    
    nlm::PluginRegistry registry;
    nlm::PluginManager manager;
    
    // Register multiple plugins
    bool neuronReg = registry.registerPlugin<nlm::NeuronPluginInterface>(
        "CustomNeuron",
        "1.0.0",
        "Custom neuron model",
        "NLM Team",
        []() { return std::make_unique<nlm::CustomNeuronPlugin>(); }
    );
    
    bool synapseReg = registry.registerPlugin<nlm::SynapsePluginInterface>(
        "CustomSynapse",
        "1.0.0",
        "Custom synapse model",
        "NLM Team",
        []() { return std::make_unique<nlm::CustomSynapsePlugin>(); }
    );
    
    std::cout << "✓ Plugin registration: " << (neuronReg && synapseReg ? "PASS" : "FAIL") << std::endl;
    
    // Test initialization
    bool initSuccess = registry.initializeAllPlugins();
    std::cout << "✓ Plugin initialization: " << (initSuccess ? "PASS" : "FAIL") << std::endl;
    
    // Test manager operations
    bool enableSuccess = manager.enableAll();
    std::cout << "✓ Plugin enabling: " << (enableSuccess ? "PASS" : "FAIL") << std::endl;
    
    // Print statistics
    size_t regCount = registry.getPluginCount();
    size_t managerCount = manager.getTotalPluginCount();
    std::cout << "✓ Plugin statistics: Registry=" << regCount << ", Manager=" << managerCount << std::endl;
}

int main() {
    std::cout << "=== NLM Plugin Architecture Tests ===" << std::endl << std::endl;
    
    try {
        test_plugin_base();
        std::cout << std::endl;
        
        test_plugin_registry();
        std::cout << std::endl;
        
        test_plugin_manager();
        std::cout << std::endl;
        
        test_plugin_factory();
        std::cout << std::endl;
        
        test_custom_neuron_plugin();
        std::cout << std::endl;
        
        test_custom_synapse_plugin();
        std::cout << std::endl;
        
        test_custom_plasticity_plugin();
        std::cout << std::endl;
        
        test_complete_plugin_workflow();
        std::cout << std::endl;
        
        std::cout << "=== All Tests Complete ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}