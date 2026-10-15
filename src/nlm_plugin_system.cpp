#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <typeinfo>
#include <any>
#include <thread>
#include <mutex>

namespace nlm {

// Plugin system forward declarations
class Plugin;
class PluginFactory;
class PluginModule;
class PluginManager;

// Plugin base class
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::any>& config) { return true; }
    virtual void shutdown() {}
    virtual bool isInitialized() const { return initialized_; }
    
    // Plugin type
    virtual std::string getType() const = 0;
    
    // Plugin capabilities
    virtual std::vector<std::string> getCapabilities() const { return capabilities_; }
    virtual bool hasCapability(const std::string& capability) const {
        return std::find(capabilities_.begin(), capabilities_.end(), capability) != capabilities_.end();
    }
    
protected:
    void setInitialized(bool initialized) { initialized_ = initialized; }
    
private:
    bool initialized_ = false;
    std::vector<std::string> capabilities_;
};

// Plugin factory for creating plugin instances
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    
    virtual std::string getFactoryId() const = 0;
    virtual std::string getFactoryName() const = 0;
    virtual std::string getFactoryDescription() const = 0;
    
    virtual std::unique_ptr<Plugin> createPlugin(const std::map<std::string, std::any>& config) = 0;
    
    virtual std::vector<std::string> getRequiredCapabilities() const { return requiredCapabilities_; }
    virtual std::vector<std::string> getProvidedCapabilities() const { return providedCapabilities_; }
    
    virtual bool validateConfig(const std::map<std::string, std::any>& config) const { return true; }
    
protected:
    void addRequiredCapability(const std::string& capability) {
        requiredCapabilities_.push_back(capability);
    }
    
    void addProvidedCapability(const std::string& capability) {
        providedCapabilities_.push_back(capability);
    }
    
private:
    std::vector<std::string> requiredCapabilities_;
    std::vector<std::string> providedCapabilities_;
};

// Plugin module for organizing related plugins
class PluginModule {
public:
    PluginModule(const std::string& name, const std::string& version = "1.0.0")
        : name_(name), version_(version) {}
    
    virtual ~PluginModule() = default;
    
    // Module identification
    std::string getName() const { return name_; }
    std::string getVersion() const { return version_; }
    std::string getDescription() const { return description_; }
    void setDescription(const std::string& description) { description_ = description; }
    
    // Plugin management
    void addPlugin(std::unique_ptr<PluginFactory> pluginFactory) {
        pluginFactories_.push_back(std::move(pluginFactory));
    }
    
    std::vector<PluginFactory*> getPluginFactories() const {
        std::vector<PluginFactory*> factories;
        factories.reserve(pluginFactories_.size());
        for (const auto& factory : pluginFactories_) {
            factories.push_back(factory.get());
        }
        return factories;
    }
    
    // Configuration
    void setModuleConfig(const std::map<std::string, std::any>& config) {
        moduleConfig_ = config;
    }
    
    std::map<std::string, std::any> getModuleConfig() const { return moduleConfig_; }
    
    // Module initialization
    virtual bool initialize() { return true; }
    virtual void shutdown() {}
    virtual bool isInitialized() const { return initialized_; }
    
protected:
    void setInitialized(bool initialized) { initialized_ = initialized; }
    
private:
    std::string name_;
    std::string version_;
    std::string description_;
    std::vector<std::unique_ptr<PluginFactory>> pluginFactories_;
    std::map<std::string, std::any> moduleConfig_;
    bool initialized_ = false;
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager() = default;
    
    // Plugin loading
    bool loadPlugin(const std::string& filepath);
    bool loadPluginsFromDirectory(const std::string& directory);
    bool unloadPlugin(const std::string& pluginName);
    bool unloadAllPlugins();
    
    // Plugin discovery
    std::vector<std::string> getLoadedPluginNames() const;
    std::vector<Plugin*> getPluginsByType(const std::string& type) const;
    std::vector<Plugin*> getPluginsByCapability(const std::string& capability) const;
    Plugin* getPlugin(const std::string& pluginName) const;
    
