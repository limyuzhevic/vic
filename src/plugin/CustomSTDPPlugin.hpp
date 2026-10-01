// Custom STDP with Reward Modulation Plugin Header
// Example plugin header for extending NLM with custom plasticity rules

#pragma once

#include "PluginInterface.hpp"

namespace nlm {

// Custom STDP with Reward Modulation Plugin
// This plugin demonstrates how to extend NLM with custom plasticity rules
// that include reward modulation for enhanced learning capabilities

class CustomSTDPPlugin : public PlasticityRulePlugin {
public:
    CustomSTDPPlugin();
    
    const char* getName() const override;
    const char* getVersion() const override;
    const char* getDescription() const override;
    const char* getComponentName() const override;
    
    bool initialize(const std::string& config) override;
    void shutdown() override;
    bool isRunning() const override;
    
    void update(class Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt) override;
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    // Custom parameters for this plugin
    void setRewardModulation(float modulation);
    float getRewardModulation() const;
    
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Get default configuration for this plugin
    static std::string getDefaultConfig();
};

// Plugin factory function for dynamic loading
extern "C" Plugin* create_plugin();

// Plugin information for discovery
extern "C" const char* get_plugin_info();

} // namespace nlm