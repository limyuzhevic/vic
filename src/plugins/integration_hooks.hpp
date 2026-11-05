// nlm/plugins/integration_hooks.hpp - Integration hook system for NLM plugins and extensions
#pragma once

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace nlm {
namespace plugins {

// Hook types for different integration points
using BrainInitializedHook = std::function<void(Brain* brain)>;
using WorldResetHook = std::function<void(World* world)>;
using SensoryInputHook = std::function<void(Brain* brain, const SensoryPercept& percept)>;
using MotorOutputHook = std::function<void(Brain* brain, Action* action, ActionResult& result)>;
using RewardModulationHook = std::function<void(Brain* brain, float reward, float curiosity)>;
using DevelopmentHook = std::function<void(Brain* brain, float deltaTime)>;
using PlasticityHook = std::function<void(Brain* brain, float deltaTime)>;
using MemoryHook = std::function<void(Brain* brain, const std::string& type, const std::string& action)>;
using CheckpointHook = std::function<bool(Brain* brain, const std::string& filepath, bool saving)>;
using PredictionHook = std::function<void(Brain* brain, float prediction, float error)>;
using ConceptHook = std::function<void(Brain* brain, size_t conceptId, const std::string& conceptType)>;
using SocialHook = std::function<void(Brain* brain, const std::string& otherAgentId, const Action& action)>;
using PerformanceHook = std::function<void(Brain* brain, const std::string& operation, float duration, float memoryUsage)>;

// Plugin interface for extension points
class IntegrationPlugin {
public:
    virtual ~IntegrationPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isEnabled() const = 0;
    
    // Hook registration
    virtual void registerBrainInitializedHook(const BrainInitializedHook& hook) = 0;
    virtual void registerWorldResetHook(const WorldResetHook& hook) = 0;
    virtual void registerSensoryInputHook(const SensoryInputHook& hook) = 0;
    virtual void registerMotorOutputHook(const MotorOutputHook& hook) = 0;
    virtual void registerRewardModulationHook(const RewardModulationHook& hook) = 0;
    virtual void registerDevelopmentHook(const DevelopmentHook& hook) = 0;
    virtual void registerPlasticityHook(const PlasticityHook& hook) = 0;
    virtual void registerMemoryHook(const MemoryHook& hook) = 0;
    virtual void registerCheckpointHook(const CheckpointHook& hook) = 0;
    virtual void registerPredictionHook(const PredictionHook& hook) = 0;
    virtual void registerConceptHook(const ConceptHook& hook) = 0;
    virtual void registerSocialHook(const SocialHook& hook) = 0;
    virtual void registerPerformanceHook(const PerformanceHook& hook) = 0;
    
    // Hook execution
    virtual void executeBrainInitializedHook(Brain* brain) = 0;
    virtual void executeWorldResetHook(World* world) = 0;
    virtual void executeSensoryInputHook(Brain* brain, const SensoryPercept& percept) = 0;
    virtual void executeMotorOutputHook(Brain* brain, Action* action, ActionResult& result) = 0;
    virtual void executeRewardModulationHook(Brain* brain, float reward, float curiosity) = 0;
    virtual void executeDevelopmentHook(Brain* brain, float deltaTime) = 0;
    virtual void executePlasticityHook(Brain* brain, float deltaTime) = 0;
    virtual void executeMemoryHook(Brain* brain, const std::string& type, const std::string& action) = 0;
    virtual bool executeCheckpointHook(Brain* brain, const std::string& filepath, bool saving) = 0;
    virtual void executePredictionHook(Brain* brain, float prediction, float error) = 0;
    virtual void executeConceptHook(Brain* brain, size_t conceptId, const std::string& conceptType) = 0;
    virtual void executeSocialHook(Brain* brain, const std::string& otherAgentId, const Action& action) = 0;
    virtual void executePerformanceHook(Brain* brain, const std::string& operation, float duration, float memoryUsage) = 0;
};

// Integration hook manager for coordinating all plugins
class IntegrationHookManager {
public:
    IntegrationHookManager();
    ~IntegrationHookManager();
    
    // Plugin management
    bool registerPlugin(std::unique_ptr<IntegrationPlugin> plugin);
    bool unregisterPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    std::vector<std::string> getEnabledPluginNames() const;
    std::vector<std::string> getPluginNames() const;
    IntegrationPlugin* getPlugin(const std::string& name);
    
    // Hook management
    void registerBrainInitializedHook(const BrainInitializedHook& hook, const std::string& source = "");
    void registerWorldResetHook(const WorldResetHook& hook, const std::string& source = "");
    void registerSensoryInputHook(const SensoryInputHook& hook, const std::string& source = "");
    void registerMotorOutputHook(const MotorOutputHook& hook, const std::string& source = "");
    void registerRewardModulationHook(const RewardModulationHook& hook, const std::string& source = "");
    void registerDevelopmentHook(const DevelopmentHook& hook, const std::string& source = "");
    void registerPlasticityHook(const PlasticityHook& hook, const std::string& source = "");
    void registerMemoryHook(const MemoryHook& hook, const std::string& source = "");
    void registerCheckpointHook(const CheckpointHook& hook, const std::string& source = "");
    void registerPredictionHook(const PredictionHook& hook, const std::string& source = "");
    void registerConceptHook(const ConceptHook& hook, const std::string& source = "");
    void registerSocialHook(const SocialHook& hook, const std::string& source = "");
    void registerPerformanceHook(const PerformanceHook& hook, const std::string& source = "");
    
    // Global hook execution
    void executeAllBrainInitializedHooks(Brain* brain);
    void executeAllWorldResetHooks(World* world);
    void executeAllSensoryInputHooks(Brain* brain, const SensoryPercept& percept);
    void executeAllMotorOutputHooks(Brain* brain, Action* action, ActionResult& result);
    void executeAllRewardModulationHooks(Brain* brain, float reward, float curiosity);
    void executeAllDevelopmentHooks(Brain* brain, float deltaTime);
    void executeAllPlasticityHooks(Brain* brain, float deltaTime);
    void executeAllMemoryHooks(Brain* brain, const std::string& type, const std::string& action);
    bool executeAllCheckpointHooks(Brain* brain, const std::string& filepath, bool saving);
    void executeAllPredictionHooks(Brain* brain, float prediction, float error);
    void executeAllConceptHooks(Brain* brain, size_t conceptId, const std::string& conceptType);
    void executeAllSocialHooks(Brain* brain, const std::string& otherAgentId, const Action& action);
    void executeAllPerformanceHooks(Brain* brain, const std::string& operation, float duration, float memoryUsage);
    
    // Plugin lifecycle management
    bool initializeAllPlugins();
    bool shutdownAllPlugins();
    
    // Configuration
    void setPluginDirectory(const std::string& directory);
    std::string getPluginDirectory() const;
    void loadPluginsFromDirectory();
    
    // Statistics and monitoring
    size_t getTotalHookCount() const;
    size_t getEnabledHookCount() const;
    float getAverageExecutionTime() const;
    void printPluginStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions for easy plugin management
namespace plugin_utils {
    // Global access to the integration hook manager
    IntegrationHookManager& getHookManager();
    
    // Plugin lifecycle helpers
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& name);
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Hook registration helpers
    template<typename HookType>
    void registerHook(const HookType& hook, const std::string& hookName, const std::string& pluginName = "");
    
    // Plugin information
    std::vector<std::string> getAvailablePlugins();
    bool isPluginLoaded(const std::string& name);
    bool isPluginEnabled(const std::string& name);
    
    // Global hook execution
    void executeBrainInitializedHook(Brain* brain);
    void executeWorldResetHook(World* world);
    void executeSensoryInputHook(Brain* brain, const SensoryPercept& percept);
    void executeMotorOutputHook(Brain* brain, Action* action, ActionResult& result);
    void executeRewardModulationHook(Brain* brain, float reward, float curiosity);
    void executeDevelopmentHook(Brain* brain, float deltaTime);
    void executePlasticityHook(Brain* brain, float deltaTime);
    void executeMemoryHook(Brain* brain, const std::string& type, const std::string& action);
    bool executeCheckpointHook(Brain* brain, const std::string& filepath, bool saving);
    void executePredictionHook(Brain* brain, float prediction, float error);
    void executeConceptHook(Brain* brain, size_t conceptId, const std::string& conceptType);
    void executeSocialHook(Brain* brain, const std::string& otherAgentId, const Action& action);
    void executePerformanceHook(Brain* brain, const std::string& operation, float duration, float memoryUsage);
}

// Plugin base class template for easy plugin development
class SimplePlugin : public IntegrationPlugin {
public:
    SimplePlugin(const std::string& name, const std::string& version, const std::string& description);
    ~SimplePlugin() override = default;
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    // Override these methods in your plugin
    virtual bool onInitialize() { return true; }
    virtual bool onShutdown() { return true; }
    
    // Hook override methods (optional - call parent if you want default behavior)
    virtual void onBrainInitialized(Brain* brain) {}
    virtual void onWorldReset(World* world) {}
    virtual void onSensoryInput(Brain* brain, const SensoryPercept& percept) {}
    virtual void onMotorOutput(Brain* brain, Action* action, ActionResult& result) {}
    virtual void onRewardModulation(Brain* brain, float reward, float curiosity) {}
    virtual void onDevelopment(Brain* brain, float deltaTime) {}
    virtual void onPlasticity(Brain* brain, float deltaTime) {}
    virtual void onMemory(Brain* brain, const std::string& type, const std::string& action) {}
    virtual bool onCheckpoint(Brain* brain, const std::string& filepath, bool saving) { return true; }
    virtual void onPrediction(Brain* brain, float prediction, float error) {}
    virtual void onConcept(Brain* brain, size_t conceptId, const std::string& conceptType) {}
    virtual void onSocial(Brain* brain, const std::string& otherAgentId, const Action& action) {}
    virtual void onPerformance(Brain* brain, const std::string& operation, float duration, float memoryUsage) {}
    
    // Hook registration helpers
    void registerBrainInitializedHook(const BrainInitializedHook& hook) override;
    void registerWorldResetHook(const WorldResetHook& hook) override;
    void registerSensoryInputHook(const SensoryInputHook& hook) override;
    void registerMotorOutputHook(const MotorOutputHook& hook) override;
    void registerRewardModulationHook(const RewardModulationHook& hook) override;
    void registerDevelopmentHook(const DevelopmentHook& hook) override;
    void registerPlasticityHook(const PlasticityHook& hook) override;
    void registerMemoryHook(const MemoryHook& hook) override;
    void registerCheckpointHook(const CheckpointHook& hook) override;
    void registerPredictionHook(const PredictionHook& hook) override;
    void registerConceptHook(const ConceptHook& hook) override;
    void registerSocialHook(const SocialHook& hook) override;
    void registerPerformanceHook(const PerformanceHook& hook) override;
    
    // Hook execution with override support
    void executeBrainInitializedHook(Brain* brain) override;
    void executeWorldResetHook(World* world) override;
    void executeSensoryInputHook(Brain* brain, const SensoryPercept& percept) override;
    void executeMotorOutputHook(Brain* brain, Action* action, ActionResult& result) override;
    void executeRewardModulationHook(Brain* brain, float reward, float curiosity) override;
    void executeDevelopmentHook(Brain* brain, float deltaTime) override;
    void executePlasticityHook(Brain* brain, float deltaTime) override;
    void executeMemoryHook(Brain* brain, const std::string& type, const std::string& action) override;
    bool executeCheckpointHook(Brain* brain, const std::string& filepath, bool saving) override;
    void executePredictionHook(Brain* brain, float prediction, float error) override;
    void executeConceptHook(Brain* brain, size_t conceptId, const std::string& conceptType) override;
    void executeSocialHook(Brain* brain, const std::string& otherAgentId, const Action& action) override;
    void executePerformanceHook(Brain* brain, const std::string& operation, float duration, float memoryUsage) override;
    
protected:
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
private:
    std::string name_;
    std::string version_;
    std::string description_;
    bool enabled_ = true;
    
    // Hook storage
    std::vector<BrainInitializedHook> brainInitializedHooks_;
    std::vector<WorldResetHook> worldResetHooks_;
    std::vector<SensoryInputHook> sensoryInputHooks_;
    std::vector<MotorOutputHook> motorOutputHooks_;
    std::vector<RewardModulationHook> rewardModulationHooks_;
    std::vector<DevelopmentHook> developmentHooks_;
    std::vector<PlasticityHook> plasticityHooks_;
    std::vector<MemoryHook> memoryHooks_;
    std::vector<CheckpointHook> checkpointHooks_;
    std::vector<PredictionHook> predictionHooks_;
    std::vector<ConceptHook> conceptHooks_;
    std::vector<SocialHook> socialHooks_;
    std::vector<PerformanceHook> performanceHooks_;
};

} // namespace plugins
} // namespace nlm
