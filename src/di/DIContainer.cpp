#include "DIContainer.hpp"
#include "core/Config/Config.hpp"
#include "core/Utils.hpp"
#include <fstream>
#include <iostream>

namespace nlm {

bool DIContainer::configureFromJSON(const std::string& configFilepath) {
    // Load JSON configuration
    auto config = std::make_shared<Config>();
    if (!config->load_from_file(configFilepath)) {
        NLM_LOG_ERROR("Failed to load configuration from " + configFilepath);
        return false;
    }
    
    return registerFromConfig(config);
}

bool DIContainer::configureFromYAML(const std::string& configFilepath) {
    // Load YAML configuration
    auto config = std::make_shared<Config>();
    
    // Note: In a real implementation, you would use a YAML parser like yaml-cpp
    // For now, we'll assume the Config class supports YAML loading
    if (!config->load_from_file(configFilepath)) {
        NLM_LOG_ERROR("Failed to load YAML configuration from " + configFilepath);
        return false;
    }
    
    return registerFromConfig(config);
}

void DIContainer::registerFromConfig(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot register from null config");
        return;
    }
    
    // Example configuration-driven registration
    // In a real implementation, you would scan the config for component definitions
    
    // Register plasticity rule factory based on config
    std::string plasticityRuleType = config->get_or<std::string>("plasticity_rule_type", "STDP");
    
    // Register neuromodulator factories
    std::string dopamineType = config->get_or<std::string>("dopamine_type", "Dopamine");
    std::string curiosityType = config->get_or<std::string>("curiosity_type", "Curiosity");
    std::string noveltyType = config->get_or<std::string>("novelty_type", "Novelty");
    std::string predictionErrorType = config->get_or<std::string>("prediction_error_type", "PredictionError");
    
    // Register memory system factories
    std::string workingMemoryType = config->get_or<std::string>("working_memory_type", "NeuralWorkingMemory");
    std::string episodicMemoryType = config->get_or<std::string>("episodic_memory_type", "NeuralEpisodicMemory");
    std::string associativeMemoryType = config->get_or<std::string>("associative_memory_type", "AssociativeMemory");
    
    // Register development system factories
    std::string synaptogenesisType = config->get_or<std::string>("synaptogenesis_type", "Synaptogenesis");
    std::string pruningType = config->get_or<std::string>("pruning_type", "Pruning");
    std::string maturationType = config->get_or<std::string>("maturation_type", "Maturation");
    
    // In a real implementation, these would create the actual factories
    // For now, we'll register placeholder factories
    
    NLM_LOG_INFO("Registered components from configuration");
}

bool DIContainer::loadPlugin(const std::string& pluginPath) {
    // Load dynamic library and extract plugin factory
    // For now, this is a stub implementation
    NLM_LOG_INFO("Loading plugin from " + pluginPath);
    
    // Example: std::dynamic_library lib(pluginPath);
    // PluginFactory factory = lib.get_function("create_plugin");
    // if (factory) {
    //     factory(*this);
    //     return true;
    // }
    
    return false;
}

void DIContainer::registerPlugin(const std::string& name, PluginFactory factory) {
    plugins_[name] = factory;
    NLM_LOG_INFO("Registered plugin: " + name);
}

void DIContainer::startAllComponents() {
    // Start all registered components
    NLM_LOG_INFO("Starting all components");
    // In a real implementation, you would iterate through all components
    // and call their start methods
}

void DIContainer::stopAllComponents() {
    // Stop all registered components
    NLM_LOG_INFO("Stopping all components");
    // In a real implementation, you would iterate through all components
    // and call their stop methods
}

void DIContainer::updateAllComponents(double dt) {
    // Update all registered components
    // In a real implementation, you would iterate through all components
    // and call their update methods
}

std::vector<std::string> DIContainer::getRegisteredComponentTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : componentNames_) {
        types.push_back(pair.second);
    }
    return types;
}

std::string DIContainer::getComponentName(std::type_index typeIndex) const {
    auto it = componentNames_.find(typeIndex);
    if (it != componentNames_.end()) {
        return it->second;
    }
    return "Unknown";
}

} // namespace nlm
