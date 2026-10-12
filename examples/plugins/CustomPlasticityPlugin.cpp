// Example Custom Plasticity Plugin
// Demonstrates creating a custom learning rule beyond STDP and Hebbian

#include "../include/nlm/plugin/plasticity/PlasticityPlugin.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include <string>
#include <vector>
#include <cmath>

class CustomPlasticityPlugin : public nlm::PlasticityPlugin {
public:
    NLM_PLUGIN_IMPLEMENTATION
    
    // Custom plasticity factory
    nlm::PlasticityRule* createPlasticityRule() override {
        return new nlm::PlasticityRule();
    }
    
    bool canCreatePlasticityRule(const std::string& ruleType) const override {
        return ruleType == "stdp" || ruleType == "hebbian" || ruleType == "trace_based" || 
               ruleType == "batch_normalization" || ruleType == "online_normalization" ||
               ruleType == "bilateral_stdp" || ruleType == "delay_stdp";
    }
    
    std::vector<std::string> getSupportedPlasticityRuleTypes() const override {
        return {"stdp", "hebbian", "trace_based", "batch_normalization", "online_normalization",
                "bilateral_stdp", "delay_stdp", "homeostatic", "reward_modulated"};
    }
    
    bool configurePlasticityRule(nlm::PlasticityRule& rule, const std::string& config) override {
        // Parse configuration and apply to rule
        if (config.find("learning_rate=") != std::string::npos) {
            // Extract learning rate and configure rule
        }
        return true;
    }
    
    std::string getPlasticityRuleConfig(const nlm::PlasticityRule& rule) const override {
        return "custom_plasticity_rule:learning_rate=0.01";
    }
};

NLM_PLUGIN_FACTORY(CustomPlasticityPlugin)