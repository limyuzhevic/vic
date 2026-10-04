// PluginSystemMain.cpp
#include "PluginArchitecture.hpp"
#include <iostream>

void demonstratePluginArchitecture() {
    std::cout << "=== NLM Plugin Architecture Demonstration ===" << std::endl << std::endl;

    // 1. Create Plugin System
    nlm::PluginSystem pluginSystem;
    pluginSystem.initialize();

    // 2. Get access to plugin management components
    auto& manager = nlm::PluginSystem::getManager();
    auto& registry = nlm::PluginSystem::getRegistry();
    auto& factory = nlm::PluginSystem::getFactory();
    auto& loader = nlm::PluginSystem::getLoader();

    // 3. Register example plugins
    std::cout << "Registering plugins..." << std::endl;

    bool neuronReg = registry.registerPlugin<nlm::NeuronPluginInterface>(
        "CustomNeuronPlugin",
        "1.0.0",
        "Extended neuron model with custom dynamics",
        "NLM Team",
        []() { return std::make_unique<nlm::CustomNeuronPlugin>(); }
    );

    bool synapseReg = registry.registerPlugin<nlm::SynapsePluginInterface>(
        "CustomSynapsePlugin",
        "1.0.0",
        "Extended synapse model with custom dynamics",
        "NLM Team",
        []() { return std::make_unique<nlm::CustomSynapsePlugin>(); }
    );

    bool plasticityReg = registry.registerPlugin<nlm::PlasticityRulePluginInterface>(
        "CustomPlasticityPlugin",
        "1.0.0",
        "Extended plasticity rules with custom STDP and Hebbian dynamics",
        "NLM Team",
        []() { return std::make_unique<nlm::CustomPlasticityPlugin>(); }
    );

    std::cout << "✓ Registered " << (neuronReg + synapseReg + plasticityReg) << " plugins" << std::endl;

    // 4. Get available plugins
    std::vector<std::string> availablePlugins = registry.getPluginNames();
    std::cout << "\nAvailable plugins: " << availablePlugins.size() << std::endl;
    for (const auto& name : availablePlugins) {
        std::cout << "  - " << name << " (" << registry.getPluginVersion(name) << ")" << std::endl;
        std::cout << "    Description: " << registry.getPluginDescription(name) << std::endl;
        std::cout << "    Author: " << registry.getPluginAuthor(name) << std::endl << std::endl;
    }

    // 5. Test plugin manager
    std::cout << "Testing PluginManager..." << std::endl;

    bool initNeuron = manager.initializePlugin("CustomNeuronPlugin");
    bool initSynapse = manager.initializePlugin("CustomSynapsePlugin");
    bool initPlasticity = manager.initializePlugin("CustomPlasticityPlugin");

    std::cout << "✓ Initialized plugins: " << (initNeuron + initSynapse + initPlasticity) << std::endl;

    bool enableNeuron = manager.enablePlugin("CustomNeuronPlugin");
    bool enableSynapse = manager.enablePlugin("CustomSynapsePlugin");
    bool enablePlasticity = manager.enablePlugin("CustomPlasticityPlugin");

    std::cout << "✓ Enabled plugins: " << (enableNeuron + enableSynapse + enablePlasticity) << std::endl;

    // 6. Test individual plugins
    std::cout << "\nTesting individual plugins..." << std::endl;

    // Test CustomNeuronPlugin
    nlm::CustomNeuronPlugin neuronPlugin;
    nlm::Config neuronConfig;
    neuronConfig.set("neuron_type", "Custom");
    neuronConfig.set("plasticity_enabled", true);

    bool neuronInitialized = neuronPlugin.initialize(neuronConfig);
    std::cout << "✓ CustomNeuronPlugin initialized: " << (neuronInitialized ? "YES" : "NO") << std::endl;
    std::cout << "  Default neuron type: " << neuronPlugin.getDefaultNeuronType() << std::endl;
    std::cout << "  Plugin name: " << neuronPlugin.getName() << std::endl;

    // Test CustomSynapsePlugin
    nlm::CustomSynapsePlugin synapsePlugin;
    nlm::Config synapseConfig;
    synapseConfig.set("weight", 0.75f);
    synapseConfig.set("plasticity_enabled", true);

    bool synapseInitialized = synapsePlugin.initialize(synapseConfig);
    std::cout << "✓ CustomSynapsePlugin initialized: " << (synapseInitialized ? "YES" : "NO") << std::endl;
    std::cout << "  Default weight: " << synapsePlugin.getDefaultWeight() << std::endl;
    std::cout << "  Default delay: " << synapsePlugin.getDefaultDelay() << std::endl;

    // Test CustomPlasticityPlugin
    nlm::CustomPlasticityPlugin plasticityPlugin;
    nlm::Config plasticityConfig;
    plasticityConfig.set("learning_rate", 0.05f);
    plasticityConfig.set("stdp_time_constant", 15.0f);

    bool plasticityInitialized = plasticityPlugin.initialize(plasticityConfig);
    std::cout << "✓ CustomPlasticityPlugin initialized: " << (plasticityInitialized ? "YES" : "NO") << std::endl;
    std::cout << "  Default learning rate: " << plasticityPlugin.getDefaultLearningRate() << std::endl;
    std::cout << "  Supported plasticity flags: " << static_cast<int>(plasticityPlugin.getSupportedPlasticityFlags()) << std::endl;

    // 7. Test factory pattern
    std::cout << "\nTesting PluginFactory..." << std::endl;
    std::vector<std::type_index> factoryTypes = factory.getRegisteredPluginTypes();
    std::cout << "✓ Factory registered types: " << factoryTypes.size() << std::endl;

    // 8. Test loader
    std::cout << "\nTesting PluginLoader..." << std::endl;
    bool loadSuccess = loader.loadAllPluginsFromDirectory(".");
    std::cout << "✓ Plugin loader status: " << (loadSuccess ? "Success" : "Failed") << std::endl;

    // 9. Test plugin system statistics
    std::cout << "\nPlugin System Statistics:" << std::endl;
    std::cout << "  Total plugins: " << manager.getTotalPluginCount() << std::endl;
    std::cout << "  Enabled plugins: " << manager.getEnabledPluginCount() << std::endl;
    std::cout << "  Initialized plugins: " << manager.getInitializedPluginCount() << std::endl;

    // 10. Demonstrate plugin configuration
    std::cout << "\nPlugin Configuration Test:" << std::endl;
    bool configSuccess = manager.setPluginConfiguration<nlm::NeuronPluginInterface>(
        "CustomNeuronPlugin", neuronConfig
    );
    std::cout << "✓ Configuration set: " << (configSuccess ? "SUCCESS" : "FAILED") << std::endl;

    // 11. Demonstrate plugin validation
    std::cout << "\nPlugin Validation Test:" << std::endl;
    bool validateSuccess = manager.validatePlugin<nlm::SynapsePluginInterface>("CustomSynapsePlugin");
    std::cout << "✓ Plugin validation: " << (validateSuccess ? "PASSED" : "FAILED") << std::endl;

    // 12. Cleanup
    std::cout << "\nCleanup..." << std::endl;
    manager.shutdownAll();
    pluginSystem.shutdown();

    std::cout << "\n=== Plugin Architecture Demonstration Complete ===" << std::endl;
}

int main() {
    try {
        demonstratePluginArchitecture();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}