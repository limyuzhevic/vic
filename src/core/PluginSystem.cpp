#include "PluginSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <dlfcn.h>  // For dynamic loading

namespace nlm {

// PluginRegistry implementation
std::unordered_map<std::string, PluginFactory> PluginRegistry::pluginFactories_;
std::unordered_map<std::string, std::unique_ptr<IPlugin>> PluginRegistry::loadedPlugins_;
bool PluginRegistry::pluginsInitialized_ = false;

PluginRegistry::PluginRegistry() {}

PluginRegistry::~PluginRegistry() {
    shutdownAllPlugins();
}

void PluginRegistry::registerPlugin(const std::string& name, PluginFactory factory) {
    if (pluginFactories_.find(name) != pluginFactories_.end()) {
        NLM_LOG_WARNING("Plugin already registered: " + name);
    }
    pluginFactories_[name] = factory;
    NLM_LOG_INFO("Registered plugin: " + name);
}

std::unique_ptr<IPlugin> PluginRegistry::loadPlugin(const std::string& name) {
    if (loadedPlugins_.find(name) != loadedPlugins_.end()) {
        return std::move(loadedPlugins_[name]);
    }
    
    if (pluginFactories_.find(name) == pluginFactories_.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return nullptr;
    }
    
    auto plugin = pluginFactories_[name]();
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin: " + name);
        return nullptr;
    }
    
    if (!plugin->initialize()) {
        NLM_LOG_ERROR("Failed to initialize plugin: " + name);
        return nullptr;
    }
    
    loadedPlugins_[name] = std::move(plugin);
    return std::move(loadedPlugins_[name]);
}

std::vector<std::string> PluginRegistry::getPluginNames() {
    std::vector<std::string> names;
    names.reserve(pluginFactories_.size());
    for (const auto& pair : pluginFactories_) {
        names.push_back(pair.first);
    }
    return names;
}

bool PluginRegistry::isPluginRegistered(const std::string& name) {
    return pluginFactories_.find(name) != pluginFactories_.end();
}

void PluginRegistry::initializeAllPlugins() {
    for (auto& pair : loadedPlugins_) {
        if (!pair.second->isInitialized()) {
            if (pair.second->initialize()) {
                NLM_LOG_INFO("Initialized plugin: " + pair.first);
            } else {
                NLM_LOG_ERROR("Failed to initialize plugin: " + pair.first);
            }
        }
    }
    pluginsInitialized_ = true;
}

void PluginRegistry::shutdownAllPlugins() {
    for (auto& pair : loadedPlugins_) {
        pair.second->shutdown();
    }
    loadedPlugins_.clear();
    pluginsInitialized_ = false;
}

// DefaultBrainRegionPlugin implementation
std::unique_ptr<class NeuralRegion> DefaultBrainRegionPlugin::createRegion(class Brain* brain, class RandomGenerator& rng) {
    // This would create a neural region based on default parameters
    // For now, return nullptr as this is just a skeleton
    return nullptr;
}

std::unordered_map<std::string, std::string> DefaultBrainRegionPlugin::getDefaultConfig() const {
    std::unordered_map<std::string, std::string> config;
    config["neuronCount"] = "100";
    config["connectionProbability"] = "0.1";
    config["regionType"] = "default";
    return config;
}

std::vector<std::string> DefaultBrainRegionPlugin::getCapabilities() const {
    return {"basic_neural_processing", "synaptic_connections", "plasticity"};
}

// DefaultSensoryPopulationPlugin implementation
std::vector<class Neuron*> DefaultSensoryPopulationPlugin::createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) {
    std::vector<class Neuron*> neurons;
    neurons.reserve(count);
    
    // Create sensory neurons with specific properties
    for (size_t i = 0; i < count; ++i) {
        // Create neuron based on brain configuration
        // This is simplified - real implementation would use brain's factory methods
        neurons.push_back(nullptr);  // Placeholder
    }
    
    return neurons;
}

std::unordered_map<std::string, double> DefaultSensoryPopulationPlugin::getProperties() const {
    std::unordered_map<std::string, double> props;
    props["maxCapacity"] = 100.0;
    props["activationThreshold"] = 5.0;
    props["refractoryPeriod"] = 2.0;
    props["plasticityRate"] = 0.1;
    return props;
}