    // Plugin factories
    std::vector<PluginFactory*> getPluginFactories() const;
    std::vector<PluginFactory*> getPluginFactoriesByType(const std::string& type) const;
    PluginFactory* getPluginFactory(const std::string& factoryId) const;
    
    // Module management
    void registerModule(std::unique_ptr<PluginModule> module);
    std::vector<PluginModule*> getRegisteredModules() const;
    PluginModule* getModule(const std::string& moduleName) const;
    
    // Configuration
    void setGlobalConfig(const std::map<std::string, std::any>& config) {
        globalConfig_ = config;
    }
    std::map<std::string, std::any> getGlobalConfig() const { return globalConfig_; }
    
    // Lifecycle management
    bool initializeAllPlugins();
    void shutdownAllPlugins();
    bool initializeAllModules();
    void shutdownAllModules();
    
    // Plugin information
    std::string getPluginInfo(const std::string& pluginName) const;
    std::string getModuleInfo(const std::string& moduleName) const;
    
    // Plugin execution
    bool executePluginAction(const std::string& pluginName, const std::string& action, 
                            const std::map<std::string, std::any>& parameters = {});
    
    // Save/Load plugin state
    bool savePluginState(const std::string& pluginName, const std::string& filepath);
    bool loadPluginState(const std::string& pluginName, const std::string& filepath);
    
    // Plugin statistics
    std::map<std::string, std::any> getPluginStatistics() const;
    
private:
    struct PluginInfo {
        std::string name;
        std::string filepath;
        std::unique_ptr<Plugin> instance;
        std::unique_ptr<PluginFactory> factory;
        bool initialized = false;
        std::chrono::system_clock::time_point loadTime;
    };
    
    std::map<std::string, PluginInfo> loadedPlugins_;
    std::map<std::string, std::unique_ptr<PluginModule>> registeredModules_;
    std::map<std::string, std::any> globalConfig_;
    mutable std::mutex pluginsMutex_;
    
    // Dynamic library loading
    void* loadDynamicLibrary(const std::string& filepath);
    void unloadDynamicLibrary(void* handle);
    void* getSymbol(void* handle, const std::string& symbolName);
    
    // Plugin factory creation
    template<typename T>
    std::unique_ptr<PluginFactory> createPluginFactory() {
        return std::make_unique<T>();
    }
};

