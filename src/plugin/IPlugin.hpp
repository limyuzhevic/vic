#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "../core/Types/Types.hpp"

namespace nlm {

// Plugin system version
#define NLM_PLUGIN_VERSION "1.0.0"

// Plugin status enum
enum class PluginStatus {
    UNLOADED,
    LOADING,
    LOADED,
    ERROR,
    UNLOADING
};

// Plugin category for organization
enum class PluginCategory {
    PLASTICITY,
    NEUROMODULATION,
    REWARD,
    DEVELOPMENT,
    UTILITY,
    EXTERNAL
};

// IPlugin - Abstract base class for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin identification
    virtual const char* getName() const = 0;
    virtual const char* getVersion() const = 0;
    virtual const char* getDescription() const = 0;
    virtual PluginCategory getCategory() const = 0;
    
    // Plugin lifecycle management
    virtual bool initialize(std::map<std::string, std::string> config) = 0;
    virtual void shutdown() = 0;
    virtual PluginStatus getStatus() const = 0;
    
    // Configuration management
    virtual bool setParameter(const std::string& name, const std::string& value) = 0;
    virtual std::string getParameter(const std::string& name) const = 0;
    virtual std::map<std::string, std::string> getAllParameters() const = 0;
    
    // Plugin metadata
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    
    // Factory method for plugin creation
    using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;
    static bool registerPlugin(const std::string& name, PluginFactory factory);
    
    // Get all registered plugins
    static std::vector<std::string> getRegisteredPluginNames();
    
protected:
    IPlugin() = default;
};

// IPlasticityRule - Abstract base class for plasticity rules
class IPlasticityRule : public IPlugin {
public:
    virtual ~IPlasticityRule() = default;
    
    // Plasticity rule interface
    virtual void update(class Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) = 0;
    
    // Get rule type
    virtual PluginCategory getCategory() const override { return PluginCategory::PLASTICITY; }
    
    // Get rule statistics (optional)
    virtual float getLearningRate() const { return 0.0f; }
    virtual void setLearningRate(float rate) {}
    
    // Check if rule is applicable for current conditions
    virtual bool isApplicable(SimulationStep step, Timestamp time) const { return true; }
};

// INeuromodulator - Abstract base class for neuromodulators
class INeuromodulator : public IPlugin {
public:
    virtual ~INeuromodulator() = default;
    
    // Neuromodulator interface
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual float getPlasticityFactor() const = 0;
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator type
    virtual PluginCategory getCategory() const override { return PluginCategory::NEUROMODULATION; }
    
    // Apply effect to plasticity
    virtual float applyToPlasticity(float currentFactor, SimulationStep step) const {
        return currentFactor * getPlasticityFactor();
    }
    
    // Get neuromodulator name for display
    virtual const char* getTypeName() const { return "Neuromodulator"; }
};

// IRewardFunction - Abstract base class for reward functions
class IRewardFunction : public IPlugin {
public:
    virtual ~IRewardFunction() = default;
    
    // Reward function interface
    virtual float computeReward(const class Observation& observation,
                               SimulationStep step) const = 0;
    
    // Get function type
    virtual PluginCategory getCategory() const override { return PluginCategory::REWARD; }
    
    // Get reward statistics
    virtual float getBaselineReward() const { return 0.0f; }
    virtual void setBaselineReward(float baseline) {}
    
    // Check if reward is valid for current state
    virtual bool isValid(const class Observation& observation,
                        SimulationStep step) const { return true; }
};

// IDevelopmentModel - Abstract base class for development models
class IDevelopmentModel : public IPlugin {
public:
    virtual ~IDevelopmentModel() = default;
    
    // Development model interface
    virtual DevelopmentalStage getCurrentStage() const = 0;
    virtual void advanceStage() = 0;
    virtual float getDevelopmentProgress() const = 0;
    virtual void update(class Brain* brain, TimestepDuration dt) = 0;
    
    // Get model type
    virtual PluginCategory getCategory() const override { return PluginCategory::DEVELOPMENT; }
    
    // Development stage information
    virtual const char* getStageName(DevelopmentalStage stage) const { return "Unknown"; }
    virtual float getStageDuration(DevelopmentalStage stage) const { return 0.0f; }
    
    // Check if development is active for current conditions
    virtual bool isActive(SimulationStep step, Timestamp time) const { return true; }
};

// PluginLoader - Centralized plugin loading and management
class PluginLoader {
public:
    static PluginLoader& getInstance();
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginName,
                   const std::string& configPath = "",
                   std::map<std::string, std::string> config = {});
    
    bool unloadPlugin(const std::string& pluginName);
    
    // Plugin access
    std::unique_ptr<IPlugin>& getPlugin(const std::string& pluginName);
    bool hasPlugin(const std::string& pluginName) const;
    
    // Plugin category filtering
    std::vector<std::string> getPluginsByCategory(PluginCategory category) const;
    
    // Status management
    PluginStatus getPluginStatus(const std::string& pluginName) const;
    std::map<std::string, PluginStatus> getAllPluginStatuses() const;
    
    // Configuration management
    bool updatePluginConfig(const std::string& pluginName,
                           const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    
    // Plugin lifecycle
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool restartPlugin(const std::string& pluginName);
    
    // Factory methods for built-in plugins
    static std::unique_ptr<IPlugin> createPlasticityRulePlugin(const std::string& ruleType);
    static std::unique_ptr<IPlugin> createNeuromodulatorPlugin(const std::string& neuromodType);
    static std::unique_ptr<IPlugin> createRewardFunctionPlugin(const std::string& rewardType);
    static std::unique_ptr<IPlugin> createDevelopmentModelPlugin(const std::string& modelType);
    
private:
    PluginLoader() = default;
    ~PluginLoader() = default;
    PluginLoader(const PluginLoader&) = delete;
    PluginLoader& operator=(const PluginLoader&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PluginUtils - Utility functions for plugin development
class PluginUtils {
public:
    // Configuration parsing utilities
    static std::map<std::string, std::string> parseConfigFile(const std::string& configPath);
    static std::string toLower(const std::string& str);
    static bool stringToFloat(const std::string& str, float& value);
    static bool stringToInt(const std::string& str, int& value);
    
    // Plugin validation
    static bool validatePluginName(const std::string& name);
    static bool validatePluginVersion(const std::string& version);
    
    // Path utilities
    static std::string getPluginDirectory();
    static std::string getPluginConfigDirectory();
    static std::string getPluginDataDirectory();
    
    // Error handling
    static std::string getLastPluginError();
    static void clearLastPluginError();
};

} // namespace nlm