class NeuronType DefaultSensoryPopulationPlugin::getNeuronType() const {
    return NeuronType::Sensory;
}

// DefaultInternalPopulationPlugin implementation
std::vector<class Neuron*> DefaultInternalPopulationPlugin::createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) {
    std::vector<class Neuron*> neurons;
    neurons.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        neurons.push_back(nullptr);  // Placeholder
    }
    
    return neurons;
}

std::unordered_map<std::string, double> DefaultInternalPopulationPlugin::getProperties() const {
    std::unordered_map<std::string, double> props;
    props["maxCapacity"] = 500.0;
    props["activationThreshold"] = 10.0;
    props["refractoryPeriod"] = 5.0;
    props["plasticityRate"] = 0.2;
    return props;
}

class NeuronType DefaultInternalPopulationPlugin::getNeuronType() const {
    return NeuronType::Internal;
}

// DefaultMotorPopulationPlugin implementation
std::vector<class Neuron*> DefaultMotorPopulationPlugin::createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) {
    std::vector<class Neuron*> neurons;
    neurons.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        neurons.push_back(nullptr);  // Placeholder
    }
    
    return neurons;
}

std::unordered_map<std::string, double> DefaultMotorPopulationPlugin::getProperties() const {
    std::unordered_map<std::string, double> props;
    props["maxCapacity"] = 100.0;
    props["activationThreshold"] = 8.0;
    props["refractoryPeriod"] = 3.0;
    props["plasticityRate"] = 0.15;
    return props;
}

class NeuronType DefaultMotorPopulationPlugin::getNeuronType() const {
    return NeuronType::Motor;
}

// DefaultSTDPPlugin implementation
void DefaultSTDPPlugin::apply(class Synapse* synapse, double weightChange) {
    // Apply STDP weight change
    if (synapse) {
        double currentWeight = synapse->getWeight();
        double newWeight = currentWeight + weightChange;
        synapse->setWeight(newWeight);
    }
}

std::unordered_map<std::string, double> DefaultSTDPPlugin::getParameters() const {
    std::unordered_map<std::string, double> params;
    params["learningRate"] = 0.01;
    params["timeConstant"] = 20.0;
    params["maxWeightChange"] = 0.1;
    return params;
}

void DefaultSTDPPlugin::setParameters(const std::unordered_map<std::string, double>& params) {
    // Set parameters - in real implementation, store them for later use
    for (const auto& param : params) {
        NLM_LOG_INFO("STDP parameter set: " + param.first + " = " + std::to_string(param.second));
    }
}

// DefaultHebbianPlugin implementation
void DefaultHebbianPlugin::apply(class Synapse* synapse, double weightChange) {
    // Apply Hebbian weight change
    if (synapse) {
        double currentWeight = synapse->getWeight();
        double newWeight = currentWeight + weightChange;
        synapse->setWeight(newWeight);
    }
}

std::unordered_map<std::string, double> DefaultHebbianPlugin::getParameters() const {
    std::unordered_map<std::string, double> params;
    params["learningRate"] = 0.05;
    params["cooperativity"] = 2.0;
    params["baseline"] = 0.1;
    return params;
}

void DefaultHebbianPlugin::setParameters(const std::unordered_map<std::string, double>& params) {
    // Set parameters - in real implementation, store them for later use
    for (const auto& param : params) {
        NLM_LOG_INFO("Hebbian parameter set: " + param.first + " = " + std::to_string(param.second));
    }
}

// DefaultDopaminePlugin implementation
void DefaultDopaminePlugin::update(class Brain* brain, double timestep) {
    // Update dopamine level based on prediction error
    if (brain) {
        auto* predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            double error = predictionError->getLevel();
            // Simple model: dopamine increases with positive prediction error
            level_ += error * 0.1 * timestep;
            level_ = std::clamp(level_, -1.0, 1.0);
        }
    }
}

double DefaultDopaminePlugin::getLevel() const {
    return level_;
}