// Plugin module for brain components
class BrainComponentModule : public PluginModule {
public:
    BrainComponentModule() : PluginModule("BrainComponents", "1.0.0") {
        setDescription("Module containing brain component plugins");
        
        // Register neuron factories
        class NeuronPluginFactory : public PluginFactory {
        public:
            std::string getFactoryId() const override { return "neuron_factory"; }
            std::string getFactoryName() const override { return "Neuron Plugin Factory"; }
            std::string getFactoryDescription() const override { return "Creates neuron plugins"; }
            
            std::vector<std::string> getProvidedCapabilities() const override {
                return {"neuron_creation", "neuron_dynamics", "neuron_plasticity"};
            }
            
            std::unique_ptr<Plugin> createPlugin(const std::map<std::string, std::any>& config) override {
                // Create neuron plugin instance
                return createNeuronPlugin(config);
            }
            
        private:
            std::unique_ptr<Plugin> createNeuronPlugin(const std::map<std::string, std::any>& config) {
                // Implementation would create a neuron plugin
                // For now, return a placeholder
                class NeuronPlugin : public Plugin {
                public:
                    std::string getName() const override { return "Generic Neuron Plugin"; }
                    std::string getVersion() const override { return "1.0.0"; }
                    std::string getDescription() const override { return "Generic neuron plugin"; }
                    std::string getAuthor() const override { return "NLM Team"; }
                    std::string getType() const override { return "neuron"; }
                    
                    bool initialize(const std::map<std::string, std::any>& config) override {
                        // Initialize neuron plugin
                        return true;
                    }
                };
                return std::make_unique<NeuronPlugin>();
            }
        };
        
        addPlugin(std::make_unique<NeuronPluginFactory>());
        
        // Register synapse factories
        class SynapsePluginFactory : public PluginFactory {
        public:
            std::string getFactoryId() const override { return "synapse_factory"; }
            std::string getFactoryName() const override { return "Synapse Plugin Factory"; }
            std::string getFactoryDescription() const override { return "Creates synapse plugins"; }
            
            std::vector<std::string> getProvidedCapabilities() const override {
                return {"synapse_creation", "synaptic_transmission", "synaptic_plasticity"};
            }
            
            std::unique_ptr<Plugin> createPlugin(const std::map<std::string, std::any>& config) override {
                // Create synapse plugin instance
                return createSynapsePlugin(config);
            }
            
        private:
            std::unique_ptr<Plugin> createSynapsePlugin(const std::map<std::string, std::any>& config) {
                // Implementation would create a synapse plugin
                class SynapsePlugin : public Plugin {
                public:
                    std::string getName() const override { return "Generic Synapse Plugin"; }
                    std::string getVersion() const override { return "1.0.0"; }
                    std::string getDescription() const override { return "Generic synapse plugin"; }
                    std::string getAuthor() const override { return "NLM Team"; }
                    std::string getType() const override { return "synapse"; }
                    
                    bool initialize(const std::map<std::string, std::any>& config) override {
                        // Initialize synapse plugin
                        return true;
                    }
                };
                return std::make_unique<SynapsePlugin>();
            }
        };
        
        addPlugin(std::make_unique<SynapsePluginFactory>());
    }
};

// Plugin module for memory systems
class MemorySystemModule : public PluginModule {
public:
    MemorySystemModule() : PluginModule("MemorySystems", "1.0.0") {
        setDescription("Module containing memory system plugins");
        
        // Register memory factory
        class MemoryPluginFactory : public PluginFactory {
        public:
            std::string getFactoryId() const override { return "memory_factory"; }
            std::string getFactoryName() const override { return "Memory Plugin Factory"; }
            std::string getFactoryDescription() const override { return "Creates memory system plugins"; }
            
            std::vector<std::string> getProvidedCapabilities() const override {
                return {"memory_storage", "memory_recall", "memory_association"};
            }
            
            std::unique_ptr<Plugin> createPlugin(const std::map<std::string, std::any>& config) override {
                // Create memory plugin instance
                return createMemoryPlugin(config);
            }
            
        private:
            std::unique_ptr<Plugin> createMemoryPlugin(const std::map<std::string, std::any>& config) {
                // Implementation would create a memory plugin
                class MemoryPlugin : public Plugin {
                public:
                    std::string getName() const override { return "Generic Memory Plugin"; }
                    std::string getVersion() const override { return "1.0.0"; }
                    std::string getDescription() const override { return "Generic memory plugin"; }
                    std::string getAuthor() const override { return "NLM Team"; }
                    std::string getType() const override { return "memory"; }
                    
                    bool initialize(const std::map<std::string, std::any>& config) override {
                        // Initialize memory plugin
                        return true;
                    }
                };
                return std::make_unique<MemoryPlugin>();
            }
        };
        
        addPlugin(std::make_unique<MemoryPluginFactory>());
    }
};

