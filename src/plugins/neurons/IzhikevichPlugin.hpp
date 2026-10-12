#pragma once

#include "../Plugin.hpp"
#include <random>

namespace nlm {

class IzhikevichNeuronPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "IzhikevichNeuron",
            "Implementation of the Izhikevich neuron model for biologically plausible spiking",
            "NLM Team",
            PluginVersion(1, 0, 0),
            PluginType::Neuron
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // Izhikevich model parameters (typical values for different neuron types)
        a_ = -0.02f;   // Recovery variable time constant
        b_ = 0.2f;     // Recovery variable sensitivity
        c_ = -65.0f;   // After-spike reset of membrane potential
        d_ = 8.0f;     // After-spike reset of recovery variable
        
        return true;
    }
    
    void enable() override {
        Plugin::enable();
        // Register extension point for Izhikevich neuron creation
        if (getManager()) {
            getManager()->registerExtensionPoint("neuron_creation", PluginType::Neuron,
                [this](std::shared_ptr<Plugin>) { return true; });
        }
    }
    
    // Create an Izhikevich neuron simulation step
    float simulateIzhikevichStep(float v, float u, float dt, float I) {
        // Izhikevich model equations:
        // dv/dt = 0.04*v^2 + 5*v + 140 - u + I
        // du/dt = a*(b*v - u)
        
        // Euler integration
        float dv = (0.04f * v * v + 5.0f * v + 140.0f - u + I) * dt;
        float du = (a_ * (b_ * v - u)) * dt;
        
        v += dv;
        u += du;
        
        // Check for spike
        if (v >= 30.0f) {
            v = c_;  // Reset membrane potential
            u += d_;  // Add recovery variable increment
            return 1.0f;  // Spike occurred
        }
        
        return 0.0f;  // No spike
    }
    
    // Set neuron parameters
    void setNeuronParameters(float a, float b, float c, float d) {
        a_ = a;
        b_ = b;
        c_ = c;
        d_ = d;
    }
    
    // Get neuron parameters
    void getNeuronParameters(float& a, float& b, float& c, float& d) const {
        a = a_;
        b = b_;
        c = c_;
        d = d_;
    }
    
    // Get extension points
    std::vector<std::string> getExtensionPoints() const override {
        return {"neuron_creation", "neuron_simulation", "plasticity_rules"};
    }
    
    // Get services
    std::vector<std::string> getProvidedServices() const override {
        return {"izhikevich_simulation", "adaptive_exponential_neuron", "spiking_neural_model"};
    }
    
    // Update method for simulation
    void update(float dt) override {
        // Update internal parameters based on simulation time
        currentTime_ += dt;
        
        // Adjust parameters for adaptation if needed
        if (adaptationRate_ > 0.0f) {
            a_ = std::max(a_, 0.0f);  // Keep positive
            c_ = std::max(c_, -100.0f);  // Keep reasonable
        }
    }
    
    // Configuration methods
    void setAdaptationRate(float rate) { adaptationRate_ = rate; }
    float getAdaptationRate() const { return adaptationRate_; }
    
    void setInitialRecoveryVariable(float u0) { u0_ = u0; }
    float getInitialRecoveryVariable() const { return u0_; }
    
private:
    // Izhikevich model parameters
    float a_ = -0.02f;   // Recovery variable time constant
    float b_ = 0.2f;     // Recovery variable sensitivity
    float c_ = -65.0f;   // After-spike reset of membrane potential
    float d_ = 8.0f;     // After-spike reset of recovery variable
    float adaptationRate_ = 0.0f;
    float currentTime_ = 0.0f;
    float u0_ = -14.0f;  // Initial recovery variable
};

} // namespace nlm