//! Enhanced Plasticity Plugin for NLM Neural Simulation Project
//! 
//! This module provides an enhanced plasticity system plugin that can be
//! dynamically loaded into the NLM neural simulation project. The plugin
//! includes advanced plasticity mechanisms including adaptive learning rules,
//! homeostatic plasticity, and metaplasticity.

#pragma once

#include "PluginBase.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class EnhancedPlasticityPlugin : public PlasticityRulePlugin {
public:
    EnhancedPlasticityPlugin();
    ~EnhancedPlasticityPlugin() override;
    
    // Plugin lifecycle implementation
    bool load(const PluginConfig& config) override;
    bool initialize() override;
    void start() override;
    void update(double deltaTime) override;
    void stop() override;
    bool shutdown() override;
    bool unload() override;
    
    // Plugin identification
    const PluginMetadata& getMetadata() const override;
    const PluginConfig& getConfig() const override;
    PluginConfig& getConfig() override;
    
    // Plugin state
    PluginState getState() const override;
    std::string getStateString() const override;
    
    // Plugin information
    const std::string& getName() const override;
    const std::string& getDescription() const override;
    PluginVersion getVersion() const override;
    
    // Event handling
    void onEvent(const PluginEventData& event) override;
    
    // Hook handling
    void onHook(const HookEventData& hook) override;
    
    // Dependency management
    const std::vector<std::string>& getDependencies() const override;
    const std::vector<std::string>& getOptionalDependencies() const override;
    
    // Serialization support
    std::string serializeConfig() const override;
    bool deserializeConfig(const std::string& data) override;
    
    // Plugin-specific operations
    bool validateConfiguration() override;
    std::string getConfigurationError() const override;
    
    // Security and permissions
    bool checkSecurityPermissions() const override;
    std::vector<std::string> getRequiredPermissions() const override;
    std::vector<std::string> getOptionalPermissions() const override;
    
    // Plasticity-specific methods
    void addPlasticityRule(class PlasticityRule* rule) override;
    bool removePlasticityRule(const std::string& name) override;
    const std::vector<class PlasticityRule*>& getPlasticityRules() const override;
    void updatePlasticityRules(class Brain* brain) override;
    
private:
    struct PlasticityRules {
        std::vector<class PlasticityRule*> rules;
        std::unordered_map<std::string, float> ruleWeights;
        std::unordered_map<std::string, float> ruleFactors;
        
        void addRule(class PlasticityRule* rule);
        bool removeRule(const std::string& name);
        std::vector<class PlasticityRule*>& getRules();
        const std::vector<class PlasticityRule*>& getRules() const;
        void updateRuleWeights(class Brain* brain);
        void updateRuleFactors(double deltaTime);
    };
    
    PluginMetadata metadata_;
    PluginConfig config_;
    PluginState state_;
    std::unique_ptr<PlasticityRules> plasticityRules_;
    bool initialized_;
    std::chrono::system_clock::time_point lastUpdateTime_;
    
    // Plasticity rule instances
    std::vector<std::unique_ptr<class PlasticityRule>> plasticityRuleInstances_;
    
    // Configuration parameters
    float learningRate_;
    float adaptationRate_;
    float maxWeightChange_;
    bool enableAdaptation_;
    bool enableMetaplasticity_;
    
    // State tracking
    std::unordered_map<std::string, float> ruleActivityLevels_;
    std::unordered_map<std::string, double> ruleHistory_;
    
    // Helper methods
    bool loadConfiguration(const PluginConfig& config);
    bool initializePlasticityRules();
    bool registerHooks();
    void startPlasticityMonitoring();
    void updatePlasticityRules(double deltaTime);
    void checkPlasticityEvents();
    void stopPlasticityMonitoring();
    void cleanupPlasticityRules();
    void unregisterHooks();
    void cleanup();
    void saveConfiguration();
    
    // Event handlers
    void onLoadEvent(const PluginEventData& event);
    void onInitializeEvent(const PluginEventData& event);
    void onUpdateEvent(const PluginEventData& event);
    void onShutdownEvent(const PluginEventData& event);
    
    // Hook handlers
    void onPlasticityRuleAddedHook(const HookEventData& hook);
    void onPlasticityRuleUpdatedHook(const HookEventData& hook);
    void onPlasticityRuleRemovedHook(const HookEventData& hook);
};

// Plugin factory function
extern "C" std::unique_ptr<PluginBase> create_EnhancedPlasticityPlugin(const PluginConfig& config);

// Plugin registration function
extern "C" const PluginMetadata& get_EnhancedPlasticityPluginMetadata();

} // namespace nlm
