#pragma once

#include <string>
#include <memory>
#include <map>
#include "../core/Errors/Errors.hpp"

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class RandomGenerator;
class Config;

// Plugin interface for extensibility
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Initialization and cleanup
    virtual Result initialize(const Config& config) = 0;
    virtual Result shutdown() = 0;
    
    // Plugin lifecycle
    virtual void preStep(float dt) = 0;
    virtual void postStep(float dt) = 0;
    
    // Event handlers
    virtual void onNeuronCreated(Neuron* neuron) = 0;
    virtual void onNeuronDestroyed(Neuron* neuron) = 0;
    virtual void onSynapseCreated(Synapse* synapse) = 0;
    virtual void onSynapseDestroyed(Synapse* synapse) = 0;
    virtual void onSpike(Neuron* neuron, Timestamp timestamp) = 0;
    virtual void onInputChanged(Neuron* neuron, float delta) = 0;
};

// Concrete plugin types
enum class PluginType {
    Plasticity,    // Synaptic plasticity rules
    Neuromodulator, // Neuromodulatory signals
    Development,   // Developmental processes
    Memory,        // Memory systems
    Cognition,     // Cognitive functions
    Sensor,        // Sensory processing
    Motor,         // Motor control
    Environment    // Environment interaction
};

// Plugin factory interface
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    virtual std::unique_ptr<IPlugin> createInstance() = 0;
    virtual std::string getPluginTypeName() const = 0;
    virtual PluginType getPluginType() const = 0;
};

// Plugin registration macro
#define REGISTER_PLUGIN(PluginClass) \
    namespace { \
        class PluginRegistrar { \
        public: \
            PluginRegistrar() { PluginClass::registerFactory(); } \
        }; \
        PluginRegistrar pluginRegistrarInstance; \
    }

// Plugin base implementation with common functionality
class BasePlugin : public IPlugin {
protected:
    std::shared_ptr<Config> config_;
    std::shared_ptr<RandomGenerator> random_;
    bool initialized_ = false;
    
public:
    BasePlugin() = default;
    virtual ~BasePlugin() = default;
    
    // Getters for common dependencies
    std::shared_ptr<Config> getConfig() const { return config_; }
    std::shared_ptr<RandomGenerator> getRandomGenerator() const { return random_; }
    bool isInitialized() const { return initialized_; }
    
    // Default implementations for lifecycle methods
    Result initialize(const Config& config) override {
        config_ = std::make_shared<Config>(config);
        initialized_ = true;
        return Result();
    }
    
    Result shutdown() override {
        initialized_ = false;
        config_.reset();
        return Result();
    }
    
    void preStep(float dt) override {}
    void postStep(float dt) override {}
    
    void onNeuronCreated(Neuron* neuron) override {}
    void onNeuronDestroyed(Neuron* neuron) override {}
    void onSynapseCreated(Synapse* synapse) override {}
    void onSynapseDestroyed(Synapse* synapse) override {}
    void onSpike(Neuron* neuron, Timestamp timestamp) override {}
    void onInputChanged(Neuron* neuron, float delta) override {}
    
    // Utility methods
    template<typename T>
    Result getConfigValue(const std::string& key, T& value) const {
        if (!config_) {
            return Result(ErrorCode::InvalidParameter, "Plugin not initialized");
        }
        auto optValue = config_->get<T>(key);
        if (optValue) {
            value = *optValue;
            return Result();
        } else {
            return Result(ErrorCode::InvalidParameter, "Config key not found: " + key);
        }
    }
};

// Plugin manager for dynamic plugin loading and management
class PluginManager {
private:
    std::map<std::string, std::unique_ptr<IPlugin>> plugins_;
    std::map<PluginType, std::vector<std::string>> pluginsByType_;
    std::map<std::string, std::unique_ptr<IPluginFactory>> factories_;
    
public:
    PluginManager() = default;
    ~PluginManager() = default;
    
    // Disable copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Plugin management
    Result registerPlugin(std::unique_ptr<IPlugin> plugin) {
        if (!plugin) {
            return Result(ErrorCode::InvalidParameter, "Cannot register null plugin");
        }
        
        std::string name = plugin->getName();
        if (plugins_.find(name) != plugins_.end()) {
            return Result(ErrorCode::InvalidParameter, "Plugin already registered: " + name);
        }
        
        plugins_[name] = std::move(plugin);
        return Result();
    }
    
