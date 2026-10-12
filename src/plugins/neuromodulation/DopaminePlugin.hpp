#pragma once

#include "../Plugin.hpp"
#include <cmath>

namespace nlm {

class DopamineNeuromodulatorPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "Dopamine",
            "Dopamine neuromodulation system for reward prediction and reinforcement learning",
            "NLM Team",
            PluginVersion(1, 0, 0),
            PluginType::Neuromodulator
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // Dopamine system parameters
        baseline_level_ = 0.1f;          // Baseline dopamine level
        reward_prediction_error_ = 0.0f; // RPE for learning
        learning_rate_ = 0.01f;         // Learning rate for dopamine modulation
        max_level_ = 1.0f;              // Maximum dopamine level
        min_level_ = 0.0f;              // Minimum dopamine level
        
        return true;
    }
    
    void enable() override {
        Plugin::enable();
        // Register extension point for neuromodulator creation
        if (getManager()) {
            getManager()->registerExtensionPoint("neuromodulator", PluginType::Neuromodulator,
                [this](std::shared_ptr<Plugin>) { return true; });
        }
    }
    
    // Update dopamine level based on prediction error
    void update(float dt, float rewardPredictionError) {
        // dopamine(t) = baseline + learning_rate * prediction_error
        dopamine_level_ = baseline_level_ + learning_rate_ * rewardPredictionError;
        
        // Clamp to valid range
        dopamine_level_ = std::max(std::min(dopamine_level_, max_level_), min_level_);
        
        reward_prediction_error_ = rewardPredictionError;
        
        // Update internal state
        currentTime_ += dt;
    }
    
    // Get current dopamine level
    float getLevel() const { return dopamine_level_; }
    
    // Get plasticity factor based on dopamine level
    float getPlasticityFactor() const {
        // Dopamine modulates plasticity: higher dopamine = more plasticity
        return 1.0f + (dopamine_level_ * plasticity_sensitivity_);
    }
    
    // Get extension points
    std::vector<std::string> getExtensionPoints() const override {
        return {"neuromodulator_system", "reward_signal", "learning_modulation"};
    }
    
    // Get services
    std::vector<std::string> getProvidedServices() const override {
        return {"dopamine_signal", "reward_processing", "plasticity_modulation"};
    }
    
    // Update method
    void update(float dt) override {
        // Regular update without external signals
        dopamine_level_ = baseline_level_;
        currentTime_ += dt;
    }
    
    // Configuration methods
    void setBaselineLevel(float baseline) { baseline_level_ = baseline; }
    float getBaselineLevel() const { return baseline_level_; }
    
    void setLearningRate(float rate) { learning_rate_ = rate; }
    float getLearningRate() const { return learning_rate_; }
    
    void setPlasticitySensitivity(float sensitivity) { plasticity_sensitivity_ = sensitivity; }
    float getPlasticitySensitivity() const { return plasticity_sensitivity_; }
    
private:
    float baseline_level_ = 0.1f;          // Baseline dopamine level
    float reward_prediction_error_ = 0.0f; // Reward prediction error
    float learning_rate_ = 0.01f;         // Learning rate
    float max_level_ = 1.0f;              // Maximum dopamine level
    float min_level_ = 0.0f;              // Minimum dopamine level
    float dopamine_level_ = 0.1f;         // Current dopamine level
    float plasticity_sensitivity_ = 0.5f; // Sensitivity of plasticity to dopamine
    float currentTime_ = 0.0f;           // Current simulation time
};

} // namespace nlm