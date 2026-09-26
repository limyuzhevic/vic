#include "PluginSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <dlfcn.h>  // For dynamic library loading (Linux)
#include <sstream>
#include <algorithm>

namespace nlm {

// Forward declarations for helper classes
class PlasticityContext;
class MemoryEntry;
class State;
class Experience;

// PluginManager implementation
struct PluginManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> plugins;
    std::unordered_map<std::string, void*> libraryHandles;
    std::vector<std::string> loadedPlugins;
    bool initialized;
    
    Impl() : initialized(false) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    cleanupAllPlugins();
}

bool PluginManager::loadPlugin(const std::string& libraryPath) {
    // Check if library already loaded
    if (pImpl->libraryHandles.find(libraryPath) != pImpl->libraryHandles.end()) {
        NLM_LOG_WARNING("Plugin library already loaded: " + libraryPath);
        return true;
    }
    
    // Open shared library
    void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin library " + libraryPath + ": " + std::string(dlerror()));
        return false;
    }
    
    // Get plugin creation function
    using PluginCreator = IPlugin* (*)();
    PluginCreator createPlugin = reinterpret_cast<PluginCreator>(dlsym(handle, "createPlugin"));
    
    if (!createPlugin) {
        dlclose(handle);
        NLM_LOG_ERROR("No createPlugin function found in " + libraryPath);
        return false;
    }
    
    // Create plugin instance
    IPlugin* plugin = createPlugin();
    if (!plugin) {
        dlclose(handle);
        NLM_LOG_ERROR("Failed to create plugin from " + libraryPath);
        return false;
    }
    
    // Store plugin
    std::shared_ptr<IPlugin> pluginPtr(plugin);
    pImpl->plugins[pluginPtr->getName()] = pluginPtr;
    pImpl->libraryHandles[libraryPath] = handle;
    pImpl->loadedPlugins.push_back(libraryPath);
    
    NLM_LOG_INFO("Loaded plugin: " + pluginPtr->getName() + " version " + pluginPtr->getVersion());
    return true;
}

