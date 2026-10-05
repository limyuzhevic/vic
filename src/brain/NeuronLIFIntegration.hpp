// Neuron LIF Integration System
// Handles the detailed Leaky Integrate-and-Fire computation
#ifndef NEURON_LIF_INTEGRATION_HPP
#define NEURON_LIF_INTEGRATION_HPP

#include "Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class NeuronLIFIntegration {
public:
    NeuronLIFIntegration(Neuron* neuron) : neuron(neuron) {}
    
    // Execute LIF integration for one timestep
    bool integrate(Timestamp currentTime, TimestepDuration dt);
    
    // Handle refractory period
    void handleRefractoryPeriod();
    
    // Update membrane potential using LIF equation
    void updateMembranePotential(Timestamp currentTime, TimestepDuration dt);
    
    // Apply spike-frequency adaptation
    void applyAdaptation();
    
    // Check if neuron should fire
    bool checkForSpike() const;
    
    // Handle spike event
    void handleSpike(Timestamp currentTime);
    
    // Clear integration state
    void clearIntegrationState();
    
private:
    Neuron* neuron;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    
    // Helper methods
    MembranePotential calculateLeakContribution() const;
    MembranePotential calculateSynapticContribution() const;
    MembranePotential clampMembranePotential(MembranePotential V) const;
};

} // namespace nlm

#endif // NEURON_LIF_INTEGRATION_HPP