// Plugin module for neuromodulation
class NeuromodulationModule : public PluginModule {
public:
    NeuromodulationModule() : PluginModule("Neuromodulation", "1.0.0") {
        setDescription("Module containing neuromodulation plugins");
        
        // Register neuromodulator factory
        class NeuromodulatorPluginFactory : public PluginFactory {
        public:
            std::string getFactoryId() const override { return "neuromodulator_factory"; }
            std::string getFactoryName() const override { return "Neuromodulator Plugin Factory"; }
            std::string getFactoryDescription() const override { return "Creates neuromodulator plugins"; }
            
            std::vector<std::string> getProvidedCapabilities() const override {
                return {"neurotransmission", "neuroplasticity", "behavioral_modulation"};
            }
            
            std::unique_ptr<Plugin> createPlugin(const std::map<std::string, std::any>& config) override {
                // Create neuromodulator plugin instance
                return createNeuromodulatorPlugin(config);
            }
            
        private:
            std::unique_ptr<Plugin> createNeuromodulatorPlugin(const std::map<std::string, std::any>& config) {
                // Implementation would create a neuromodulator plugin
                class NeuromodulatorPlugin : public Plugin {
                public:
                    std::string getName() const override { return "Generic Neuromodulator Plugin"; }
                    std::string getVersion() const override { return "1.0.0"; }
                    std::string getDescription() const override { return "Generic neuromodulator plugin"; }
                    std::string getAuthor() const override { return "NLM Team"; }
                    std::string getType() const override { return "neuromodulation"; }
                    
                    bool initialize(const std::map<std::string, std::any>& config) override {
                        // Initialize neuromodulator plugin
                        return true;
                    }
                };
                return std::make_unique<NeuromodulatorPlugin>();
            }
        };
        
        addPlugin(std::make_unique<NeuromodulatorPluginFactory>());
    }
};

// Plugin module for cognitive systems
class CognitiveSystemModule : public PluginModule {
public:
    CognitiveSystemModule() : PluginModule("CognitiveSystems", "1.0.0") {
        setDescription("Module containing cognitive system plugins");
        
        // Register cognitive system factory
        class CognitiveSystemPluginFactory : public PluginFactory {
        public:
            std::string getFactoryId() const override { return "cognitive_system_factory"; }
            std::string getFactoryName() const override { return "Cognitive System Plugin Factory"; }
            std::string getFactoryDescription() const override { return "Creates cognitive system plugins"; }
            
            std::vector<std::string> getProvidedCapabilities() const override {
                return {"concept_formation", "planning", "attention", "reasoning"};
            }
            
            std::unique_ptr<Plugin> createPlugin(const std::map<std::string, std::any>& config) override {
                // Create cognitive system plugin instance
                return createCognitiveSystemPlugin(config);
            }
            
        private:
            std::unique_ptr<Plugin> createCognitiveSystemPlugin(const std::map<std::string, std::any>& config) {
                // Implementation would create a cognitive system plugin
                class CognitiveSystemPlugin : public Plugin {
                public:
                    std::string getName() const override { return "Generic Cognitive System Plugin"; }
                    std::string getVersion() const override { return "1.0.0"; }
                    std::string getDescription() const override { return "Generic cognitive system plugin"; }
                    std::string getAuthor() const override { return "NLM Team"; }
                    std::string getType() const override { return "cognition"; }
                    
                    bool initialize(const std::map<std::string, std::any>& config) override {
                        // Initialize cognitive system plugin
                        return true;
                    }
                };
                return std::make_unique<CognitiveSystemPlugin>();
            }
        };
        
        addPlugin(std::make_unique<CognitiveSystemPluginFactory>());
    }
};

// Convenience functions for plugin management
inline PluginManager& getPluginManager() {
    static PluginManager manager;
    return manager;
}

inline void registerDefaultModules() {
    auto& pluginManager = getPluginManager();
    
    // Register core modules
    pluginManager.registerModule(std::make_unique<BrainComponentModule>());
    pluginManager.registerModule(std::make_unique<MemorySystemModule>());
    pluginManager.registerModule(std::make_unique<NeuromodulationModule>());
    pluginManager.registerModule(std::make_unique<CognitiveSystemModule>());
}

inline bool initializeNLMPlugins() {
    auto& pluginManager = getPluginManager();
    return pluginManager.initializeAllModules();
}

inline void shutdownNLMPlugins() {
    auto& pluginManager = getPluginManager();
    pluginManager.shutdownAllModules();
}

inline bool loadNLMPlugins(const std::string& pluginsDirectory) {
    auto& pluginManager = getPluginManager();
    return pluginManager.loadPluginsFromDirectory(pluginsDirectory);
}

