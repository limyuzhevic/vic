#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <functional>
#include <vector>
#include <type_traits>
#include <chrono>
#include "../core/Types/Types.hpp"

namespace nlm {

// Plugin system version
constexpr int PLUGIN_SYSTEM_VERSION = 1;

// Plugin interface
class PluginBase {
public:
    virtual ~PluginBase() = default;
    
    // Get plugin metadata
    virtual const std::string& getName() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual const std::string& getAuthor() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Configuration management
    virtual void setConfig(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
    virtual std::unordered_map<std::string, std::string> getAllConfig() const = 0;
    
    // Resource management
    virtual void reserveResources(size_t numNeurons, size_t numSynapses) = 0;
    virtual void releaseResources() = 0;
    
    // Statistics
    virtual size_t getNeuronCount() const = 0;
    virtual size_t getSynapseCount() const = 0;
    virtual float getMemoryUsage() const = 0;
    
    // Thread safety
    virtual void lock() const = 0;
    virtual void unlock() const = 0;
    
    // Plugin-specific operations (to be implemented by derived classes)
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    
    // Clone plugin (for registry management)
    virtual std::unique_ptr<PluginBase> clone() const = 0;
    
    // Library handle for dynamic loading
    virtual void* getLibraryHandle() const = 0;
    virtual void setLibraryHandle(void* handle) = 0;
    
    // Plugin state
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;
    virtual std::chrono::system_clock::time_point getLastActivity() const = 0;
    virtual void setLastActivity(const std::chrono::system_clock::time_point& time) = 0;
    
protected:
    std::mutex configMutex;
    std::atomic<bool> initialized{false};
    std::atomic<bool> active{false};
    std::chrono::system_clock::time_point lastActivity;
};

// Factory function type for plugin creation
using PluginFactory = std::function<std::unique_ptr<PluginBase>()>;

// Plugin metadata for discovery
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string pluginType; // "plasticity", "neuron", "synapse", "cognition"
    bool isBuiltIn;
    std::string libraryPath; // for external plugins
};

// CRTP base class for easy plugin implementation
template<typename Derived>
class Plugin : public PluginBase {
public:
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        static_cast<Derived*>(this)->onInitialize(config);
        initialized = true;
        active = true;
        lastActivity = std::chrono::system_clock::now();
        return true;
    }
    
    void shutdown() override {
        if (initialized) {
            static_cast<Derived*>(this)->onShutdown();
            initialized = false;
            active = false;
        }
    }
    
    void setConfig(const std::string& key, const std::string& value) override {
        std::lock_guard<std::mutex> lock(configMutex);
        configMap[key] = value;
        static_cast<Derived*>(this)->onConfigChanged(key, value);
    }
    
    std::string getConfig(const std::string& key) const override {
        std::lock_guard<std::mutex> lock(configMutex);
        auto it = configMap.find(key);
        return it != configMap.end() ? it->second : "";
    }
    
    std::unordered_map<std::string, std::string> getAllConfig() const override {
        std::lock_guard<std::mutex> lock(configMutex);
        return configMap;
    }
    
    void lock() const override { configMutex.lock(); }
    void unlock() const override { configMutex.unlock(); }
    
    // Implement pure virtual methods
    bool isActive() const override { return active; }
    void setActive(bool active) override { this->active = active; }
    std::chrono::system_clock::time_point getLastActivity() const override { return lastActivity; }
    void setLastActivity(const std::chrono::system_clock::time_point& time) override { lastActivity = time; }
    
protected:
    std::unordered_map<std::string, std::string> configMap;
    
    virtual void onInitialize(const std::unordered_map<std::string, std::string>& config) {}
    virtual void onShutdown() {}
    virtual void onConfigChanged(const std::string& key, const std::string& value) {}
};

} // namespace nlm