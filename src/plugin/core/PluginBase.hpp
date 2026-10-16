#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {
namespace plugin {

// Plugin status enum
enum class PluginStatus {
    UNLOADED,
    LOADING,
    LOADED,
    INITIALIZING,
    RUNNING,
    SHUTTING_DOWN,
    UNLOADING,
    FAILED,
    DISABLED
};

// Plugin lifecycle states
enum class PluginPhase {
    PRE_INIT,
    INIT,
    POST_INIT,
    PRE_RUN,
    RUNNING,
    PRE_SHUTDOWN,
    SHUTDOWN,
    POST_SHUTDOWN
};

// Plugin metadata structure
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string category;
    std::map<std::string, std::string> dependencies;
    std::vector<std::string> tags;
    bool requiresRestart;
    std::chrono::system_clock::time_point createdAt;
    
    PluginMetadata()
        : requiresRestart(false)
        , createdAt(std::chrono::system_clock::now()) {}
};

// Base Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin identification
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual const std::string& getName() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual PluginStatus getStatus() const = 0;
    
    // Lifecycle management
    virtual bool load(const nlohmann::json& config) = 0;
    virtual bool initialize() = 0;
    virtual bool run() = 0;
    virtual bool shutdown() = 0;
    virtual bool unload() = 0;
    
    // State management
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
    virtual nlohmann::json getState() const = 0;
    virtual bool setState(const nlohmann::json& state) = 0;
    
    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getError() const = 0;
    virtual std::vector<std::string> getErrors() const = 0;
    
    // Configuration
    virtual nlohmann::json getDefaultConfig() const = 0;
    virtual bool validateConfig(const nlohmann::json& config) = 0;
    
    // Thread safety
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool tryLock() = 0;
    
    // Plugin events
    virtual void onPhaseChanged(PluginPhase from, PluginPhase to) {}
    virtual void onConfigChanged(const std::string& key, const nlohmann::json& value) {}
    virtual void onDependencyLoaded(const std::string& pluginName) {}
    virtual void onDependencyUnloaded(const std::string& pluginName) {}
    
    // Registration
    virtual bool registerPlugin() = 0;
    virtual bool unregisterPlugin() = 0;
};

// Thread-safe plugin base class
class ThreadSafePlugin : public Plugin {
protected:
    mutable std::mutex mutex_;
    mutable std::recursive_mutex recursiveMutex_;
    mutable std::atomic<PluginStatus> status_;
    PluginMetadata metadata_;
    nlohmann::json config_;
    nlohmann::json state_;
    std::vector<std::string> errors_;
    bool enabled_;
    
public:
    ThreadSafePlugin();
    virtual ~ThreadSafePlugin() override;
    
    const PluginMetadata& getMetadata() const override;
    const std::string& getName() const override;
    const std::string& getVersion() const override;
    PluginStatus getStatus() const override;
    
    bool load(const nlohmann::json& config) override;
    bool initialize() override;
    bool run() override;
    bool shutdown() override;
    bool unload() override;
    
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;
    nlohmann::json getState() const override;
    bool setState(const nlohmann::json& state) override;
    
    bool hasError() const override;
    std::string getError() const override;
    std::vector<std::string> getErrors() const override;
    
    nlohmann::json getDefaultConfig() const override;
    bool validateConfig(const nlohmann::json& config) override;
    
    void lock() override;
    void unlock() override;
    bool tryLock() override;
    
    // Internal helper methods
protected:
    void addError(const std::string& error);
    void clearErrors();
    std::string getCurrentPhaseAsString() const;
    
    // Virtual methods for subclasses to override
    virtual bool onLoad(const nlohmann::json& config);
    virtual bool onInitialize();
    virtual bool onRun();
    virtual bool onShutdown();
    virtual bool onUnload();
    virtual nlohmann::json onGetDefaultConfig() const;
    virtual bool onValidateConfig(const nlohmann::json& config);
    virtual void onStateChange() {}
};

// NeuronType plugin interface
class NeuronTypePlugin : public ThreadSafePlugin {
public:
    virtual ~NeuronTypePlugin() = default;
    
