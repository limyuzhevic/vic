// Plugin registry implementation
// This file implements the PluginRegistry class and related functionality

#include "PluginInterface.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace nlm {

void PluginRegistry::registerPlugin(const std::string& name, PluginFactory factory) {
    if (factories_.find(name) != factories_.end()) {
        // Plugin already registered - we could log a warning here
        return;
    }
    factories_[name] = std::move(factory);
}

PluginFactory PluginRegistry::getPluginFactory(const std::string& name) const {
    auto it = factories_.find(name);
    if (it != factories_.end()) {
        return it->second;
    }
    return nullptr;
}

bool PluginRegistry::hasPlugin(const std::string& name) const {
    return factories_.find(name) != factories_.end();
}

std::vector<std::string> PluginRegistry::getPluginNames() const {
    std::vector<std::string> names;
    names.reserve(factories_.size());
    for (const auto& pair : factories_) {
        names.push_back(pair.first);
    }
    return names;
}

bool PluginRegistry::loadPluginsFromDirectory(const std::string& directory) {
#ifdef __linux__
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.size() >= 3 && filename.substr(filename.size() - 3) == ".so") {
                // Try to load the shared library
                void* handle = dlopen(entry.path().string().c_str(), RTLD_LAZY);
                if (handle) {
                    // Look for plugin factory symbol
                    void* symbol = dlsym(handle, "create_plugin");
                    if (symbol) {
                        auto factory = reinterpret_cast<PluginFactory(*)()>(symbol);
                        PluginFactory pluginFactory = factory();
                        if (pluginFactory) {
                            std::string pluginName = pluginFactory()->getName();
                            registerPlugin(pluginName, std::move(pluginFactory));
                        }
                    }
                    dlclose(handle);
                }
            }
        }
    }
#endif
    return true;
}

std::unique_ptr<Plugin> PluginRegistry::createPlugin(const std::string& name) {
    auto it = factories_.find(name);
    if (it != factories_.end()) {
        auto instance = it->second();
        instances_[name] = std::move(instance);
        return std::move(instances_[name]);
    }
    return nullptr;
}

bool PluginRegistry::initializeAllPlugins(const std::string& config) {
    bool allSuccess = true;
    for (auto& pair : factories_) {
        auto plugin = pair.second();
        if (plugin && !plugin->initialize(config)) {
            allSuccess = false;
            // Clean up partially initialized plugins
            for (auto& cleanupPair : instances_) {
                cleanupPair.second->shutdown();
            }
            instances_.clear();
            break;
        }
        instances_[pair.first] = std::move(plugin);
    }
    return allSuccess;
}

void PluginRegistry::shutdownAllPlugins() {
    for (auto& pair : instances_) {
        if (pair.second) {
            pair.second->shutdown();
        }
    }
    instances_.clear();
}

PluginRegistry& PluginRegistry::getInstance() {
    static PluginRegistry instance;
    return instance;
}

// Template implementations for PluginUtils
namespace PluginUtils {

template<typename T>
std::unique_ptr<PlasticityRulePlugin> createPlasticityRulePlugin(
    const std::string& name, const std::string& description) {
    class PlasticityRulePluginWrapper : public PlasticityRulePlugin {
    public:
        PlasticityRulePluginWrapper() : T() {}
        
        const char* getName() const override { return name.c_str(); }
        const char* getVersion() const override { return getPluginVersion(1, 0, 0).c_str(); }
        const char* getDescription() const override { return description.c_str(); }
        const char* getComponentName() const override { return "Unknown"; }
        
        bool initialize(const std::string& config) override { return true; }
        void shutdown() override {}
        bool isRunning() const override { return true; }
        
        void update(class Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) override {
            // Default implementation - delegate to concrete class
            static_cast<T*>(this)->update(synapse, preSpikes, postSpikes, dt);
        }
        
        float getLearningRate() const override {
            return static_cast<const T*>(this)->getLearningRate();
        }
        
        void setLearningRate(float rate) override {
            static_cast<T*>(this)->setLearningRate(rate);
        }
    };
    
    return std::make_unique<PlasticityRulePluginWrapper>();
}

template<typename T>
std::unique_ptr<NeuromodulatorPlugin> createNeuromodulatorPlugin(
    const std::string& name, const std::string& description) {
    class NeuromodulatorPluginWrapper : public NeuromodulatorPlugin {
    public:
        NeuromodulatorPluginWrapper() : T() {}
        
        const char* getName() const override { return name.c_str(); }
        const char* getVersion() const override { return getPluginVersion(1, 0, 0).c_str(); }
        const char* getDescription() const override { return description.c_str(); }
        const char* getComponentName() const override { return "Unknown"; }
        
        bool initialize(const std::string& config) override { return true; }
        void shutdown() override {}
        bool isRunning() const override { return true; }
        
        float calculateEffect(const class Brain* brain,
                             TimestepDuration dt) override {
            return static_cast<T*>(this)->calculateEffect(brain, dt);
        }
        
        float getBaselineLevel() const override {
            return static_cast<const T*>(this)->getBaselineLevel();
        }
        
        void setBaselineLevel(float level) override {
            static_cast<T*>(this)->setBaselineLevel(level);
        }
    };
    
    return std::make_unique<NeuromodulatorPluginWrapper>();
}

template<typename T>
std::unique_ptr<MemorySystemPlugin> createMemorySystemPlugin(
    const std::string& name, const std::string& description) {
    class MemorySystemPluginWrapper : public MemorySystemPlugin {
    public:
        MemorySystemPluginWrapper() : T() {}
        
        const char* getName() const override { return name.c_str(); }
        const char* getVersion() const override { return getPluginVersion(1, 0, 0).c_str(); }
        const char* getDescription() const override { return description.c_str(); }
        const char* getComponentName() const override { return "Unknown"; }
        
        bool initialize(const std::string& config) override { return true; }
        void shutdown() override {}
        bool isRunning() const override { return true; }
        
        void encode(const std::vector<float>& input,
                   const std::string& context) override {
            static_cast<T*>(this)->encode(input, context);
        }
        
        std::vector<float> retrieve(const std::string& context,
                                   float threshold) override {
            return static_cast<T*>(this)->retrieve(context, threshold);
        }
        
        size_t getCapacity() const override {
            return static_cast<const T*>(this)->getCapacity();
        }
        
        void setCapacity(size_t capacity) override {
            static_cast<T*>(this)->setCapacity(capacity);
        }
    };
    
    return std::make_unique<MemorySystemPluginWrapper>();
}

} // namespace PluginUtils

} // namespace nlm