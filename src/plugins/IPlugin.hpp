#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <map>

namespace nlm {
namespace plugins {

// Plugin lifecycle states
enum class PluginState {
    UNLOADED,
    LOADING,
    LOADED,
    INITIALIZING,
    INITIALIZED,
    RUNNING,
    SHUTTING_DOWN,
    UNLOADING,
    FAILED,
    DISABLED
};

// Plugin error types
enum class PluginErrorType {
    NONE,
    INIT_FAILED,
    UPDATE_FAILED,
    CONFIG_ERROR,
    DEPENDENCY_ERROR,
    LOAD_FAILED,
    UNLOAD_FAILED,
    VALIDATION_ERROR
};

// Plugin error
struct PluginError {
    PluginErrorType type;
    std::string message;
    std::string pluginName;
    std::string source;
    double timestamp;
    
    PluginError(PluginErrorType t, const std::string& msg, const std::string& name, const std::string& src = "")
        : type(t), message(msg), pluginName(name), source(src), timestamp(0.0) {}
};

// Base plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin identification
    virtual const char* getName() const = 0;
    virtual const char* getVersion() const = 0;
    virtual const char* getDescription() const = 0;
    virtual const char* getAuthor() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const Config& config) = 0;
    virtual void update(TimestepDuration dt) = 0;
    virtual void shutdown() = 0;
    virtual void cleanup() = 0;
    
    // State management
    virtual PluginState getState() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    
    // Configuration
    virtual bool configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
    virtual std::map<std::string, std::string> getAllConfig() const = 0;
    
    // Dependencies
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual bool hasDependency(const std::string& pluginName) const = 0;
    
    // Plugin metadata
    virtual std::string getCategory() const = 0;
    virtual std::vector<std::string> getTags() const = 0;
    
    // Event system
    using EventCallback = std::function<void(const std::string& eventName, const std::map<std::string, std::string>& data)>;
    virtual void subscribeToEvent(const std::string& eventName, const EventCallback& callback) = 0;
    virtual void unsubscribeFromEvent(const std::string& eventName, const EventCallback& callback) = 0;
    
    // Error handling
    virtual std::vector<PluginError> getErrors() const = 0;
    virtual bool hasErrors() const = 0;
    virtual void clearErrors() = 0;
    
    // Performance monitoring
    virtual double getLastUpdateTime() const = 0;
    virtual double getUpdateTimeAverage() const = 0;
    virtual uint64_t getUpdateCount() const = 0;
    
    // Thread safety
    virtual void lock() const = 0;
    virtual void unlock() const = 0;
    virtual bool tryLock() const = 0;
    
    // Copyable but not movable
    IPlugin() = default;
    IPlugin(const IPlugin&) = default;
    IPlugin& operator=(const IPlugin&) = default;
    IPlugin(IPlugin&&) = delete;
    IPlugin& operator=(IPlugin&&) = delete;
};

// General component plugin interface for specific system types
class IComponentPlugin : public virtual IPlugin {
public:
    virtual ~IComponentPlugin() = default;
    
    // Component identification
    virtual std::string getComponentType() const = 0;
    virtual std::string getComponentName() const = 0;
    
    // Component validation
    virtual bool validate() const = 0;
    virtual std::vector<std::string> getValidationErrors() const = 0;
    
    // Component integration
    virtual bool integrate(void* systemContext) = 0;
    virtual bool deintegrate() = 0;
};

// Plasticity plugin interface
class IPlasticityPlugin : public virtual IPlugin {
public:
    virtual ~IPlasticityPlugin() = default;
    
    // Plasticity rule properties
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;
    
    virtual float getEligibilityTraceDecay() const = 0;
    virtual void setEligibilityTraceDecay(float decay) = 0;
    
    virtual float getModulatoryFactor() const = 0;
    virtual void setModulatoryFactor(float factor) = 0;
    
    // Plasticity operation
    virtual float computePlasticityChange(
        const Synapse& synapse,
        float preActivity,
        float postActivity,
        float neuromodulatorLevel = 1.0f) = 0;
    
    // Parameter constraints
    virtual bool validateParameters() const = 0;
    virtual std::vector<std::string> getParameterConstraints() const = 0;
};

// Neuromodulation plugin interface
class INeuromodulationPlugin : public virtual IPlugin {
public:
    virtual ~INeuromodulationPlugin() = default;
    
    // Neuromodulator properties
    virtual float getModulationStrength() const = 0;
    virtual void setModulationStrength(float strength) = 0;
    
    virtual float getModulationRange() const = 0;
    virtual void setModulationRange(float range) = 0;
    
    // Modulation computation
    virtual float computeModulation(
        float input,
        float timeFactor = 1.0f,
        float contextFactor = 1.0f) = 0;
    
    // Modulation targets
    virtual std::vector<std::string> getTargetSystems() const = 0;
    virtual bool affectsPlasticity() const = 0;
    virtual bool affectsExcitability() const = 0;
    virtual bool affectsInhibition() const = 0;
};

// Memory system plugin interface
class IMemoryPlugin : public virtual IPlugin {
public:
    virtual ~IMemoryPlugin() = default;
    
    // Memory system properties
    virtual std::string getMemoryType() const = 0;
    virtual size_t getCapacity() const = 0;
    virtual float getAccessLatency() const = 0;
    
    // Memory operations
    virtual bool store(const std::string& key, const std::vector<float>& value, double timestamp) = 0;
    virtual bool retrieve(const std::string& key, std::vector<float>& value, double& timestamp) const = 0;
    virtual bool erase(const std::string& key) = 0;
    virtual bool contains(const std::string& key) const = 0;
    
    // Memory management
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual float getMemoryUtilization() const = 0;
};

// Development plugin interface
class IDevelopmentPlugin : public virtual IPlugin {
public:
    virtual ~IDevelopmentPlugin() = default;
    
    // Developmental stage
    virtual DevelopmentalStage getStage() const = 0;
    virtual const char* getStageName() const = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;
    
    // Age tracking
    virtual double getAge() const = 0;
    virtual void setAge(double age) = 0;
    virtual void advanceAge(double delta) = 0;
    
    // Developmental parameters
    virtual float getPlasticityRate() const = 0;
    virtual float getCriticalPeriodProgress() const = 0;
    virtual bool isCriticalPeriodActive() const = 0;
    
    // Developmental triggers
    virtual void triggerEvent(const std::string& event) = 0;
    virtual std::vector<std::string> getActiveTriggers() const = 0;
};

// Plugin registry entry
struct PluginEntry {
    std::string name;
    std::string version;
    std::string path;
    std::string category;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> config;
    bool enabled;
    PluginState state;
    
    PluginEntry(const std::string& n, const std::string& v, const std::string& p)
        : name(n), version(v), path(p), enabled(true), state(PluginState::UNLOADED) {}
};

} // namespace plugins
} // namespace nlm