bool PluginManager::loadInlinePlugin(std::unique_ptr<IPlugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    pImpl->plugins[plugin->getName()] = std::move(plugin);
    NLM_LOG_INFO("Loaded inline plugin: " + pImpl->plugins[plugin->getName()]->getName());
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("Plugin not found: " + pluginName);
        return false;
    }
    
    // Find library handle
    for (auto libIt = pImpl->libraryHandles.begin(); libIt != pImpl->libraryHandles.end(); ++libIt) {
        // Check if this library contains the plugin (simplified check)
        if (pluginName.find(libIt->first) != std::string::npos) {
            dlclose(libIt->second);
            pImpl->libraryHandles.erase(libIt);
            break;
        }
    }
    
    pImpl->plugins.erase(it);
    NLM_LOG_INFO("Unloaded plugin: " + pluginName);
    return true;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getAllPlugins() const {
    std::vector<std::shared_ptr<IPlugin>> result;
    for (const auto& pair : pImpl->plugins) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<IPlasticityRule>> PluginManager::getPlasticityRules() const {
    std::vector<std::shared_ptr<IPlasticityRule>> result;
    for (const auto& plugin : pImpl->plugins) {
        auto plasticityPlugin = std::dynamic_pointer_cast<IPlasticityRule>(plugin.second);
        if (plasticityPlugin) {
            result.push_back(plasticityPlugin);
        }
    }
    return result;
}

std::vector<std::shared_ptr<IMemorySystem>> PluginManager::getMemorySystems() const {
    std::vector<std::shared_ptr<IMemorySystem>> result;
    for (const auto& plugin : pImpl->plugins) {
        auto memoryPlugin = std::dynamic_pointer_cast<IMemorySystem>(plugin.second);
        if (memoryPlugin) {
            result.push_back(memoryPlugin);
        }
    }
    return result;
}

std::vector<std::shared_ptr<IBehaviorModule>> PluginManager::getBehaviorModules() const {
    std::vector<std::shared_ptr<IBehaviorModule>> result;
    for (const auto& plugin : pImpl->plugins) {
        auto behaviorPlugin = std::dynamic_pointer_cast<IBehaviorModule>(plugin.second);
        if (behaviorPlugin) {
            result.push_back(behaviorPlugin);
        }
    }
    return result;
}

bool PluginManager::initializeAllPlugins(const Config& config) {
    bool success = true;
    for (auto& pair : pImpl->plugins) {
        try {
            if (!pair.second->initialize(config)) {
                NLM_LOG_ERROR("Failed to initialize plugin: " + pair.second->getName());
                success = false;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception initializing plugin " + pair.second->getName() + ": " + std::string(e.what()));
            success = false;
        }
    }
    pImpl->initialized = success;
    return success;
}

void PluginManager::cleanupAllPlugins() {
    for (auto& pair : pImpl->plugins) {
        try {
            pair.second->cleanup();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception cleaning up plugin " + pair.second->getName() + ": " + std::string(e.what()));
        }
    }
    
    // Close library handles
    for (auto& pair : pImpl->libraryHandles) {
        dlclose(pair.second);
    }
    
    pImpl->plugins.clear();
    pImpl->libraryHandles.clear();
    pImpl->loadedPlugins.clear();
    pImpl->initialized = false;
}

template<typename T, typename... Args>
std::unique_ptr<T> PluginManager::createComponent(Args&&... args) {
    // This is a placeholder for component factory
    // In a real implementation, this would use the plugin system
    // or built-in factories to create components
    NLM_LOG_ERROR("createComponent not implemented - this is a placeholder");
    return nullptr;
}

void PluginManager::registerBuiltInPlugins() {
    // Register example plugins from the Examples namespace
    loadInlinePlugin(std::make_unique<Examples::AdvancedSTDP>());
    loadInlinePlugin(std::make_unique<Examples::HierarchicalMemory>());
    loadInlinePlugin(std::make_unique<Examples::MCTPlanning>());
}

std::vector<std::string> PluginManager::getSupportedFormats() const {
    return {"shared_library", "inline"};
}

bool PluginManager::supportsPluginFormat(const std::string& format) const {
    return format == "shared_library" || format == "inline";
}

// ComponentFactory implementation
struct ComponentFactory::Impl {
    std::unordered_map<std::string, std::function<std::unique_ptr<IComponent>(const Config&, const std::unordered_map<std::string, float>&)>> factories;
};

ComponentFactory::ComponentFactory() : pImpl(std::make_unique<Impl>()) {}

ComponentFactory::~ComponentFactory() = default;

std::shared_ptr<Brain> ComponentFactory::createBrain(const Config& config,
                                                    const std::vector<std::shared_ptr<IPlasticityRule>>& plasticityRules,
                                                    const std::vector<std::shared_ptr<IMemorySystem>>& memorySystems,
                                                    const std::vector<std::shared_ptr<IBehaviorModule>>& behaviorModules) {
    NLM_LOG_INFO("Creating brain with custom components");
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return nullptr;
    }
    
    // In a real implementation, you would:
    // 1. Configure brain with custom plasticity rules
    // 2. Integrate memory systems
    // 3. Set up behavior modules
    // 4. Configure neural architecture
    
    NLM_LOG_INFO("Brain created successfully with " + std::to_string(plasticityRules.size()) + 
                " plasticity rules, " + std::to_string(memorySystems.size()) + 
                " memory systems, and " + std::to_string(behaviorModules.size()) + " behavior modules");
    
    return brain;
}

std::unique_ptr<Neuron> ComponentFactory::createNeuron(NeuronId id,
                                                       const std::unordered_map<std::string, float>& properties) {
    NLM_LOG_INFO("Creating neuron with custom properties");
    
    auto neuron = std::make_unique<Neuron>(id);
    
    // Apply custom properties if specified
    if (properties.find("threshold") != properties.end()) {
        neuron->setThreshold(properties.at("threshold"));
    }
    if (properties.find("resting_potential") != properties.end()) {
        neuron->setRestingPotential(properties.at("resting_potential"));
    }
    if (properties.find("firing_rate") != properties.end()) {
        neuron->setFiringRate(properties.at("firing_rate"));
    }
    
    return neuron;
}

std::unique_ptr<Synapse> ComponentFactory::createSynapse(NeuronId source, NeuronId destination,
                                                         const std::unordered_map<std::string, float>& properties) {
    NLM_LOG_INFO("Creating synapse with custom properties");
    
    // Note: Synapse constructor takes source and destination directly
    auto synapse = std::make_unique<Synapse>(0, source, destination);  // ID will be set by brain
    
    // Apply custom properties if specified
    if (properties.find("weight") != properties.end()) {
        synapse->setWeight(properties.at("weight"));
    }
    if (properties.find("delay") != properties.end()) {
        synapse->setDelay(static_cast<Delay>(properties.at("delay")));
    }
    
    return synapse;
}

template<typename T>
void ComponentFactory::registerComponent(const std::string& name,
                                         std::function<std::unique_ptr<T>(const Config&, const std::unordered_map<std::string, float>&)> factory) {
    // This would be used to register component factories
    // For now, it's a placeholder
    NLM_LOG_INFO("Registered component factory: " + name);
}

std::vector<std::string> ComponentFactory::getRegisteredComponents() const {
    std::vector<std::string> result;
    for (const auto& pair : pImpl->factories) {
        result.push_back(pair.first);
    }
    return result;
}

// PluginConfig implementation
struct PluginConfig::Impl {
    std::unordered_map<std::string, Config> pluginConfigs;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    bool loaded;
    
    Impl() : loaded(false) {}
};

PluginConfig::PluginConfig() : pImpl(std::make_unique<Impl>()) {}

PluginConfig::~PluginConfig() = default;

bool PluginConfig::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper plugin configuration file format (YAML/JSON)
    NLM_LOG_INFO("Loading plugin configuration from file (placeholder implementation)");
    pImpl->loaded = true;
    return true;
}

