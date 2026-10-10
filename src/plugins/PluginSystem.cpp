#include "plugins/PluginSystem.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {

struct PluginRegistry::Impl {
    std::vector<std::unique_ptr<PluginFactory>> factories;
    std::unordered_map<std::string, std::unique_ptr<PluginInterface>> plugins;
    std::unordered_map<std::string, std::unique_ptr<NeuronModelPlugin>> neuronModels;
    std::unordered_map<std::string, std::unique_ptr<PlasticityRulePlugin>> plasticityRules;
    std::unordered_map<std::string, bool> pluginEnabled;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata;
    
    bool initialize() {
        // Register built-in core plugins
        registerCorePlugins();
        return true;
    }
    
    void registerCorePlugins() {
        // Register built-in neuron models
        // In a real implementation, these would be auto-registered via macros
        
        // Register built-in plasticity rules
        // In a real implementation, these would be auto-registered via macros
        
        NLM_LOG_INFO("Registered core plugins");
    }
    
    bool loadPlugin(const std::string& filepath) {
        // This would implement dynamic loading of shared libraries
        // For now, just log that loading is not yet implemented
        NLM_LOG_WARNING("Dynamic plugin loading not yet implemented: " + filepath);
        return false;
    }
    
    std::unique_ptr<PluginInterface> createPlugin(const PluginMetadata& metadata) {
        // Find factory that can create this plugin
        for (const auto& factory : factories) {
            if (factory->canCreate(metadata)) {
                return factory->createPlugin(metadata);
            }
        }
        return nullptr;
    }
    
    std::unique_ptr<NeuronModelPlugin> getNeuronModel(const std::string& name) const {
        auto it = neuronModels.find(name);
        if (it != neuronModels.end()) {
            return std::make_unique<NeuronModelPlugin>(*it->second);
        }
        return nullptr;
    }
    
    std::unique_ptr<PlasticityRulePlugin> getPlasticityRule(const std::string& name) const {
        auto it = plasticityRules.find(name);
        if (it != plasticityRules.end()) {
            return std::make_unique<PlasticityRulePlugin>(*it->second);
        }
        return nullptr;
    }
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialize();
}

PluginRegistry::~PluginRegistry() = default;

void PluginRegistry::registerFactory(std::unique_ptr<PluginFactory> factory) {
    pImpl->factories.push_back(std::move(factory));
}

void PluginRegistry::registerCorePlugin(std::unique_ptr<PluginInterface> plugin) {
    auto& metadata = plugin->getMetadata();
    pImpl->plugins[metadata.name] = std::move(plugin);
    pImpl->pluginEnabled[metadata.name] = true;
    pImpl->pluginMetadata[metadata.name] = metadata;
    
    // Register by type
    if (auto* neuronModel = dynamic_cast<NeuronModelPlugin*>(pImpl->plugins[metadata.name].get())) {
        pImpl->neuronModels[metadata.name] = std::make_unique<NeuronModelPlugin>(*neuronModel);
    }
    else if (auto* plasticityRule = dynamic_cast<PlasticityRulePlugin*>(pImpl->plugins[metadata.name].get())) {
        pImpl->plasticityRules[metadata.name] = std::make_unique<PlasticityRulePlugin>(*plasticityRule);
    }
}

bool PluginRegistry::loadPlugin(const std::string& filepath) {
    return pImpl->loadPlugin(filepath);
}

bool PluginRegistry::unloadPlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    // Shutdown plugin
    if (it->second->isInitialized()) {
        it->second->shutdown();
    }
    
    // Remove from registry
    pImpl->plugins.erase(it);
    pImpl->pluginEnabled.erase(pluginName);
    pImpl->pluginMetadata.erase(pluginName);
    
    // Remove by type
    pImpl->neuronModels.erase(pluginName);
    pImpl->plasticityRules.erase(pluginName);
    
    NLM_LOG_INFO("Unloaded plugin: " + pluginName);
    return true;
}

std::unique_ptr<PluginInterface> PluginRegistry::getPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return nullptr;
    }
    
    if (!pImpl->pluginEnabled.at(name)) {
        return nullptr;
    }
    
    // Create a copy for the caller
    if (auto* neuronModel = dynamic_cast<NeuronModelPlugin*>(it->second.get())) {
        return std::make_unique<NeuronModelPlugin>(*neuronModel);
    }
    else if (auto* plasticityRule = dynamic_cast<PlasticityRulePlugin*>(it->second.get())) {
        return std::make_unique<PlasticityRulePlugin>(*plasticityRule);
    }
    else {
        return std::make_unique<PluginInterface>(*it->second.get());
    }
}