inline std::string listAvailablePlugins() {
    auto& pluginManager = getPluginManager();
    std::stringstream ss;
    
    ss << "=== Available Plugins ===" << std::endl;
    
    // List registered modules
    ss << "\n=== Registered Modules ===" << std::endl;
    auto modules = pluginManager.getRegisteredModules();
    for (const auto& module : modules) {
        ss << module->getName() << " (" << module->getVersion() << ") - " 
           << module->getDescription() << std::endl;
        
        auto factories = module->getPluginFactories();
        for (const auto& factory : factories) {
            ss << "  - " << factory->getFactoryName() << ": " 
               << factory->getFactoryDescription() << std::endl;
        }
    }
    
    // List loaded plugins
    ss << "\n=== Loaded Plugins ===" << std::endl;
    auto pluginNames = pluginManager.getLoadedPluginNames();
    for (const auto& name : pluginNames) {
        ss << name << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm

// Implementation details for PluginManager
namespace nlm {

bool PluginManager::loadPlugin(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    // Check if plugin is already loaded
    std::string pluginName = extractPluginName(filepath);
    if (loadedPlugins_.find(pluginName) != loadedPlugins_.end()) {
        std::cout << "Plugin already loaded: " << pluginName << std::endl;
        return true;
    }
    
    try {
        // Load dynamic library
        void* handle = loadDynamicLibrary(filepath);
        if (!handle) {
            std::cout << "Failed to load dynamic library: " << filepath << std::endl;
            return false;
        }
        
        // Get plugin factory creation function
        using CreatePluginFactoryFunc = PluginFactory* (*)();
        auto createFunc = reinterpret_cast<CreatePluginFactoryFunc>(
            getSymbol(handle, "createPluginFactory"));
        
        if (!createFunc) {
            std::cout << "Failed to find createPluginFactory symbol in: " << filepath << std::endl;
            unloadDynamicLibrary(handle);
            return false;
        }
        
        // Create plugin factory
        PluginFactory* factory = createFunc();
        if (!factory) {
            std::cout << "Failed to create plugin factory from: " << filepath << std::endl;
            unloadDynamicLibrary(handle);
            return false;
        }
        
        // Store plugin information
        PluginInfo info;
        info.name = pluginName;
        info.filepath = filepath;
        info.factory = std::unique_ptr<PluginFactory>(factory);
        info.loadTime = std::chrono::system_clock::now();
        
        loadedPlugins_[pluginName] = std::move(info);
        
        std::cout << "Successfully loaded plugin: " << pluginName << " from " << filepath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Error loading plugin " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool PluginManager::loadPluginsFromDirectory(const std::string& directory) {
    bool success = true;
    
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        std::cout << "Directory does not exist or is not a directory: " << directory << std::endl;
        return false;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            
            // Check if file has a plugin extension
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == ".so" || extension == ".dll" || extension == ".dylib") {
                if (!loadPlugin(filepath)) {
                    success = false;
                }
            }
        }
    }
    
    return success;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) {
        std::cout << "Plugin not found: " << pluginName << std::endl;
        return false;
    }
    
    // Shutdown plugin instance if initialized
    PluginInfo& info = it->second;
    if (info.instance && info.instance->isInitialized()) {
        info.instance->shutdown();
    }
    
    // Unload dynamic library
    unloadDynamicLibrary(it->second.handle);
    
    // Remove plugin
    loadedPlugins_.erase(it);
    
    std::cout << "Successfully unloaded plugin: " << pluginName << std::endl;
    return true;
}

bool PluginManager::unloadAllPlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    bool success = true;
    for (auto& [name, info] : loadedPlugins_) {
        if (!unloadPlugin(name)) {
            success = false;
        }
    }
    
    return success;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<std::string> names;
    names.reserve(loadedPlugins_.size());
    for (const auto& [name, info] : loadedPlugins_) {
        names.push_back(name);
    }
    return names;
}