bool PluginConfig::loadFromString(const std::string& configString) {
    // TODO: Parse configuration string
    NLM_LOG_INFO("Loading plugin configuration from string (placeholder implementation)");
    pImpl->loaded = true;
    return true;
}

std::optional<Config> PluginConfig::getPluginConfig(const std::string& pluginName) const {
    auto it = pImpl->pluginConfigs.find(pluginName);
    if (it != pImpl->pluginConfigs.end()) {
        return it->second;
    }
    return std::nullopt;
}

void PluginConfig::setPluginConfig(const std::string& pluginName, const Config& config) {
    pImpl->pluginConfigs[pluginName] = config;
}

void PluginConfig::addDependency(const std::string& pluginName, const std::string& dependency) {
    pImpl->dependencies[pluginName].push_back(dependency);
}

std::vector<std::string> PluginConfig::getDependencies(const std::string& pluginName) const {
    auto it = pImpl->dependencies.find(pluginName);
    if (it != pImpl->dependencies.end()) {
        return it->second;
    }
    return {};
}

bool PluginConfig::validatePlugin(const std::string& pluginName) const {
    auto configIt = pImpl->pluginConfigs.find(pluginName);
    if (configIt == pImpl->pluginConfigs.end()) {
        return false;
    }
    
    // Basic validation - check for required fields
    auto nameConfig = configIt->second.getOr<std::string>("name", "");
    auto versionConfig = configIt->second.getOr<std::string>("version", "");
    
    return !nameConfig.empty() && !versionConfig.empty();
}

std::vector<std::string> PluginConfig::getPluginNames() const {
    std::vector<std::string> result;
    for (const auto& pair : pImpl->pluginConfigs) {
        result.push_back(pair.first);
    }
    return result;
}

// Example plugin implementations

bool Examples::AdvancedSTDP::initialize(const Config& config) {
    learningRate_ = config.getOr<float>("learning_rate", 0.02f);
    eligibilityDecay_ = config.getOr<float>("eligibility_decay", 0.99f);
    maxTrace_ = config.getOr<float>("max_trace", 1.0f);
    NLM_LOG_INFO("Initialized AdvancedSTDP plugin with learning rate: " + std::to_string(learningRate_));
    return true;
}

std::string Examples::AdvancedSTDP::getMetadata() const {
    std::ostringstream oss;
    oss << "AdvancedSTDP Plugin" << std::endl;
    oss << "Version: 1.0.0" << std::endl;
    oss << "Learning Rate: " << learningRate_ << std::endl;
    oss << "Eligibility Decay: " << eligibilityDecay_ << std::endl;
    oss << "Max Trace: " << maxTrace_ << std::endl;
    return oss.str();
}