std::vector<std::unique_ptr<PluginInterface>> PluginRegistry::getAllPlugins() const {
    std::vector<std::unique_ptr<PluginInterface>> result;
    for (const auto& pair : pImpl->plugins) {
        if (pImpl->pluginEnabled.at(pair.first)) {
            if (auto* neuronModel = dynamic_cast<NeuronModelPlugin*>(pair.second.get())) {
                result.push_back(std::make_unique<NeuronModelPlugin>(*neuronModel));
            }
            else if (auto* plasticityRule = dynamic_cast<PlasticityRulePlugin*>(pair.second.get())) {
                result.push_back(std::make_unique<PlasticityRulePlugin>(*plasticityRule));
            }
            else {
                result.push_back(std::make_unique<PluginInterface>(*pair.second.get()));
            }
        }
    }
    return result;
}

std::vector<std::unique_ptr<PluginInterface>> PluginRegistry::getPluginsByType(PluginType type) const {
    std::vector<std::unique_ptr<PluginInterface>> result;
    
    for (const auto& pair : pImpl->plugins) {
        if (pImpl->pluginEnabled.at(pair.first)) {
            const auto& metadata = pImpl->pluginMetadata.at(pair.first);
            if (metadata.type == type) {
                if (auto* neuronModel = dynamic_cast<NeuronModelPlugin*>(pair.second.get())) {
                    result.push_back(std::make_unique<NeuronModelPlugin>(*neuronModel));
                }
                else if (auto* plasticityRule = dynamic_cast<PlasticityRulePlugin*>(pair.second.get())) {
                    result.push_back(std::make_unique<PlasticityRulePlugin>(*plasticityRule));
                }
                else {
                    result.push_back(std::make_unique<PluginInterface>(*pair.second.get()));
                }
            }
        }
    }
    return result;
}

void PluginRegistry::setPluginEnabled(const std::string& name, bool enabled) {
    pImpl->pluginEnabled[name] = enabled;
}

bool PluginRegistry::isPluginEnabled(const std::string& name) const {
    auto it = pImpl->pluginEnabled.find(name);
    return it != pImpl->pluginEnabled.end() && it->second;
}

bool PluginRegistry::initializeAllPlugins() {
    bool allSuccess = true;
    for (auto& pair : pImpl->plugins) {
        if (pImpl->pluginEnabled.at(pair.first) && !pair.second->isInitialized()) {
            if (!pair.second->initialize()) {
                NLM_LOG_ERROR("Failed to initialize plugin: " + pair.first);
                allSuccess = false;
            }
        }
    }
    return allSuccess;
}

void PluginRegistry::shutdownAllPlugins() {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->shutdown();
        }
    }
}

std::vector<std::unique_ptr<NeuronModelPlugin>> PluginRegistry::getNeuronModels() const {
    std::vector<std::unique_ptr<NeuronModelPlugin>> result;
    for (const auto& pair : pImpl->neuronModels) {
        if (pImpl->pluginEnabled.at(pair.first)) {
            result.push_back(std::make_unique<NeuronModelPlugin>(*pair.second));
        }
    }
    return result;
}

std::vector<std::unique_ptr<PlasticityRulePlugin>> PluginRegistry::getPlasticityRules() const {
    std::vector<std::unique_ptr<PlasticityRulePlugin>> result;
    for (const auto& pair : pImpl->plasticityRules) {
        if (pImpl->pluginEnabled.at(pair.first)) {
            result.push_back(std::make_unique<PlasticityRulePlugin>(*pair.second));
        }
    }
    return result;
}

std::unique_ptr<Neuron> PluginRegistry::createNeuron(const std::string& modelName, NeuronId id) {
    auto model = getNeuronModel(modelName);
    if (model) {
        return model->createNeuron(id);
    }
    return nullptr;
}

void PluginRegistry::applyPlasticityRule(
    std::unique_ptr<Synapse>& synapse,
    const std::string& ruleName,
    const std::vector<Timestamp>& preSpikeHistory,
    const std::vector<Timestamp>& postSpikeHistory,
    float learningRate,
    float timeConstant
) {
    auto rule = getPlasticityRule(ruleName);
    if (rule) {
        rule->updateSynapticWeight(
            synapse->getWeight(),
            preSpikeHistory,
            postSpikeHistory,
            learningRate,
            timeConstant
        );
        synapse->setWeight(synapse->getWeight());
    }
}

} // namespace nlm