std::vector<Plugin*> PluginManager::getPluginsByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<Plugin*> plugins;
    for (const auto& [name, info] : loadedPlugins_) {
        if (info.instance && info.instance->getType() == type) {
            plugins.push_back(info.instance.get());
        }
    }
    return plugins;
}

std::vector<Plugin*> PluginManager::getPluginsByCapability(const std::string& capability) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<Plugin*> plugins;
    for (const auto& [name, info] : loadedPlugins_) {
        if (info.instance && info.instance->hasCapability(capability)) {
            plugins.push_back(info.instance.get());
        }
    }
    return plugins;
}

Plugin* PluginManager::getPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = loadedPlugins_.find(pluginName);
    if (it != loadedPlugins_.end()) {
        return it->second.instance.get();
    }
    return nullptr;
}

std::vector<PluginFactory*> PluginManager::getPluginFactories() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<PluginFactory*> factories;
    for (const auto& [name, info] : loadedPlugins_) {
        if (info.factory) {
            factories.push_back(info.factory.get());
        }
    }
    return factories;
}

std::vector<PluginFactory*> PluginManager::getPluginFactoriesByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<PluginFactory*> factories;
    for (const auto& [name, info] : loadedPlugins_) {
        if (info.factory && info.factory->getFactoryId().find(type) != std::string::npos) {
            factories.push_back(info.factory.get());
        }
    }
    return factories;
}

PluginFactory* PluginManager::getPluginFactory(const std::string& factoryId) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    for (const auto& [name, info] : loadedPlugins_) {
        if (info.factory && info.factory->getFactoryId() == factoryId) {
            return info.factory.get();
        }
    }
    return nullptr;
}

void PluginManager::registerModule(std::unique_ptr<PluginModule> module) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    std::string moduleName = module->getName();
    
    if (registeredModules_.find(moduleName) != registeredModules_.end()) {
        std::cout << "Module already registered: " << moduleName << std::endl;
        return;
    }
    
    registeredModules_[moduleName] = std::move(module);
    std::cout << "Successfully registered module: " << moduleName << std::endl;
}

std::vector<PluginModule*> PluginManager::getRegisteredModules() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<PluginModule*> modules;
    modules.reserve(registeredModules_.size());
    for (const auto& [name, module] : registeredModules_) {
        modules.push_back(module.get());
    }
    return modules;
}

PluginModule* PluginManager::getModule(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = registeredModules_.find(moduleName);
    if (it != registeredModules_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool PluginManager::initializeAllPlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    bool success = true;
    for (auto& [name, info] : loadedPlugins_) {
        if (info.factory) {
            // Create plugin instance
            auto config = globalConfig_;
            info.instance = info.factory->createPlugin(config);
            
            if (info.instance && info.instance->initialize(config)) {
                info.instance->setInitialized(true);
                std::cout << "Successfully initialized plugin: " << name << std::endl;
            } else {
                std::cout << "Failed to initialize plugin: " << name << std::endl;
                success = false;
            }
        }
    }
    
    return success;
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    for (auto& [name, info] : loadedPlugins_) {
        if (info.instance && info.instance->isInitialized()) {
            info.instance->shutdown();
            info.instance->setInitialized(false);
        }
    }
}

bool PluginManager::initializeAllModules() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    bool success = true;
    for (auto& [name, module] : registeredModules_) {
        if (module->initialize()) {
            module->setInitialized(true);
            std::cout << "Successfully initialized module: " << name << std::endl;
        } else {
            std::cout << "Failed to initialize module: " << name << std::endl;
            success = false;
        }
    }
    
    return success;
}

void PluginManager::shutdownAllModules() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    for (auto& [name, module] : registeredModules_) {
        if (module->isInitialized()) {
            module->shutdown();
            module->setInitialized(false);
        }
    }
}