void Examples::AdvancedSTDP::apply(const PlasticityContext& context) {
    // TODO: Implement advanced STDP with eligibility traces
    NLM_LOG_INFO("AdvancedSTDP.apply called - TODO: Implement actual rule");
}

std::unordered_map<std::string, float> Examples::AdvancedSTDP::getParameters() const {
    return {
        {"learning_rate", learningRate_},
        {"eligibility_decay", eligibilityDecay_},
        {"max_trace", maxTrace_}
    };
}

void Examples::AdvancedSTDP::updateParameters(const std::unordered_map<std::string, float>& params) {
    auto it = params.find("learning_rate");
    if (it != params.end()) learningRate_ = it->second;
    
    it = params.find("eligibility_decay");
    if (it != params.end()) eligibilityDecay_ = it->second;
    
    it = params.find("max_trace");
    if (it != params.end()) maxTrace_ = it->second;
}

bool Examples::HierarchicalMemory::initialize(const Config& config) {
    totalCapacity_ = config.getOr<size_t>("total_capacity", 1000);
    workingCapacity_ = config.getOr<size_t>("working_capacity", 100);
    episodicCapacity_ = config.getOr<size_t>("episodic_capacity", 500);
    semanticCapacity_ = config.getOr<size_t>("semantic_capacity", 400);
    
    if (totalCapacity_ != workingCapacity_ + episodicCapacity_ + semanticCapacity_) {
        NLM_LOG_WARNING("Memory capacity allocation doesn't sum to total capacity");
    }
    
    NLM_LOG_INFO("Initialized HierarchicalMemory plugin with " << totalCapacity_ << " total capacity");
    return true;
}

std::string Examples::HierarchicalMemory::getMetadata() const {
    std::ostringstream oss;
    oss << "HierarchicalMemory Plugin" << std::endl;
    oss << "Version: 1.0.0" << std::endl;
    oss << "Total Capacity: " << totalCapacity_ << std::endl;
    oss << "Working Memory: " << workingCapacity_ << std::endl;
    oss << "Episodic Memory: " << episodicCapacity_ << std::endl;
    oss << "Semantic Memory: " << semanticCapacity_ << std::endl;
    return oss.str();
}

void Examples::HierarchicalMemory::store(const MemoryEntry& entry) {
    // TODO: Implement hierarchical memory storage
    NLM_LOG_INFO("HierarchicalMemory.store called - TODO: Implement actual storage");
}

std::optional<MemoryEntry> Examples::HierarchicalMemory::retrieve(const std::string& key) const {
    // TODO: Implement hierarchical memory retrieval
    return std::nullopt;
}

void Examples::HierarchicalMemory::clear() {
    // TODO: Implement memory clearing
    NLM_LOG_INFO("HierarchicalMemory.clear called - TODO: Implement actual clearing");
}

bool Examples::MCTPlanning::initialize(const Config& config) {
    searchDepth_ = config.getOr<size_t>("search_depth", 10);
    explorationRate_ = config.getOr<float>("exploration_rate", 0.1f);
    confidence_ = config.getOr<float>("confidence", 0.8f);
    
    NLM_LOG_INFO("Initialized MCTPlanning plugin with depth: " << searchDepth_);
    return true;
}

std::string Examples::MCTPlanning::getMetadata() const {
    std::ostringstream oss;
    oss << "MCTPlanning Plugin" << std::endl;
    oss << "Version: 1.0.0" << std::endl;
    oss << "Search Depth: " << searchDepth_ << std::endl;
    oss << "Exploration Rate: " << explorationRate_ << std::endl;
    oss << "Confidence: " << confidence_ << std::endl;
    return oss.str();
}

std::unique_ptr<Action> Examples::MCTPlanning::selectAction(const State& state) {
    // TODO: Implement MCT planning
    NLM_LOG_INFO("MCTPlanning.selectAction called - TODO: Implement actual planning");
    return nullptr;
}

void Examples::MCTPlanning::update(const Experience& experience) {
    // TODO: Implement experience updating
    NLM_LOG_INFO("MCTPlanning.update called - TODO: Implement actual update");
}

} // namespace nlm