double DefaultDopaminePlugin::getEffect(const std::string& target) const {
    if (target == "plasticity") {
        return std::max(0.0, level_) * 2.0;  // Positive effect on plasticity
    } else if (target == "excitability") {
        return 1.0 + level_ * 0.5;  // Modulates neural excitability
    }
    return 0.0;
}

// DefaultCuriosityPlugin implementation
void DefaultCuriosityPlugin::update(class Brain* brain, double timestep) {
    // Update curiosity based on novelty and prediction error
    double novelty = 0.0;  // Would get from novelty detector
    double predictionError = 0.0;  // Would get from prediction error signal
    
    curiosityLevel_ = (novelty * 0.7 + std::abs(predictionError) * 0.3);
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0, 1.0);
}

double DefaultCuriosityPlugin::getLevel() const {
    return curiosityLevel_;
}

double DefaultCuriosityPlugin::getEffect(const std::string& target) const {
    if (target == "exploration") {
        return curiosityLevel_ * 0.8;  // Drives exploration behavior
    }
    return 0.0;
}

// DefaultNoveltyPlugin implementation
void DefaultNoveltyPlugin::update(class Brain* brain, double timestep) {
    // Update novelty based on sensory input changes
    // In real implementation, would compare current sensory input to expected patterns
    noveltyLevel_ = 0.0;  // Placeholder
}

double DefaultNoveltyPlugin::getLevel() const {
    return noveltyLevel_;
}

double DefaultNoveltyPlugin::getEffect(const std::string& target) const {
    if (target == "attention") {
        return noveltyLevel_ * 0.5;  // Captures attention to novel stimuli
    }
    return 0.0;
}

// PluginLoader implementation
class PluginLoader::Impl {
public:
    void* handle;
    std::string filepath;
};

PluginLoader::PluginLoader() : pImpl(std::make_unique<Impl>()) {
    pImpl->handle = nullptr;
    pImpl->filepath = "";
}

PluginLoader::~PluginLoader() {
    if (pImpl->handle) {
        dlclose(pImpl->handle);
    }
}

bool PluginLoader::loadFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Plugin file not found: " + filepath);
        return false;
    }
    
    // Close existing plugin if any
    if (pImpl->handle) {
        dlclose(pImpl->handle);
    }
    
    // Open plugin file
    pImpl->handle = dlopen(filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!pImpl->handle) {
        NLM_LOG_ERROR("Failed to load plugin: " + filepath + ". Error: " + std::string(dlerror()));
        return false;
    }
    
    pImpl->filepath = filepath;
    NLM_LOG_INFO("Loaded plugin from: " + filepath);
    return true;
}

std::unique_ptr<IPlugin> PluginLoader::getPlugin(const std::string& name) const {
    // This is a simplified implementation - in practice would need to map names to loaded symbols
    auto plugin = PluginRegistry::loadPlugin(name);
    return plugin;
}

bool PluginLoader::isPluginLoaded(const std::string& name) const {
    return PluginRegistry::isPluginRegistered(name) && 
           PluginRegistry::loadedPlugins_.find(name) != PluginRegistry::loadedPlugins_.end();
}

std::vector<std::string> PluginLoader::getLoadedPluginNames() const {
    return PluginRegistry::getPluginNames();
}

bool PluginLoader::unloadPlugin(const std::string& name) {
    if (PluginRegistry::loadedPlugins_.find(name) == PluginRegistry::loadedPlugins_.end()) {
        return false;
    }
    
    PluginRegistry::loadedPlugins_[name]->shutdown();
    PluginRegistry::loadedPlugins_.erase(name);
    return true;
}

