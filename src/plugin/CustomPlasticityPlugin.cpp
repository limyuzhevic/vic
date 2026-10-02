// Example custom plasticity plugin
// 
// Demonstrates how to extend NLM with custom plasticity rules.

#include "PluginArchitecture.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <vector>
#include <string>

namespace nlm {

class CustomPlasticityPlugin : public PlasticityPlugin {
public:
    CustomPlasticityPlugin() = default;
    
    std::string getName() const override {
        return "CustomPlasticityPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Example custom plasticity plugin demonstrating plugin architecture";
    }
    
    std::string getPlasticityName() const override {
        return "CustomSTDP";
    }
    
    std::unique_ptr<PlasticityRule> createPlasticityRule() override {
        auto rule = std::make_unique<CustomSTDP>("CustomSTDP", 0.015f, 0.02f, 15.0f);
        return rule;
    }
    
    bool initialize() override {
        // Custom initialization logic
        PluginUtils::logPluginEvent(getName(), "Initializing custom plasticity plugin");
        
        // Could load custom parameters from config file
        customParams.learningRate = 0.01f;
        customParams.timeConstant = 20.0f;
        customParams.maxWeightChange = 0.05f;
        
        return true;
    }
    
    void cleanup() override {
        PluginUtils::logPluginEvent(getName(), "Cleaning up custom plasticity plugin");
    }

private:
    struct CustomPlasticityParameters {
        float learningRate;
        float timeConstant;
        float maxWeightChange;
    };
    
    CustomPlasticityParameters customParams;
};

class CustomSTDP : public PlasticityRule {
public:
    CustomSTDP(const std::string& name, float ltpWeight, float ltdWeight, float tau)
        : name_(name), ltpWeight_(ltpWeight), ltdWeight_(ltdWeight), tau_(tau) {}
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse) {
            return;
        }
        
        // Custom STDP implementation with additional learning rule
        float totalDelta = 0.0f;
        
        // Standard STDP component
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (dt > 0) {
                    // Potentiation with custom learning rate
                    float weightChange = ltpWeight_ * std::exp(-dt / tau_) * customParams.learningRate;
                    totalDelta += weightChange;
                } else if (dt < 0) {
                    // Depression with custom learning rate
                    float weightChange = ltdWeight_ * std::exp(dt / tau_) * customParams.learningRate;
                    totalDelta -= weightChange;
                }
            }
        }
        
        // Apply additional custom rule: metaplasticity
        // Synaptic learning capability changes based on previous changes
        float previousChanges = synapse->getEligibilityTrace();
        float metaplasticChange = calculateMetaplasticChange(previousChanges);
        totalDelta += metaplasticChange;
        
        // Apply weight change
        if (std::abs(totalDelta) > 1e-6f) {
            synapse->addToWeight(totalDelta);
            
            // Log significant changes
            if (std::abs(totalDelta) > 0.01f) {
                PluginUtils::logPluginEvent("CustomPlasticityPlugin",
                                         "CustomSTDP significant weight change: " + 
                                         std::to_string(totalDelta));
            }
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        // Apply weight change with custom bounds and limits
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + delta;
        
        // Custom weight limits for this plasticity rule
        float minWeight = -0.5f;
        float maxWeight = 0.5f;
        
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        
        // Apply additional custom constraint: weight change speed limit
        if (std::abs(newWeight - currentWeight) > customParams.maxWeightChange) {
            newWeight = currentWeight + std::copysign(customParams.maxWeightChange, newWeight - currentWeight);
        }
        
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override {
        return name_.c_str();
    }
    
    float getEfficacy() const override {
        return efficacy_;
    }
    
    void setEfficacy(float efficacy) override {
        efficacy_ = efficacy;
    }

private:
    float calculateMetaplasticChange(float previousChanges) {
        // Custom metaplasticity rule: reduces learning capability after large changes
        if (std::abs(previousChanges) > 1.0f) {
            // High metaplasticity - reduce learning rate
            return previousChanges * 0.1f;  // Dampen the effect
        } else if (std::abs(previousChanges) > 0.5f) {
            // Medium metaplasticity
            return previousChanges * 0.5f;
        }
        
        // Low metaplasticity - normal learning
        return previousChanges * 1.0f;
    }
    
    std::string name_;
    float ltpWeight_;
    float ltdWeight_;
    float tau_;
    float efficacy_ = 1.0f;
    
    // Note: Need access to custom parameters
    // This would typically be done through a plugin context or configuration
    static CustomPlasticityParameters customParams;
};

CustomPlasticityPlugin::CustomPlasticityParameters CustomSTDP::customParams;

// Example usage macro for easy plugin registration
#define CUSTOM_PLASTICITY_PLUGIN_REGISTER \
    REGISTER_PLUGIN("CustomPlasticityPlugin", "1.0.0", \
                   "Example custom plasticity plugin", \
                   []() { return std::make_unique<CustomPlasticityPlugin>(); })

} // namespace nlm