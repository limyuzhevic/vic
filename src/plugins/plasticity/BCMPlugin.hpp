#pragma once

#include "../Plugin.hpp"
#include <cmath>

namespace nlm {

class BCMPlasticityPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "BCMPlasticity",
            "Bayesian Competitive Learning plasticity rule implementation",
            "NLM Team",
            PluginVersion(1, 0, 0),
            PluginType::Plasticity
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // BCM rule parameters
        theta_ = 0.0f;      // Threshold variable
        tau_theta_ = 100.0f;  // Threshold time constant
        learning_rate_ = 0.01f;  // Learning rate
        
        return true;
    }
    
    void enable() override {
        Plugin::enable();
        // Register extension point for plasticity rule creation
        if (getManager()) {
            getManager()->registerExtensionPoint("plasticity_rule", PluginType::Plasticity,
                [this](std::shared_ptr<Plugin>) { return true; });
        }
    }
    
    // BCM plasticity update function
    float updateBCM(float preSpike, float postSpike, float theta, float dt) {
        // BCM rule: Δw = η * postSpike * (v_post - θ) * preSpike
        // where v_post is postsynaptic activity and θ is the threshold
        
        float dv = postSpike * (postSpike - theta) * preSpike * learning_rate_;
        theta += (postSpike * (postSpike - theta) - theta) * (dt / tau_theta_);
        
        return dv;
    }
    
    // Set BCM parameters
    void setParameters(float theta, float tauTheta, float learningRate) {
        theta_ = theta;
        tau_theta_ = tauTheta;
        learning_rate_ = learningRate;
    }
    
    // Get BCM parameters
    void getParameters(float& theta, float& tauTheta, float& learningRate) const {
        theta = theta_;
        tauTheta = tau_theta_;
        learningRate = learning_rate_;
    }
    
    // Get extension points
    std::vector<std::string> getExtensionPoints() const override {
        return {"plasticity_rule", "learning_rule", "synaptic_modulation"};
    }
    
    // Get services
    std::vector<std::string> getProvidedServices() const override {
        return {"BCM_learning_rule", "competitive_plasticity", "stability_plasticity"};
    }
    
    // Update method
    void update(float dt) override {
        // Update internal state
        currentTime_ += dt;
    }
    
    // Configuration methods
    void setTimeConstant(float tau) { tau_theta_ = tau; }
    float getTimeConstant() const { return tau_theta_; }
    
    void setTheta(float theta) { theta_ = theta; }
    float getTheta() const { return theta_; }
    
private:
    float theta_ = 0.0f;           // Threshold variable
    float tau_theta_ = 100.0f;     // Threshold time constant (ms)
    float learning_rate_ = 0.01f; // Learning rate
    float currentTime_ = 0.0f;    // Current simulation time
};

} // namespace nlm