    // Neuron type information
    virtual const std::string& getNeuronTypeName() const = 0;
    virtual float getDefaultThreshold() const = 0;
    virtual float getDefaultRestingPotential() const = 0;
    virtual float getDefaultMembraneTimeConstant() const = 0;
    virtual float getDefaultRefractoryPeriod() const = 0;
    
    // Neuron creation
    virtual std::unique_ptr<class Neuron> createNeuron(NeuronId id) = 0;
    
    // Neuron type specific dynamics
    virtual bool updateNeuronState(class Neuron& neuron, TimestepDuration dt) = 0;
    virtual bool checkForSpike(class Neuron& neuron) const = 0;
    virtual void resetNeuron(class Neuron& neuron) = 0;
};

// PlasticityRule plugin interface
class PlasticityRulePlugin : public ThreadSafePlugin {
public:
    virtual ~PlasticityRulePlugin() = default;
    
    // Rule information
    virtual const std::string& getRuleName() const = 0;
    virtual const std::string& getRuleDescription() const = 0;
    virtual PlasticityRuleType getSupportedRuleType() const = 0;
    
    // Plasticity updates
    virtual bool updatePlasticity(class Synapse& synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) = 0;
    virtual bool applyWeightChange(class Synapse& synapse, SynapticWeight delta) = 0;
    
    // Eligibility traces (for reward-modulated plasticity)
    virtual float getEligibilityTrace(class Synapse& synapse) const = 0;
    virtual void updateEligibilityTrace(class Synapse& synapse, float delta, TimestepDuration dt) = 0;
    virtual void resetEligibilityTrace(class Synapse& synapse) = 0;
};

// Neuromodulator plugin interface
class NeuromodulatorPlugin : public ThreadSafePlugin {
public:
    virtual ~NeuromodulatorPlugin() = default;
    
    // Modulator information
    virtual const std::string& getModulatorName() const = 0;
    virtual NeuromodulatorType getModulatorType() const = 0;
    virtual float getDefaultConcentration() const = 0;
    
    // Neuromodulatory effects
    virtual float getEffectOnPlasticity() const = 0;
    virtual float getEffectOnExcitation() const = 0;
    virtual float getEffectOnInhibition() const = 0;
    
    // Update dynamics
    virtual bool updateModulatorLevel(TimestepDuration dt) = 0;
    virtual void applyModulatoryEffects(class Brain& brain) = 0;
    virtual void resetModulator() = 0;
};

// MemorySystem plugin interface
class MemorySystemPlugin : public ThreadSafePlugin {
public:
    virtual ~MemorySystemPlugin() = default;
    
    // Memory system information
    virtual const std::string& getMemorySystemName() const = 0;
    virtual MemorySystemType getMemorySystemType() const = 0;
    virtual size_t getMaxCapacity() const = 0;
    
    // Memory operations
    virtual bool storeMemory(const MemoryEntry& entry) = 0;
    virtual bool retrieveMemory(MemoryId id, MemoryEntry& entry) const = 0;
    virtual bool updateMemory(const MemoryEntry& entry) = 0;
    virtual bool deleteMemory(MemoryId id) = 0;
    
    // Memory management
    virtual bool clearMemory() = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual float getMemoryUtilization() const = 0;
    virtual std::vector<MemoryId> getAllMemoryIds() const = 0;
};

// LearningAlgorithm plugin interface
class LearningAlgorithmPlugin : public ThreadSafePlugin {
public:
    virtual ~LearningAlgorithmPlugin() = default;
    
    // Algorithm information
    virtual const std::string& getAlgorithmName() const = 0;
    virtual const std::string& getAlgorithmDescription() const = 0;
    virtual LearningAlgorithmType getAlgorithmType() const = 0;
    
    // Learning operations
    virtual bool learnFromExperience(const LearningExperience& experience) = 0;
    virtual nlohmann::json getLearningParameters() const = 0;
    virtual bool setLearningParameters(const nlohmann::json& params) = 0;
    
    // Model management
    virtual bool saveModel(const std::string& filepath) const = 0;
    virtual bool loadModel(const std::string& filepath) = 0;
    virtual nlohmann::json getModelState() const = 0;
    virtual bool setModelState(const nlohmann::json& state) = 0;
};

}}} // namespace nlm::plugin