    Result unregisterPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            return Result(ErrorCode::InvalidParameter, "Plugin not found: " + name);
        }
        
        // Clean up plugin
        Result result = it->second->shutdown();
        if (!result.success()) {
            return result;
        }
        
        plugins_.erase(it);
        return Result();
    }
    
    // Plugin factory registration
    Result registerFactory(std::unique_ptr<IPluginFactory> factory) {
        if (!factory) {
            return Result(ErrorCode::InvalidParameter, "Cannot register null factory");
        }
        
        std::string typeName = factory->getPluginTypeName();
        if (factories_.find(typeName) != factories_.end()) {
            return Result(ErrorCode::InvalidParameter, "Factory already registered: " + typeName);
        }
        
        factories_[typeName] = std::move(factory);
        return Result();
    }
    
    // Plugin access
    IPlugin* getPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        return (it != plugins_.end()) ? it->second.get() : nullptr;
    }
    
    const IPlugin* getPlugin(const std::string& name) const {
        auto it = plugins_.find(name);
        return (it != plugins_.end()) ? it->second.get() : nullptr;
    }
    
    // Get plugins by type
    std::vector<IPlugin*> getPluginsByType(PluginType type) {
        std::vector<IPlugin*> result;
        auto it = pluginsByType_.find(type);
        if (it != pluginsByType_.end()) {
            for (const auto& name : it->second) {
                auto pluginIt = plugins_.find(name);
                if (pluginIt != plugins_.end()) {
                    result.push_back(pluginIt->second.get());
                }
            }
        }
        return result;
    }
    
    // Plugin lifecycle
    Result initializeAll(const Config& config) {
        for (auto& pair : plugins_) {
            Result result = pair.second->initialize(config);
            if (!result.success()) {
                return Result(ErrorCode::InitializationFailed, 
                    "Failed to initialize plugin '" + pair.first + "': " + result.message());
            }
        }
        return Result();
    }
    
    Result shutdownAll() {
        for (auto& pair : plugins_) {
            Result result = pair.second->shutdown();
            if (!result.success()) {
                return result;
            }
        }
        return Result();
    }
    
    // Update all plugins
    void updateAll(float dt) {
        for (auto& pair : plugins_) {
            pair.second->preStep(dt);
        }
        
        for (auto& pair : plugins_) {
            pair.second->postStep(dt);
        }
    }
    
    // Plugin event handling
    void onNeuronCreated(Neuron* neuron) {
        for (auto& pair : plugins_) {
            pair.second->onNeuronCreated(neuron);
        }
    }
    
    void onNeuronDestroyed(Neuron* neuron) {
        for (auto& pair : plugins_) {
            pair.second->onNeuronDestroyed(neuron);
        }
    }
    
    void onSynapseCreated(Synapse* synapse) {
        for (auto& pair : plugins_) {
            pair.second->onSynapseCreated(synapse);
        }
    }
    
    void onSynapseDestroyed(Synapse* synapse) {
        for (auto& pair : plugins_) {
            pair.second->onSynapseDestroyed(synapse);
        }
    }
    
    void onSpike(Neuron* neuron, Timestamp timestamp) {
        for (auto& pair : plugins_) {
            pair.second->onSpike(neuron, timestamp);
        }
    }
    
    void onInputChanged(Neuron* neuron, float delta) {
        for (auto& pair : plugins_) {
            pair.second->onInputChanged(neuron, delta);
        }
    }
    
    // Plugin listing
    std::vector<std::string> getPluginNames() const {
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<std::string> getFactoryTypeNames() const {
        std::vector<std::string> typeNames;
        typeNames.reserve(factories_.size());
        for (const auto& pair : factories_) {
            typeNames.push_back(pair.first);
        }
        return typeNames;
    }
    
    // Get plugins by name
    bool hasPlugin(const std::string& name) const {
        return plugins_.find(name) != plugins_.end();
    }
    
    size_t getPluginCount() const {
        return plugins_.size();
    }
    
    size_t getFactoryCount() const {
        return factories_.size();
    }
};

// Plugin iterator helper class
class PluginIterator {
    std::map<std::string, std::unique_ptr<IPlugin>>::iterator it_;
    
public:
    explicit PluginIterator(std::map<std::string, std::unique_ptr<IPlugin>>::iterator it) : it_(it) {}
    
    IPlugin* operator->() { return it_->second.get(); }
    IPlugin& operator*() { return *it_->second; }
    std::string name() const { return it_->first; }
    
    PluginIterator& operator++() {
        ++it_;
        return *this;
    }
    
    PluginIterator operator++(int) {
        PluginIterator temp = *this;
        ++it_;
        return temp;
    }
    
    bool operator==(const PluginIterator& other) const { return it_ == other.it_; }
    bool operator!=(const PluginIterator& other) const { return it_ != other.it_; }
};

// Global plugin manager instance
inline PluginManager& getPluginManager() {
    static PluginManager manager;
    return manager;
}

// Convenience functions for plugin management
inline Result registerPlugin(std::unique_ptr<IPlugin> plugin) {
    return getPluginManager().registerPlugin(std::move(plugin));
}

inline Result unregisterPlugin(const std::string& name) {
    return getPluginManager().unregisterPlugin(name);
}

inline IPlugin* getPlugin(const std::string& name) {
    return getPluginManager().getPlugin(name);
}

inline std::vector<std::string> getPluginNames() {
    return getPluginManager().getPluginNames();
}

inline Result initializePlugins(const Config& config) {
    return getPluginManager().initializeAll(config);
}

inline Result shutdownPlugins() {
    return getPluginManager().shutdownAll();
}

inline void updatePlugins(float dt) {
    getPluginManager().updateAll(dt);
}

inline void onNeuronCreatedEvent(Neuron* neuron) {
    getPluginManager().onNeuronCreated(neuron);
}

inline void onNeuronDestroyedEvent(Neuron* neuron) {
    getPluginManager().onNeuronDestroyed(neuron);
}

inline void onSynapseCreatedEvent(Synapse* synapse) {
    getPluginManager().onSynapseCreated(synapse);
}

inline void onSynapseDestroyedEvent(Synapse* synapse) {
    getPluginManager().onSynapseDestroyed(synapse);
}

inline void onSpikeEvent(Neuron* neuron, Timestamp timestamp) {
    getPluginManager().onSpike(neuron, timestamp);
}

inline void onInputChangedEvent(Neuron* neuron, float delta) {
    getPluginManager().onInputChanged(neuron, delta);
}

} // namespace nlm