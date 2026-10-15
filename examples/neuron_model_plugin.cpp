// Neuron model plugin example
#include "src/plugins/NeuronModelPlugin.hpp"
#include <iostream>
#include <cmath>

class CustomNeuronModel : public nlm::INeuronModelPlugin {
public:
    std::string getName() const override {
        return "CustomNeuronModel";
    }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        try {
            membrane_tau = std::stof(parameters.at("membrane_tau"));
            membrane_threshold = std::stof(parameters.at("membrane_threshold"));
            membrane_reset = std::stof(parameters.at("membrane_reset"));
            resting_potential = std::stof(parameters.at("resting_potential"));
            gain = std::stof(parameters.at("gain"));
            spike_triggered_current = std::stof(parameters.at("spike_triggered_current"));
            
            enabled = true;
            error_message.clear();
            
            return true;
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomNeuronModel: " + std::string(e.what());
            return false;
        }
    }
    
    void update(float dt) override {
        if (refractory_timer > 0) {
            refractory_timer -= dt;
        }
    }
    
    float updateDynamics(const NeuronState& state, float dt) override {
        float dv = (-state.membranePotential + restingPotential) / membrane_tau;
        dv += currentInjection * gain;
        
        if (state.membranePotential > threshold) {
            dv += spike_triggered_current;
        }
        
        return dv * dt;
    }
    
    bool hasSpike() const override {
        return spike;
    }
    
    void resetSpike() override {
        spike = false;
    }
    
    float getRestingPotential() const override {
        return resting_potential;
    }
    
    float getThreshold() const override {
        return membrane_threshold;
    }
    
    float getMembraneTimeConstant() const override {
        return membrane_tau;
    }
    
    void resetState() override {
        state.membranePotential = resting_potential;
        state.spikeCount = 0;
        state.refractoryTime = 0.0f;
        spike = false;
        refractory_timer = 0.0;
    }
    
    NeuronState getState() const override {
        return state;
    }
    
    void setState(const NeuronState& newState) override {
        state = newState;
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
    
    float membrane_tau = 0.01f;
    float membrane_threshold = 0.5f;
    float membrane_reset = 0.0f;
    float resting_potential = 0.0f;
    float gain = 1.0f;
    float spike_triggered_current = 0.1f;
    
    NeuronState state;
    bool spike = false;
    float refractory_timer = 0.0f;
};

int main() {
    std::cout << "Neuron model plugin example" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set neuron parameters
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Load custom neuron model plugin
    if (!brain->loadPlugin("custom_neuron_model", "/path/to/plugin.so")) {
        std::cerr << "Failed to load custom neuron model plugin!" << std::endl;
        return 1;
    }
    
    std::cout << "Custom neuron model plugin loaded successfully!" << std::endl;
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
        }
    }
    
    std::cout << "Neuron model plugin simulation completed successfully!" << std::endl;
    return 0;
}