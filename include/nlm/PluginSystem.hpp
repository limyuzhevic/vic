// Plugin System for NLM
// Extensible plugin architecture for neural network plugins
// Enables third-party extensions, custom components, and functionality expansion

#ifndef NLM_PLUGIN_SYSTEM_HPP
#define NLM_PLUGIN_SYSTEM_HPP

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <any>

namespace nlm {

// Forward declarations
class Config;
class Brain;

// Plugin System Error Types
enum class PluginError {
    SUCCESS = 0,
    PLUGIN_NOT_FOUND = 1,
    PLUGIN_ALREADY_LOADED = 2,
    INITIALIZATION_FAILED = 3,
    UNLOADING_FAILED = 4,
    INVALID_PLUGIN = 5,
    DEPENDENCY_MISSING = 6,
    CONFIGURATION_ERROR = 7
};

class PluginError : public std::runtime_error {
public:
    PluginError(PluginError code, const std::string& message)
        : std::runtime_error("Plugin error: " + message), code_(code) {}
    
    PluginError errorCode() const { return code_; }
    
private:
    PluginError code_;
};

// Plugin Interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin identification
    virtual std::string name() const = 0;
    virtual std::string version() const = 0;
    virtual std::string description() const = 0;
    virtual std::string author() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void configure(const Config& config) = 0;
    
    // Plugin capabilities
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::vector<std::string> getProvidedServices() const = 0;
    
    // Plugin events
    virtual void onBrainInitialize(Brain* brain) {}
    virtual void onBrainStep(Brain* brain, size_t step) {}
    virtual void onBrainReset(Brain* brain) {}
    virtual void onEvent(const std::string& eventType, const std::any& data) {}
    
    // Plugin statistics
    virtual std::unordered_map<std::string, double> getStatistics() const = 0;
};

// Plugin Base Class Template
template<typename TDerived>
class BasePlugin : public IPlugin {
public:
    std::string name() const override {
        return TDerived::pluginName();
    }
    
    std::string version() const override {
        return TDerived::pluginVersion();
    }
    
    std::string description() const override {
        return TDerived::pluginDescription();
    }
    
    std::string author() const override {
        return TDerived::pluginAuthor();
    }
    
    std::vector<std::string> getDependencies() const override {
        return TDerived::pluginDependencies();
    }
    
    std::vector<std::string> getProvidedServices() const override {
        return TDerived::pluginServices();
    }
    
    std::unordered_map<std::string, double> getStatistics() const override {
        return TDerived::pluginStatistics();
    }
};

// Plugin Macros
#define NLM_PLUGIN_CLASS(className) \
    public BasePlugin<className> \
public:
    static std::string pluginName() { return #className; }
    static std::string pluginVersion() { return "1.0.0"; }
    static std::string pluginDescription() { return "NLM Plugin"; }
    static std::string pluginAuthor() { return "NLM Team"; }
    static std::vector<std::string> pluginDependencies() { return {}; }
    static std::vector<std::string> pluginServices() { return {}; }
    static std::unordered_map<std::string, double> pluginStatistics() { return {}; }

} // namespace nlm

#endif // NLM_PLUGIN_SYSTEM_HPP