std::string PluginManager::getPluginInfo(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) {
        return "Plugin not found: " + pluginName;
    }
    
    const PluginInfo& info = it->second;
    std::stringstream ss;
    
    ss << "=== Plugin Information ===" << std::endl;
    ss << "Name: " << info.name << std::endl;
    ss << "Filepath: " << info.filepath << std::endl;
    ss << "Initialized: " << (info.initialized ? "Yes" : "No") << std::endl;
    ss << "Load Time: " << std::chrono::system_clock::to_time_t(info.loadTime) << std::endl;
    
    if (info.instance) {
        ss << "Plugin Instance: " << info.instance->getName() << std::endl;
        ss << "Plugin Version: " << info.instance->getVersion() << std::endl;
        ss << "Plugin Type: " << info.instance->getType() << std::endl;
        ss << "Plugin Description: " << info.instance->getDescription() << std::endl;
        ss << "Plugin Author: " << info.instance->getAuthor() << std::endl;
        
        auto capabilities = info.instance->getCapabilities();
        if (!capabilities.empty()) {
            ss << "Plugin Capabilities:" << std::endl;
            for (const auto& capability : capabilities) {
                ss << "  - " << capability << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::string PluginManager::getModuleInfo(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = registeredModules_.find(moduleName);
    if (it == registeredModules_.end()) {
        return "Module not found: " + moduleName;
    }
    
    const auto& module = it->second;
    std::stringstream ss;
    
    ss << "=== Module Information ===" << std::endl;
    ss << "Name: " << module->getName() << std::endl;
    ss << "Version: " << module->getVersion() << std::endl;
    ss << "Description: " << module->getDescription() << std::endl;
    ss << "Initialized: " << (module->isInitialized() ? "Yes" : "No") << std::endl;
    
    auto factories = module->getPluginFactories();
    if (!factories.empty()) {
        ss << "Plugin Factories:" << std::endl;
        for (const auto& factory : factories) {
            ss << "  - " << factory->getFactoryName() << ": " 
               << factory->getFactoryDescription() << std::endl;
            
            auto providedCapabilities = factory->getProvidedCapabilities();
            if (!providedCapabilities.empty()) {
                ss << "    Provides capabilities:" << std::endl;
                for (const auto& capability : providedCapabilities) {
                    ss << "      - " << capability << std::endl;
                }
            }
            
            auto requiredCapabilities = factory->getRequiredCapabilities();
            if (!requiredCapabilities.empty()) {
                ss << "    Requires capabilities:" << std::endl;
                for (const auto& capability : requiredCapabilities) {
                    ss << "      - " << capability << std::endl;
                }
            }
        }
    }
    
    return ss.str();
}

bool PluginManager::executePluginAction(const std::string& pluginName, const std::string& action,
                                      const std::map<std::string, std::any>& parameters) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end() || !it->second.instance) {
        std::cout << "Plugin not found or not instantiated: " << pluginName << std::endl;
        return false;
    }
    
    // Implementation would depend on the specific plugin and action
    // For now, this is a placeholder
    std::cout << "Executing action " << action << " on plugin " << pluginName << std::endl;
    return true;
}

bool PluginManager::savePluginState(const std::string& pluginName, const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end() || !it->second.instance) {
        std::cout << "Plugin not found or not instantiated: " << pluginName << std::endl;
        return false;
    }
    
    // Implementation would save plugin state to file
    std::cout << "Saving plugin state for " << pluginName << " to " << filepath << std::endl;
    return true;
}

bool PluginManager::loadPluginState(const std::string& pluginName, const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end() || !it->second.instance) {
        std::cout << "Plugin not found or not instantiated: " << pluginName << std::endl;
        return false;
    }
    
    // Implementation would load plugin state from file
    std::cout << "Loading plugin state for " << pluginName << " from " << filepath << std::endl;
    return true;
}

