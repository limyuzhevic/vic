#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include <array>
#include <cmath>

namespace nlm {

// Neural dynamics configuration parameters
struct NeuralDynamicsConfig {
    // Common parameters
    float simulationTemperature;  // Temperature in Celsius (for rate constants)
    float voltageClamp;           // Clamping voltage for voltage clamp experiments
    bool enableStochasticity;     // Enable stochastic ion channel opening
    float noiseLevel;             // Level of stochastic noise
    
    // HH-specific parameters
    float hhsodiumConductance;    // Maximum sodium conductance (mS/cm^2)
    float hhsodiumReversal;       // Sodium reversal potential (mV)
    float hkpotassiumConductance; // Maximum potassium conductance (mS/cm^2)
    float hkpotassiumReversal;    // Potassium reversal potential (mV)
    float hhleakConductance;      // Leak conductance (mS/cm^2)
    float hlleakReversal;         // Leak reversal potential (mV)
    
    // FHN-specific parameters
    float fhnthreshold;           // Threshold voltage (mV)
    float fhngradient;            // Gradient parameter (0.7-0.8 for excitable)
    float fhnbaseline;            // Baseline parameter (0.7 for excitable)
    float fhnrecoveryTime;        // Recovery time constant
    
    // Synaptic parameters
    float synapticWeightMax;      // Maximum synaptic weight
    float synapticWeightMin;      // Minimum synaptic weight
    float synapticTau;            // Synaptic time constant
    bool enableSTDP;             // Enable spike-timing dependent plasticity
    float stdpLearningRate;       // STDP learning rate
    
    // Integration parameters
    bool useVectorization;        // Enable vectorized operations
    bool useAdaptiveStep;         // Enable adaptive time stepping
    float stabilityThreshold;     // Numerical stability threshold
};

// Base dynamics configuration
struct DynamicsParameters {
    float temperature;            // Temperature (C)
    float restingPotential;       // Resting membrane potential (mV)
    float threshold;              // Firing threshold (mV)
    float resetPotential;         // Reset potential (mV)
    float refractoryPeriod;       // Refractory period (ms)
};

// Abstract base class for neural dynamics
// PLACEHOLDER - Phase 2 will implement various dynamics models

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    // TODO PHASE 2: Implement real dynamics
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    // TODO PHASE 2: Implement real synaptic dynamics
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state
    virtual void reset() = 0;
    
    // Load configuration from Config object
    virtual void loadConfig(const Config& config) = 0;
    
    // Get dynamics model name
    virtual const char* getName() const = 0;
};

// Integrate-and-fire dynamics (simple model)
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed model)
// TODO PHASE 2: Implement HH dynamics with proper channel dynamics
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement HH dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Load configuration from Config
    void loadConfig(const Config& config) override;
    
    // Get model name
    const char* getName() const override;
    
    // HH-specific parameters
    void setSodiumConductance(float gNa);
    void setPotassiumConductance(float gK);
    void setLeakConductance(float gL);
    void setTemperature(float temperature);
    float getSodiumConductance() const;
    float getPotassiumConductance() const;
    float getLeakConductance() const;
    float getTemperature() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// FitzHugh-Nagumo dynamics (simplified兴奋性模型)
// TODO PHASE 2: Implement FHN dynamics with recovery and threshold behaviors
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement FHN dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Load configuration from Config
    void loadConfig(const Config& config) override;
    
    // Get model name
    const char* getName() const override;
    
    // FHN-specific parameters
    void setThreshold(float threshold);
    void setGradient(float gradient);
    void setBaseline(float baseline);
    void setRecoveryTime(float recoveryTime);
    float getThreshold() const;
    float getGradient() const;
    float getBaseline() const;
    float getRecoveryTime() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
