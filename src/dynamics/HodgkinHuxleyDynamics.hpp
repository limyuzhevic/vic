#pragma once

#include "NeuralDynamics.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <map>

namespace nlm {

// Hodgkin-Huxley dynamics implementation
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
    // HH-specific parameters
    void setThresholdVoltage(float V_th) { thresholdVoltage = V_th; }
    void setRestingVoltage(float V_rest) { restingVoltage = V_rest; }
    void setCapacitance(float C_m) { capacitance = C_m; }
    void setConductance(float g_Na_max, float g_K_max, float g_L) { 
        g_Na_max_ = g_Na_max; g_K_max_ = g_K_max; g_L_ = g_L; }
    
    float getThresholdVoltage() const { return thresholdVoltage; }
    float getRestingVoltage() const { return restingVoltage; }
    float getCapacitance() const { return capacitance; }
    
    // HH gating variables
    float getSodiumActivation() const { return m_; }
    float getSodiumInactivation() const { return h_; }
    float getPotassiumActivation() const { return n_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // HH parameters
    float thresholdVoltage;
    float restingVoltage;
    float capacitance;
    float g_Na_max_;
    float g_K_max_;
    float g_L_;
    
    // HH gating variables
    float m_;  // Sodium activation
    float h_;  // Sodium inactivation
    float n_;  // Potassium activation
    
    // Time constants for gating variables
    float tau_m_;
    float tau_h_;
    float tau_n_;
};

// FitzHugh-Nagumo dynamics implementation
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setTimeConstant(float tau) { timeConstant = tau; }
    void setSensitivity(float beta) { sensitivity = beta; }
    float getTimeConstant() const { return timeConstant; }
    void setRecoveryVariableTimeConstant(float tau_r) { recoveryTimeConstant = tau_r; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // FHN parameters
    float timeConstant;
    float sensitivity;
    float recoveryTimeConstant;
    
    // FHN state variables
    float v_;    // Membrane potential
    float w_;    // Recovery variable
};

} // namespace nlm