// Plugin convenience functions
void Plugins::registerBuiltInPlugins() {
    // Register default brain region plugin
    PluginRegistry::registerPlugin("DefaultBrainRegion", 
        []() { return std::make_unique<DefaultBrainRegionPlugin>(); });
    
    // Register neural population plugins
    PluginRegistry::registerPlugin("DefaultSensoryPopulation",
        []() { return std::make_unique<DefaultSensoryPopulationPlugin>(); });
    
    PluginRegistry::registerPlugin("DefaultInternalPopulation",
        []() { return std::make_unique<DefaultInternalPopulationPlugin>(); });
    
    PluginRegistry::registerPlugin("DefaultMotorPopulation",
        []() { return std::make_unique<DefaultMotorPopulationPlugin>(); });
    
    // Register plasticity rule plugins
    PluginRegistry::registerPlugin("DefaultSTDP",
        []() { return std::make_unique<DefaultSTDPPlugin>(); });
    
    PluginRegistry::registerPlugin("DefaultHebbian",
        []() { return std::make_unique<DefaultHebbianPlugin>(); });
    
    // Register neuromodulation plugins
    PluginRegistry::registerPlugin("DefaultDopamine",
        []() { return std::make_unique<DefaultDopaminePlugin>(); });
    
    PluginRegistry::registerPlugin("DefaultCuriosity",
        []() { return std::make_unique<DefaultCuriosityPlugin>(); });
    
    PluginRegistry::registerPlugin("DefaultNovelty",
        []() { return std::make_unique<DefaultNoveltyPlugin>(); });
    
    NLM_LOG_INFO("Registered all built-in plugins");
}

std::unordered_map<std::string, std::string> Plugins::getDefaultBrainRegionConfig(const std::string& regionType) {
    std::unordered_map<std::string, std::string> config;
    
    if (regionType == "sensory") {
        config["neuronCount"] = "50";
        config["connectionProbability"] = "0.05";
        config["regionType"] = "sensory";
    } else if (regionType == "internal") {
        config["neuronCount"] = "200";
        config["connectionProbability"] = "0.1";
        config["regionType"] = "internal";
    } else if (regionType == "motor") {
        config["neuronCount"] = "80";
        config["connectionProbability"] = "0.08";
        config["regionType"] = "motor";
    }
    
    return config;
}

std::unordered_map<std::string, double> Plugins::getDefaultPopulationProperties(const std::string& populationType) {
    std::unordered_map<std::string, double> props;
    
    if (populationType == "sensory") {
        props["maxCapacity"] = 100.0;
        props["activationThreshold"] = 5.0;
        props["refractoryPeriod"] = 2.0;
        props["plasticityRate"] = 0.1;
    } else if (populationType == "internal") {
        props["maxCapacity"] = 500.0;
        props["activationThreshold"] = 10.0;
        props["refractoryPeriod"] = 5.0;
        props["plasticityRate"] = 0.2;
    } else if (populationType == "motor") {
        props["maxCapacity"] = 100.0;
        props["activationThreshold"] = 8.0;
        props["refractoryPeriod"] = 3.0;
        props["plasticityRate"] = 0.15;
    }
    
    return props;
}

std::unordered_map<std::string, double> Plugins::getDefaultNeuromodulationParams(const std::string& modulatorType) {
    std::unordered_map<std::string, double> params;
    
    if (modulatorType == "dopamine") {
        params["baseline"] = 0.0;
        params["learningRate"] = 0.1;
        params["decayRate"] = 0.95;
    } else if (modulatorType == "curiosity") {
        params["baseline"] = 0.0;
        params["noveltySensitivity"] = 0.7;
        params["predictionErrorWeight"] = 0.3;
    }
    
    return params;
}

bool Plugins::validatePluginCompatibility(const IPlugin& plugin, const std::string& brainVersion) {
    // Check plugin version compatibility
    std::string pluginVersion = plugin.getVersion();
    
    // Simple version checking - in practice would use semantic versioning
    if (pluginVersion.empty()) return false;
    
    // For now, assume compatibility
    return true;
}

std::string Plugins::getPluginInfo(const std::string& pluginName) {
    std::ostringstream oss;
    auto plugin = PluginRegistry::loadPlugin(pluginName);
    if (plugin) {
        oss << "Plugin: " << plugin->getName() << "\n";
        oss << "Version: " << plugin->getVersion() << "\n";
        oss << "Description: " << plugin->getDescription() << "\n";
        oss << "Initialized: " << (plugin->isInitialized() ? "Yes" : "No") << "\n";
    } else {
        oss << "Plugin not found: " << pluginName;
    }
    return oss.str();
}

} // namespace nlm