std::map<std::string, std::any> PluginManager::getPluginStatistics() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::map<std::string, std::any> stats;
    
    stats["total_plugins"] = static_cast<int64_t>(loadedPlugins_.size());
    stats["total_modules"] = static_cast<int64_t>(registeredModules_.size());
    
    int initialized_plugins = 0;
    for (const auto& [name, info] : loadedPlugins_) {
        if (info.initialized) {
            initialized_plugins++;
        }
    }
    stats["initialized_plugins"] = static_cast<int64_t>(initialized_plugins);
    
    int initialized_modules = 0;
    for (const auto& [name, module] : registeredModules_) {
        if (module->isInitialized()) {
            initialized_modules++;
        }
    }
    stats["initialized_modules"] = static_cast<int64_t>(initialized_modules);
    
    return stats;
}

// Dynamic library loading implementation
void* PluginManager::loadDynamicLibrary(const std::string& filepath) {
    // Implementation depends on the platform
    // For Windows: LoadLibrary
    // For Linux/macOS: dlopen
    // This is a placeholder implementation
    
    std::cout << "Loading dynamic library: " << filepath << std::endl;
    return nullptr;
}

void PluginManager::unloadDynamicLibrary(void* handle) {
    // Implementation depends on the platform
    // For Windows: FreeLibrary
    // For Linux/macOS: dlclose
    // This is a placeholder implementation
    
    std::cout << "Unloading dynamic library" << std::endl;
}

void* PluginManager::getSymbol(void* handle, const std::string& symbolName) {
    // Implementation depends on the platform
    // For Windows: GetProcAddress
    // For Linux/macOS: dlsym
    // This is a placeholder implementation
    
    std::cout << "Getting symbol: " << symbolName << std::endl;
    return nullptr;
}

std::string PluginManager::extractPluginName(const std::string& filepath) {
    std::string filename = filepath;
    
    // Extract filename from path
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        filename = filename.substr(pos + 1);
    }
    
    pos = filename.find_last_of('\\');
    if (pos != std::string::npos) {
        filename = filename.substr(pos + 1);
    }
    
    // Remove extension
    pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        filename = filename.substr(0, pos);
    }
    
    return filename;
}

} // namespace nlm

// Plugin management convenience functions
inline void registerNLMModules() {
    nlm::registerDefaultModules();
}

inline bool initializeNLMPugins(const std::string& pluginsDirectory) {
    if (!pluginsDirectory.empty()) {
        return nlm::loadNLMPlugins(pluginsDirectory);
    }
    return nlm::initializeNLMPlugins();
}

inline void shutdownNLMPugins() {
    nlm::shutdownNLMPlugins();
}

inline std::string listNLMPackages() {
    return nlm::listAvailablePlugins();
}

inline bool loadPluginFromFile(const std::string& filepath) {
    return nlm::getPluginManager().loadPlugin(filepath);
}

inline bool loadPluginsFromDirectory(const std::string& directory) {
    return nlm::getPluginManager().loadPluginsFromDirectory(directory);
}

inline bool unloadPlugin(const std::string& pluginName) {
    return nlm::getPluginManager().unloadPlugin(pluginName);
}

inline std::string getPluginInfo(const std::string& pluginName) {
    return nlm::getPluginManager().getPluginInfo(pluginName);
}

inline std::string getModuleInfo(const std::string& moduleName) {
    return nlm::getPluginManager().getModuleInfo(moduleName);
}

inline bool executePluginAction(const std::string& pluginName, const std::string& action,
                               const std::map<std::string, std::any>& parameters) {
    return nlm::getPluginManager().executePluginAction(pluginName, action, parameters);
}

inline std::string getNLMPluginStatistics() {
    auto stats = nlm::getPluginManager().getPluginStatistics();
    std::stringstream ss;
    
    ss << "=== NLM Plugin Statistics ===" << std::endl;
    ss << "Total Plugins: " << stats["total_plugins"] << std::endl;
    ss << "Initialized Plugins: " << stats["initialized_plugins"] << std::endl;
    ss << "Total Modules: " << stats["total_modules"] << std::endl;
    ss << "Initialized Modules: " << stats["initialized_modules"] << std::endl;
    
    return ss.str();
}
