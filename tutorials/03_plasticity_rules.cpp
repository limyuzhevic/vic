// Tutorial 3: Implementing Custom Plasticity
#include "src/plasticity/PlasticityRule.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <cmath>

class CustomPlasticityRule : public nlm::IPlasticityRulePlugin {
public:
    std::string getName() const override {
        return "CustomPlasticityRule";
    }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        try {
            learning_rate = std::stof(parameters.at("learning_rate"));
            time_constant = std::stof(parameters.at("time_constant"));
            neuromodulation_factor = std::stof(parameters.at("neuromodulation_factor"));
            max_weight_change = std::stof(parameters.at("max_weight_change"));
            
            enabled = true;
            error_message.clear();
            
            return true;
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomPlasticityRule: " + std::string(e.what());
            return false;
        }
    }
    
    void update(float dt) override {
        // Update rule state
    }
    
    float computeWeightChange(const SynapseState& pre,
                             const SynapseState& post,
                             float dt) override {
        // Custom Hebbian-like rule with neuromodulation
        float pre_spike = pre.spikeRecent ? 1.0f : 0.0f;
        float post_spike = post.spikeRecent ? 1.0f : 0.0f;
        
        // Basic Hebbian term
        float weight_change = learning_rate * pre_spike * post_spike;
        
        // Add neuromodulation
        weight_change *= (1.0 + neuromodulation_factor * dopamine_level);
        
        // Apply bounds
        return std::clamp(weight_change, -max_weight_change, max_weight_change);
    }
    
    float getLearningRate() const override {
        return learning_rate;
    }
    
    float getTimeConstant() const override {
        return time_constant;
    }
    
    std::string getRuleType() const override {
        return "CustomHebbian";
    }
    
    void resetState() override {
        // Reset rule state
    }
    
    void setRuleParameters(const std::map<std::string, float>& parameters) override {
        for (const auto& param : parameters) {
            if (param.first == "learning_rate") learning_rate = param.second;
            else if (param.first == "time_constant") time_constant = param.second;
            else if (param.first == "neuromodulation_factor") neuromodulation_factor = param.second;
            else if (param.first == "max_weight_change") max_weight_change = param.second;
        }
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void enable() override {
        enabled = true;
    }
    
    void disable() override {
        enabled = false;
    }
    
    std::string getLastError() const override {
        return error_message;
    }
    
    void clearError() override {
        error_message.clear();
    }
    
private:
    bool enabled = false;
    std::string error_message;
    
    float learning_rate = 0.01f;
    float time_constant = 1.0f;
    float neuromodulation_factor = 1.0f;
    float max_weight_change = 0.1f;
    float dopamine_level = 0.0f;
};

int main() {
    std::cout << "Tutorial 3: Implementing Custom Plasticity" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("plasticity_learning_rate", 0.02);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "✗ Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Brain initialized successfully!" << std::endl;
    
    // Load custom plasticity rule plugin
    if (!brain->loadPlugin("custom_plasticity_rule", "/path/to/plugin.so")) {
        std::cerr << "✗ Failed to load custom plasticity rule plugin!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Custom plasticity rule plugin loaded successfully!" << std::endl;
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
        }
    }
    
    std::cout << "✓ Custom plasticity simulation completed successfully!" << std::endl;
    return 0;
}