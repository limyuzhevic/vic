#include "NLMPluginSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <dlfcn.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace nlm {

struct PluginRegistry::Impl {
    // Map from plugin name to plugin instance
    std::map<std::string, std::shared_ptr<NLMPlugin>> plugins;
    
    // Map from plugin path to plugin name
    std::map<std::string, std::string> pathToName;
    
    // Current brain instance
    Brain* currentBrain = nullptr;
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {}

PluginRegistry::~PluginRegistry() = default;

bool PluginRegistry::loadPlugin(const std::string& pluginPath) {
    if (isPluginLoaded(pluginPath)) {
        NLM_LOG_WARNING("Plugin already loaded: " + pluginPath);
        return true;
    }
    
    // Extract plugin name from path (filename without extension)
    size_t lastSlash = pluginPath.find_last_of('/');
    size_t lastDot = pluginPath.find_last_of('.');
    std::string pluginName = pluginPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    
    // Open the shared library
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin library " + pluginPath + ": " + std::string(dlerror()));
        return false;
    }
    
    // Get the create function
    std::string createFunction = "create_" + pluginName + "_plugin";
    void* createFunc = dlsym(handle, createFunction.c_str());
    if (!createFunc) {
        NLM_LOG_ERROR("Failed to find create function " + createFunction + " in " + pluginPath);
        dlclose(handle);
        return false;
    }
    
    // Cast the function to the correct type
    using CreatePluginFunc = std::shared_ptr<NLMPlugin>(*)();
    CreatePluginFunc createPlugin = reinterpret_cast<CreatePluginFunc>(createFunc);
    
    // Create the plugin instance
    auto plugin = createPlugin();
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin instance from " + pluginPath);
        dlclose(handle);
        return false;
    }
    
    // Store the handle for cleanup
    pImpl->pathToName[pluginPath] = pluginName;
    pImpl->plugins[pluginName] = plugin;
    
    NLM_LOG_INFO("Loaded plugin: " + pluginName + " from " + pluginPath);
    return true;
}

bool PluginRegistry::unloadPlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // Find the path for this plugin
    std::string pluginPath;
    for (const auto& pair : pImpl->pathToName) {
        if (pair.second == pluginName) {
            pluginPath = pair.first;
            break;
        }
    }
    
    // Close the shared library
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (handle) {
        dlclose(handle);
    }
    
    // Remove from maps
    pImpl->plugins.erase(it);
    pImpl->pathToName.erase(pluginPath);
    
    NLM_LOG_INFO("Unloaded plugin: " + pluginName);
    return true;
}

std::shared_ptr<NLMPlugin> PluginRegistry::getPlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<std::shared_ptr<NLMPlugin>> PluginRegistry::getAllPlugins() const {
    std::vector<std::shared_ptr<NLMPlugin>> result;
    result.reserve(pImpl->plugins.size());
    for (const auto& pair : pImpl->plugins) {
        result.push_back(pair.second);
    }
    return result;
}

bool PluginRegistry::initializeAllPlugins(Brain* brain) {
    pImpl->currentBrain = brain;
    
    for (const auto& pair : pImpl->plugins) {
        auto plugin = pair.second;
        if (!plugin->initialize(brain)) {
            NLM_LOG_ERROR("Failed to initialize plugin: " + plugin->getName());
            return false;
        }
    }
    
    NLM_LOG_INFO("Initialized " + std::to_string(pImpl->plugins.size()) + " plugins");
    return true;
}

void PluginRegistry::processStepAllPlugins(SimulationStep step, Timestamp time) {
    for (const auto& pair : pImpl->plugins) {
        auto plugin = pair.second;
        plugin->processStep(step, time);
    }
}

void PluginRegistry::shutdownAllPlugins() {
    for (const auto& pair : pImpl->plugins) {
        auto plugin = pair.second;
        plugin->shutdown();
    }
    
    pImpl->plugins.clear();
    pImpl->pathToName.clear();
    pImpl->currentBrain = nullptr;
    
    NLM_LOG_INFO("Shutdown all plugins");
}

