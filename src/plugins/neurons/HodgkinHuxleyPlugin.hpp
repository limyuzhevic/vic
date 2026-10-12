#pragma once

#include "../Plugin.hpp"
#include "../../brain/Neuron.hpp"

namespace nlm {

class HodgkinHuxleyNeuronPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "HodgkinHuxleyNeuron",
            "Implementation of the Hodgkin-Huxley neuron model",
            "NLM Team",
            PluginVersion(1, 0, 0),
            PluginType::Neuron
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // Initialize Hodgkin-Huxley specific parameters
        gNa_max_ = 120.0f;  // Maximum sodium conductance
        gK_max_ = 36.0f;    // Maximum potassium conductance
        gL_ = 0.3f;         // Leak conductance
        Cm_ = 1.0f;         // Membrane capacitance
        
        // Create neuron type
        NeuronType hhType = NeuronType::Internal;
        setNeuronType(hhType);
        
        return true;
    }
    
    void enable() override {
        Plugin::enable();
        // Register extension point for Hodgkin-Huxley neuron creation
        if (getManager()) {
            getManager()->registerExtensionPoint("neuron_creation", PluginType::Neuron,
                [this](std::shared_ptr<Plugin>) { return true; });
        }
    }
    
    // Create a Hodgkin-Huxley neuron instance
    Neuron* createHodgkinHuxleyNeuron(NeuronId id) {
        // Create standard neuron
        Neuron* neuron = new Neuron(id);
        
        // Set Hodgkin-Huxley specific parameters
        neuron->setThreshold(-55.0f);  // Typical HH threshold
        neuron->setRestingPotential(-65.0f);  // Typical HH resting potential
        neuron->setResetPotential(-65.0f);    // Reset to resting potential
        neuron->setLeakConductance(0.3f);      // HH leak conductance
        
        // Enable HH-specific plasticity
        neuron->enablePlasticity(true, true, false);  // Hebbian and STDP
        
        return neuron;
    }
    
    // Get extension points provided by this plugin
    std::vector<std::string> getExtensionPoints() const override {
        return {"neuron_creation", "neuron_simulation", "plasticity_rules"};
    }
    
    // Get services provided by this plugin
    std::vector<std::string> getProvidedServices() const override {
        return {"hh_neuron_simulation", "ion_channel_modeling", "action_potential_generation"};
    }
    
    // Hodgkin-Huxley specific methods
    void setMaximumSodiumConductance(float gNa) { gNa_max_ = gNa; }
    float getMaximumSodiumConductance() const { return gNa_max_; }
    
    void setMaximumPotassiumConductance(float gK) { gK_max_ = gK; }
    float getMaximumPotassiumConductance() const { return gK_max_; }
    
    void setLeakConductance(float gL) { gL_ = gL; }
    float getLeakConductance() const { return gL_; }
    
    void setMembraneCapacitance(float Cm) { Cm_ = Cm; }
    float getMembraneCapacitance() const { return Cm_; }
    
private:
    float gNa_max_ = 120.0f;  // mS/cm²
    float gK_max_ = 36.0f;    // mS/cm²
    float gL_ = 0.3f;         // mS/cm²
    float Cm_ = 1.0f;         // μF/cm²
};

} // namespace nlm