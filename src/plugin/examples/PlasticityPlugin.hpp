#pragma once

#include "PlasticityPlugin.hpp"
#include <random>

namespace nlm {

class ExamplePlasticityPlugin : public PlasticityPlugin {
public:
    ExamplePlasticityPlugin();
    ~ExamplePlasticityPlugin() override;
    
    // Plugin identification
    std::string getName() const override { return "ExamplePlasticity"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Example plasticity plugin with custom learning rules"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    PluginType getType() const override { return PluginType::PLASTICITY; }
    
    // Plugin lifecycle
    bool initialize(const PluginConfig& config, void* manager) override;
    void update(double deltaTime) override;
    void shutdown() override;
    
    // Plugin state management
    void pause() override;
    void resume() override;
    PluginState getState() const override;
    
    // Plugin dependencies
    std::vector<std::string> getDependencies() const override;
    bool checkDependencies(const std::unordered_map<std::string, PluginInfo>& plugins) const override;
    
    // Plugin communication
    void receiveMessage(const std::string& sender, const PluginMessage& message) override;
    void sendMessage(const std::string& receiver, const PluginMessage& message) override;
    
    // Plugin configuration
    void configure(const PluginConfig& config) override;
    PluginConfig getConfig() const override;
    
    // Plasticity extension
    void updatePlasticity(std::vector<Synapse*>& synapses, double deltaTime) override;
    void applyLearningRule(const LearningRuleContext& context) override;
    PlasticityRule getPlasticityRule() const override;
    
    // Plasticity parameters
    double getLearningRate() const override;
    double getDecayRate() const override;
    double getBaselineStrength() const override;
    
private:
    struct Impl;
    Impl* pImpl;
    double learningRate;
    double decayRate;
    double baselineStrength;
};

} // namespace nlm
