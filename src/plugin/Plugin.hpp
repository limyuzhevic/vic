#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Plugin metadata for discovery and loading
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    int priority;
    bool enabled;
    std::string category; // BrainRegion, PlasticityRule, Neuromodulator, Memory, Cognition, Environment
    std::unordered_map<std::string, std::string> configParams;
};

// Standard plugin lifecycle operations
enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Initialized,
    Running,
    ShuttingDown,
    Unloaded
};

// Core interface for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin metadata
    virtual const PluginMetadata& getMetadata() const = 0;
    
    // Plugin lifecycle management
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void update(TimestepDuration dt) = 0;
    virtual void shutdown() = 0;
    
    // Get current state
    virtual PluginState getState() const = 0;
    
    // Plugin identification and categorization
    virtual const char* getCategory() const = 0;
    virtual const char* getName() const = 0;
    virtual const char* getVersion() const = 0;
    
    // Type information
    virtual bool isValid() const = 0;
};

// Interface for configurable plugins
class IConfigurable {
public:
    virtual ~IConfigurable() = default;
    
    virtual bool configure(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual const std::unordered_map<std::string, std::string>& getConfiguration() const = 0;
    virtual std::vector<std::string> getConfigParameters() const = 0;
    virtual std::string getParameterType(const std::string& paramName) const = 0;
    virtual std::string getParameterDescription(const std::string& paramName) const = 0;
};

// Interface for plugins that support state persistence
class IState {
public:
    virtual ~IState() = default;
    
    virtual bool saveState(const std::string& filepath) const = 0;
    virtual bool loadState(const std::string& filepath) = 0;
    virtual bool hasState() const = 0;
    virtual void clearState() = 0;
    virtual std::string getStateSignature() const = 0;
};

// Interface for plugins that provide computational metrics
class IComputational {
public:
    virtual ~IComputational() = default;
    
    virtual float getCpuUsage() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual size_t getActiveObjects() const = 0;
    virtual float getPerformanceScore() const = 0;
    virtual void resetPerformanceMetrics() = 0;
};

// Plugin configuration wrapper
class PluginConfig {
public:
    std::unordered_map<std::string, std::string> parameters;
    std::string pluginName;
    int priority;
    bool enabled;
    std::string category;
    
    PluginConfig() : priority(0), enabled(true) {}
    
    bool hasParameter(const std::string& name) const {
        return parameters.find(name) != parameters.end();
    }
    
    std::string getParameter(const std::string& name, const std::string& defaultValue = "") const {
        auto it = parameters.find(name);
        return it != parameters.end() ? it->second : defaultValue;
    }
    
    float getParameterFloat(const std::string& name, float defaultValue = 0.0f) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            return std::stof(it->second);
        }
        return defaultValue;
    }
    
    int getParameterInt(const std::string& name, int defaultValue = 0) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            return std::stoi(it->second);
        }
        return defaultValue;
    }
};

} // namespace nlm