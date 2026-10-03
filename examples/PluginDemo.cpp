#include "PluginBase.hpp"
#include "STDPPlugin.hpp"
#include "NeuronPlugin.hpp"
#include "SynapsePlugin.hpp"
#include "PluginManager.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Plugin Architecture Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create plugin manager
    PluginManager& manager = PluginManager::getInstance();
    
    // Initialize built-in plugins
    std::cout << "1. Loading built-in plugins..." << std::endl;
    
    // In a real scenario, these would be registered automatically
    // For demo purposes, we'll manually create them
    auto stdpPlugin = std::make_unique<STDPPlugin>();
    auto neuronPlugin = std::make_unique<NeuronPlugin>();
    auto synapsePlugin = std::make_unique<SynapsePlugin>();
    
    // Register plugins
    manager.registerBuiltInPlugin(std::move(stdpPlugin));
    manager.registerBuiltInPlugin(std::move(neuronPlugin));
    manager.registerBuiltInPlugin(std::move(synapsePlugin));
    
    std::cout << "   Available plugins: " << manager.getPluginNames().size() << std::endl;
    
    // Configure plugins
    std::cout << std::endl << "2. Configuring plugins..." << std::endl;
    
    // Global configuration
    std::unordered_map<std::string, std::string> globalConfig;
    globalConfig["neuron_leak_conductance"] = "10.0";
    globalConfig["neuron_refractory_period"] = "5";
    globalConfig["stdp_ltp_weight"] = "0.02";
    globalConfig["stdp_ltd_weight"] = "0.015";
    globalConfig["stdp_tau"] = "20.0";
    
    // Plugin-specific configuration
    std::unordered_map<std::string, std::string> neuronConfig;
    neuronConfig["neuron_leak_conductance"] = "15.0";
    neuronConfig["neuron_refractory_period"] = "8";
    
    std::unordered_map<std::string, std::string> stdpConfig;
    stdpConfig["stdp_ltp_weight"] = "0.025";
    stdpConfig["stdp_ltd_weight"] = "0.018";
    stdpConfig["stdp_tau"] = "25.0";
    
    std::unordered_map<std::string, std::string> synapseConfig;
    synapseConfig["synapse_initial_weight"] = "0.5";
    synapseConfig["synapse_delay"] = "1";
    synapseConfig["synapse_hebbian"] = "true";
    synapseConfig["synapse_stdp"] = "true";
    
    // Initialize all plugins
    std::cout << "3. Initializing plugins..." << std::endl;
    manager.initializePlugin("Neuron_Plugin", neuronConfig);
    manager.initializePlugin("STDP_Plugin", stdpConfig);
    manager.initializePlugin("Synapse_Plugin", synapseConfig);
    
    // Activate plugins
    manager.activatePlugin("Neuron_Plugin");
    manager.activatePlugin("STDP_Plugin");
    manager.activatePlugin("Synapse_Plugin");
    
    // Demonstrate plugin usage
    std::cout << std::endl << "4. Demonstrating plugin functionality..." << std::endl;
    
    // Show plugin statistics
    auto allStats = manager.getAllStats();
    for (const auto& stats : allStats) {
        std::cout << "   Plugin: " << stats.name << std::endl;
        std::cout << "     Neurons: " << stats.neuronCount << std::endl;
        std::cout << "     Synapses: " << stats.synapseCount << std::endl;
        std::cout << "     Memory: " << stats.memoryUsage << " bytes" << std::endl;
        std::cout << "     Active: " << (stats.isActive ? "Yes" : "No") << std::endl;
        std::cout << "     Initialized: " << (stats.isInitialized ? "Yes" : "No") << std::endl;
        std::cout << "     Config: " << stats.neuronCount << std::endl;
    }
    
    // Simulate some updates
    std::cout << std::endl << "5. Simulating updates..." << std::endl;
    manager.startMonitoring();
    
    for (int step = 0; step < 10; ++step) {
        manager.updateAll(0.001); // 1ms timestep
        std::cout << "   Step " << step << " - " << manager.getFPS() << " FPS" << std::endl;
    }
    
    manager.stopMonitoring();
    
    // Show system statistics
    auto systemStats = manager.getSystemStats();
    std::cout << std::endl << "6. System Statistics:" << std::endl;
    std::cout << "   Total Plugins: " << systemStats.totalPlugins << std::endl;
    std::cout << "   Active Plugins: " << systemStats.activePlugins << std::endl;
    std::cout << "   Initialized Plugins: " << systemStats.initializedPlugins << std::endl;
    std::cout << "   Total Memory: " << systemStats.totalMemoryUsage << " bytes" << std::endl;
    std::cout << "   Average Update Time: " << systemStats.averageUpdateTime << " seconds" << std::endl;
    std::cout << "   FPS: " << systemStats.fps << std::endl;
    
    // Demonstrate configuration management
    std::cout << std::endl << "7. Configuration Management Demo:" << std::endl;
    
    // Show global config
    std::cout << "   Global Config:" << std::endl;
    auto globalConfig = manager.getGlobalConfig();
    for (const auto& pair : globalConfig) {
        std::cout << "     " << pair.first << " = " << pair.second << std::endl;
    }
    
    // Merge additional config
    std::unordered_map<std::string, std::string> additionalConfig;
    additionalConfig["simulation_duration"] = "1000.0";
    additionalConfig["learning_rate"] = "0.01";
    
    manager.mergeGlobalConfig(additionalConfig);
    std::cout << "   After merging additional config:" << std::endl;
    globalConfig = manager.getGlobalConfig();
    for (const auto& pair : globalConfig) {
        std::cout << "     " << pair.first << " = " << pair.second << std::endl;
    }
    
    // Reset plugins
    std::cout << std::endl << "8. Resetting plugins..." << std::endl;
    manager.resetAll();
    
    // Shutdown all plugins
    std::cout << "9. Shutting down plugins..." << std::endl;
    manager.shutdownAll();
    
    std::cout << std::endl << "=== Plugin Demo Complete ===" << std::endl;
    return 0;
}
