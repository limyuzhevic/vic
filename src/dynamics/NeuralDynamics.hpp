#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

// Abstract base class for neural dynamics with comprehensive interface
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state
    virtual void reset() = 0;
    
    // Set temperature in Kelvin (affects ion channel kinetics)
    virtual void setTemperature(float temperature) = 0;
    
    // Get current temperature
    virtual float getTemperature() const = 0;
    
    // Set random seed for reproducible simulations
    virtual void setRandomSeed(unsigned int seed) = 0;
    
    // Get noise level for stochastic models
    virtual float getNoiseLevel() const = 0;
    virtual void setNoiseLevel(float noise) = 0;
};

// Ion channel state for Hodgkin-Huxley models
struct IonChannelState {
    float m;    // Sodium activation
    float h;    // Sodium inactivation
    float n;    // Potassium activation
    float ca;   // Calcium activation
    float d;    // Calcium decay
    float f;    // Calcium inactivation
    
    IonChannelState() : m(0.05f), h(0.6f), n(0.32f), ca(0.01f), d(0.04f), f(0.1f) {}
};

// Synaptic conductance dynamics for alpha synapses
struct SynapticConductance {
    float g;      // Maximum conductance (nS)
    float tau_r;  // Rise time constant (ms)
    float tau_d;  // Decay time constant (ms)
    float voltage; // Current conductance (nS)
    float derivative; // dV/dt
    
    SynapticConductance() : g(1.0f), tau_r(0.1f), tau_d(2.0f), voltage(0.0f), derivative(0.0f) {}
};

// Enhanced integrate-and-fire dynamics with biophysical accuracy
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    void setTemperature(float temperature) override;
    float getTemperature() const override;
    void setRandomSeed(unsigned int seed) override;
    float getNoiseLevel() const override;
    void setNoiseLevel(float noise) override;
    
    // Ion channel parameters
    void setSodiumConductance(float g_na);
    void setPotassiumConductance(float g_k);
    void setLeakConductance(float g_l);
    void setCalciumConductance(float g_ca);
    
    // Membrane capacitance
    void setMembraneCapacitance(float capacitance);
    
    // Refractory period parameters
    void setRefractoryPeriod(float period);
    void setAdaptationConductance(float g_adapt);
    
    // Dendritic parameters
    void setDendriticResistivity(float resistivity);
    void setDendriticCapacitance(float capacitance);
    
    // Synaptic parameters
    void setReversalPotential(float v_exc, float v_inh, float v_leak);
    void setSynapticTimeConstants(float tau_rise, float tau_decay);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Complete Hodgkin-Huxley dynamics with multiple ion channels
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    void setTemperature(float temperature) override;
    float getTemperature() const override;
    void setRandomSeed(unsigned int seed) override;
    float getNoiseLevel() const override;
    void setNoiseLevel(float noise) override;
    
    // HH specific parameters
    void setSodiumMaxConductance(float g_na_max);
    void setPotassiumMaxConductance(float g_k_max);
    void setLeakConductance(float g_l);
    void setSodiumReversalPotential(float v_na);
    void setPotassiumReversalPotential(float v_k);
    void setLeakReversalPotential(float v_l);
    void setCalciumReversalPotential(float v_ca);
    
    // Channel kinetics parameters
    void setSodiumActivationTau(float tau_m);
    void setSodiumInactivationTau(float tau_h);
    void setPotassiumActivationTau(float tau_n);
    
    // Adaptation parameters
    void setAdaptationStrength(float alpha);
    void setAdaptationTimeConstant(float tau_adapt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// FitzHugh-Nagumo dynamics (simplified excitable system)
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    void setTemperature(float temperature) override;
    float getTemperature() const override;
    void setRandomSeed(unsigned int seed) override;
    float getNoiseLevel() const override;
    void setNoiseLevel(float noise) override;
    
    // FHN parameters
    void setExcitationThreshold(float threshold);
    void setRecoveryTimeConstant(float tau);
    void setCouplingStrength(float epsilon);
    void setBaselineCurrent(float current);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Multi-channel neuron type for specialized dynamics
enum class NeuronModelType {
    LIF,
    HH,
    FHN,
    AdaptiveExponential,
    QuadriModal
};

// Temperature dependence utilities
namespace Thermal {
    static constexpr float Q10 = 3.0f;  // Temperature sensitivity coefficient
    static constexpr float BASE_TEMPERATURE = 310.15f;  // 37°C in Kelvin
    
    inline float q10TemperatureFactor(float T, float T0 = BASE_TEMPERATURE) {
        return std::pow(Q10, (T - T0) / 10.0f);
    }
    
    inline float sigmoid(float x, float slope = 0.1f, float center = 0.0f) {
        return 1.0f / (1.0f + std::exp(-slope * (x - center)));
    }
}

} // namespace nlm