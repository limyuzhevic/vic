#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// Real biological neuron dynamics
// Implements Integrate-and-Fire (IAF) model with spike-frequency adaptation
// Based on biophysically realistic parameters
class RealIntegrateAndFireDynamics {
public:
    RealIntegrateAndFireDynamics();
    ~RealIntegrateAndFireDynamics();
    
    // Disable copying, enable moving
    RealIntegrateAndFireDynamics(const RealIntegrateAndFireDynamics&) = delete;
    RealIntegrateAndFireDynamics& operator=(const RealIntegrateAndFireDynamics&) = delete;
    RealIntegrateAndFireDynamics(RealIntegrateAndFireDynamics&&) noexcept;
    RealIntegrateAndFireDynamics& operator=(RealIntegrateAndFireDynamics&&) noexcept;
    
    // Update neuron membrane potential and handle spiking
    void updateNeuron(Neuron* neuron, TimestepDuration dt_ms);
    
    // Update synapse dynamics with short-term plasticity
    void updateSynapse(Synapse* synapse, TimestepDuration dt_ms);
    
    // Apply spike input from synapse to neuron
    void applySpikeInput(Neuron* neuron, const Synapse* synapse);
    
    // Check if neuron should fire based on current state
    bool shouldFire(const Neuron* neuron, MembranePotential V, MembranePotential threshold) const;
    
    // Reset internal dynamics state
    void reset();
    
    // Configure biophysical parameters
    void setMembraneTimeConstant(float tau);  // ms (default: 20.0f)
    void setMembraneResistance(float R);      // MOhm (default: 10.0f)
    void setRestingPotential(float V_rest);   // mV (default: -70.0f)
    void setThresholdPotential(float V_th);   // mV (default: -55.0f)
    void setResetPotential(float V_reset);    // mV (default: -65.0f)
    void setAdaptationConductance(float gA); // nS (default: 5.0f)
    void setAdaptationTimeConstant(float tauA); // ms (default: 100.0f)
    
    // Get current biophysical parameters
    float getMembraneTimeConstant() const;
    float getMembraneResistance() const;
    float getRestingPotential() const;
    float getThresholdPotential() const;
    float getResetPotential() const;
    float getAdaptationConductance() const;
    float getAdaptationTimeConstant() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    static float getRandomNoise();
};

} // namespace nlm
