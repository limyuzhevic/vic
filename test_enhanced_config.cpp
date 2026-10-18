#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // Test basic configuration
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.5);
    config.set("memory.capacity", 1024.0);
    config.set("learning.rate", 0.01);
    config.set("brain.enabled", true);
    
    // Test basic getters
    auto neurons = config.getNestedOr<int>("brain.neuron_count", 0);
    auto density = config.getNestedOr<double>("brain.synapse_density", 0.0);
    auto capacity = config.getNestedOr<double>("memory.capacity", 0.0);
    auto learningRate = config.getNestedOr<double>("learning.rate", 0.0);
    auto enabled = config.getNestedOr<bool>("brain.enabled", false);
    
    std::cout << "Basic Configuration Test:" << std::endl;
    std::cout << "  Brain Neurons: " << neurons << std::endl;
    std::cout << "  Synapse Density: " << density << std::endl;
    std::cout << "  Memory Capacity: " << capacity << std::endl;
    std::cout << "  Learning Rate: " << learningRate << std::endl;
    std::cout << "  Brain Enabled: " << (enabled ? "Yes" : "No") << std::endl;
    
    // Test groups
    config.addGroup("brain", "Brain-related configuration");
    config.addGroup("memory", "Memory configuration");
    
    config.addConfigToGroup("brain", "brain.neuron_count");
    config.addConfigToGroup("brain", "brain.synapse_density");
    config.addConfigToGroup("brain", "brain.enabled");
    
    config.addConfigToGroup("memory", "memory.capacity");
    
    // Test group access
    auto brainConfigs = config.getConfigsInGroup("brain");
    auto memoryConfigs = config.getConfigsInGroup("memory");
    
    std::cout << "\nGroup Test:" << std::endl;
    std::cout << "  Brain Group Configs: ";
    for (const auto& key : brainConfigs) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    std::cout << "  Memory Group Configs: ";
    for (const auto& key : memoryConfigs) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    // Test schema validation
    nlm::ConfigSchema schema;
    nlm::ValidationRule neuronRule(nlm::ConfigValueType::Integer, false);
    nlm::ValidationRule densityRule(nlm::ConfigValueType::Double, false);
    nlm::ValidationRule capacityRule(nlm::ConfigValueType::Double, false);
    
    schema.addRule("brain.neuron_count", neuronRule);
    schema.addRule("brain.synapse_density", densityRule);
    schema.addRule("memory.capacity", capacityRule);
    
    config.setSchema(schema);
    
    std::cout << "\nSchema Validation Test:" << std::endl;
    std::cout << "  Schema defined: " << (config.hasSchema() ? "Yes" : "No") << std::endl;
    std::cout << "  All valid: " << (config.validate() ? "Yes" : "No") << std::endl;
    
    // Test snapshot functionality
    std::string snapshotId = config.createSnapshot("Test snapshot");
    std::cout << "\nSnapshot Test:" << std::endl;
    std::cout << "  Created snapshot: " << snapshotId << std::endl;
    
    // Test export to JSON
    std::string jsonData = config.exportToJSON();
    std::cout << "\nJSON Export Test:" << std::endl;
    std::cout << "  JSON size: " << jsonData.size() << " bytes" << std::endl;
    std::cout << "  First 200 chars: " << jsonData.substr(0, 200) << "..." << std::endl;
    
    // Test import from JSON
    nlm::Config config2;
    bool importSuccess = config2.importFromJSON(jsonData);
    std::cout << "\nJSON Import Test:" << std::endl;
    std::cout << "  Import success: " << (importSuccess ? "Yes" : "No") << std::endl;
    
    // Test inheritance
    nlm::Config baseConfig;
    baseConfig.set("brain.base_neurons", 500);
    baseConfig.set("brain.base_synapses", 200);
    
    config.inheritFrom(baseConfig);
    
    std::cout << "\nInheritance Test:" << std::endl;
    std::cout << "  Inherited brain.base_neurons: " << config.getNestedOr<int>("brain.base_neurons", -1) << std::endl;
    std::cout << "  Inherited brain.base_synapses: " << config.getNestedOr<int>("brain.base_synapses", -1) << std::endl;
    
    // Test summary
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    return 0;
}