bool PluginRegistry::isPluginLoaded(const std::string& pluginName) const {
    return pImpl->plugins.find(pluginName) != pImpl->plugins.end();
}

struct PluginFactory::Impl {
    // Map from plugin name to factory function
    std::map<std::string, std::function<std::shared_ptr<NLMPlugin>()>> factories;
};

PluginFactory::PluginFactory() : pImpl(std::make_unique<Impl>()) {}

PluginFactory::~PluginFactory() = default;

void PluginFactory::registerPlugin(const std::string& pluginName, 
                                   std::function<std::shared_ptr<NLMPlugin>()> factory) {
    pImpl->factories[pluginName] = factory;
    NLM_LOG_INFO("Registered plugin factory: " + pluginName);
}

std::shared_ptr<NLMPlugin> PluginFactory::createPlugin(const std::string& pluginName) {
    auto it = pImpl->factories.find(pluginName);
    if (it == pImpl->factories.end()) {
        NLM_LOG_ERROR("Plugin factory not found: " + pluginName);
        return nullptr;
    }
    
    return it->second();
}

std::vector<std::string> PluginFactory::getRegisteredPlugins() const {
    std::vector<std::string> result;
    result.reserve(pImpl->factories.size());
    for (const auto& pair : pImpl->factories) {
        result.push_back(pair.first);
    }
    return result;
}

// Built-in plugin implementations

class MemoryEnhancementPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "MemoryEnhancement"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Enhance working memory capacity
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->setCapacity(2000);
        }
        
        // Enhance episodic memory capacity
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->setMaxEpisodes(10000);
        }
        
        NLM_LOG_INFO("MemoryEnhancementPlugin initialized");
        return true;
    }
    
    void processStep(SimulationStep step, Timestamp time) override {
        // Periodically store enhanced memory traces
        if (step % 50 == 0 && brain_) {
            if (brain_->getWorkingMemory()) {
                std::vector<float> pattern = {1.0f, 0.5f, 0.3f, 0.1f};
                brain_->getWorkingMemory()->store(pattern, 0.8f);
            }
        }
    }
};

class PredictionEnhancementPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "PredictionEnhancement"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Enable prediction system
        if (brain->getPredictionSystem()) {
            // Add custom prediction logic
        }
        
        NLM_LOG_INFO("PredictionEnhancementPlugin initialized");
        return true;
    }
    
    void processStep(SimulationStep step, Timestamp time) override {
        // Enhanced prediction processing
    }
};

class CognitionEnhancementPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "CognitionEnhancement"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Enhance cognition systems
        if (brain->getPlanner()) {
            brain->getPlanner()->setPlanningDepth(10);
        }
        
        if (brain->getConceptFormation()) {
            // Add concept enhancement logic
        }
        
        NLM_LOG_INFO("CognitionEnhancementPlugin initialized");
        return true;
    }
    
    void processStep(SimulationStep step, Timestamp time) override {
        // Enhanced cognition processing
    }
};

class NeuromodulationEnhancementPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "NeuromodulationEnhancement"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Enhance neuromodulation systems
        if (brain->getDopamine()) {
            // Add custom dopamine modulation logic
        }
        
        NLM_LOG_INFO("NeuromodulationEnhancementPlugin initialized");
        return true;
    }
    
    void processStep(SimulationStep step, Timestamp time) override {
        // Enhanced neuromodulation processing
    }
};

namespace BuiltInPlugins {
    std::shared_ptr<NLMPlugin> createMemoryEnhancementPlugin() {
        return std::make_shared<MemoryEnhancementPlugin>();
    }
    
    std::shared_ptr<NLMPlugin> createPredictionEnhancementPlugin() {
        return std::make_shared<PredictionEnhancementPlugin>();
    }
    
    std::shared_ptr<NLMPlugin> createCognitionEnhancementPlugin() {
        return std::make_shared<CognitionEnhancementPlugin>();
    }
    
    std::shared_ptr<NLMPlugin> createNeuromodulationEnhancementPlugin() {
        return std::make_shared<NeuromodulationEnhancementPlugin>();
    }
}

} // namespace nlm