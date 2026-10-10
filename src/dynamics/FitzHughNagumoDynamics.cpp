// FitzHughNagumoDynamics.cpp implementation
#include "FitzHughNagumoDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    // Integration parameters
    float timeStep;              // Simulation timestep (ms)
    float voltageSlope;          // Voltage change slope
    float maxVoltageChange;      // Maximum voltage change per timestep
    float recoverySlope;         // Recovery variable slope
    
    // State storage
    float prevVoltage;           // Previous voltage for stability
    float recoveryRate;          // Effective recovery rate
    
    // Time integration helpers
    float alpha;                 // Integration coefficient
    float beta;                  // Integration coefficient
    
    Impl() : timeStep(0.001f),  // Default 1ms timestep
             voltageSlope(1.0f),
             maxVoltageChange(50.0f),
             recoverySlope(1.0f),
             prevVoltage(0.0f),
             recoveryRate(0.0f),
             alpha(0.0f),
             beta(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : 
    pImpl(new Impl()),
    V_(V_rest), w_(0.0f) {  // Start at rest with zero recovery
}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt, Timestamp currentTime) {
    const float dt_sec = static_cast<float>(dt) / 1000.0f;  // Convert to seconds
    
    // Get current state
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    
    // External current (from current injection)
    float I_ext = neuron->getTotalCurrent();
    
    // Ensure external current is within biological bounds
    I_ext = std::clamp(I_ext, I_ext_min, I_ext_max);
    
    // FitzHugh-Nagumo equations:
    // dV/dt = (V - v0 - w + I_ext) / tau
    // dw/dt = epsilon * (V - a*w - b)
    
    // Calculate voltage dynamics
    float dV_dt = (V - v0_ - w_ + I_ext) / tau_;
    
    // Calculate recovery dynamics
    float dw_dt = epsilon_ * (V - a_ * w_ - b_);
    
    // Apply forward Euler integration
    float dV = dV_dt * dt_sec;
    float dw = dw_dt * dt_sec;
    
    // Limit voltage changes for stability
    dV = std::clamp(dV, -pImpl->maxVoltageChange * dt_sec, pImpl->maxVoltageChange * dt_sec);
    
    // Update state variables
    V += dV;
    w_ += dw;
    
    // Clamp voltage to biologically plausible range
    V = std::clamp(V, -10.0f, 10.0f);
    w_ = std::clamp(w_, -5.0f, 5.0f);
    
    // Update neuron membrane potential
    neuron->setMembranePotential(V);
    
    // Store state for external access
    V_ = V;
    
    // Check for spike (threshold crossing with hysteresis)
    if (V > threshold_ && pImpl->prevVoltage <= threshold_) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(currentTime);  // FIXED: Pass actual time
        
        // After spike, reset recovery variable (neuron "resets")
        w_ = w_ + 0.1f;  // Add small positive offset
        
        // Set membrane potential to reset value (often near resting potential)
        float resetPotential = -1.0f;  // FHN-specific reset
        neuron->setMembranePotential(resetPotential);
        V_ = resetPotential;
        
        // Set a refractory-like effect
        neuron->setRefractoryPeriod(2);  // 2 timestep refractory
    }
    
    // Handle refractory period if neuron is refractory
    if (neuron->isRefractory()) {
        // During refractory period, hold voltage at reset
        neuron->setMembranePotential(-1.5f);
        neuron->decrementRefractory();
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
    
    // Store previous voltage for spike detection
    pImpl->prevVoltage = V;
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Synaptic dynamics for FHN neurons
    // FHN model uses simpler synaptic transmission than HH
    
    const float tau_syn = 10.0f;  // Longer synaptic time constant for FHN
    float decayFactor = std::exp(-static_cast<float>(dt) / tau_syn);
    
    // Get current synaptic conductance
    float g_syn = synapse->getConductance();
    
    // Apply decay
    g_syn *= decayFactor;
    synapse->setConductance(g_syn);
    
    // FHN-specific: synaptic input affects recovery variable directly
    // This models the slower recovery dynamics in FHN neurons
    
    // Update synaptic weight based on plasticity (placeholder)
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // For FHN neurons, spike input affects both membrane potential and recovery
    
    SynapticWeight weight = synapse->getWeight();
    float conductance = synapse->getConductance();
    
    if (synapse->isExcitatory()) {
        // Excitatory input: increase membrane potential
        float deltaV = weight * 0.2f;  // Larger effect for FHN
        neuron->setMembranePotential(neuron->getMembranePotential() + deltaV);
        
        // Also increase recovery variable (neuron "works harder")
        w_ += deltaV * 0.1f;
        
        // Add to total current
        neuron->receiveExcitatoryInput(deltaV * 5.0f);
    } else {
        // Inhibitory input: decrease membrane potential
        float deltaV = std::abs(weight) * 0.2f;
        neuron->setMembranePotential(neuron->getMembranePotential() - deltaV);
        
        // Decrease recovery variable
        w_ -= deltaV * 0.1f;
        
        // Add inhibitory current
        neuron->receiveInhibitoryInput(deltaV * 5.0f);
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    // For FHN neurons, firing is determined by threshold crossing
    // Use hysteresis to prevent multiple firing in same timestep
    return neuron->getMembranePotential() > threshold_ &&
           neuron->getMembranePotential() > 0.0f;
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN model state variables
    V_ = V_rest;
    w_ = 0.0f;
    
    // Reset integration state
    pImpl->prevVoltage = V_rest;
    pImpl->alpha = 0.0f;
    pImpl->beta = 0.0f;
}

// Setters and getters for FHN-specific parameters
void FitzHughNagumoDynamics::setRestingPotential(float v0) { v0_ = v0; }
float FitzHughNagumoDynamics::getRestingPotential() const { return v0_; }

void FitzHughNagumoDynamics::setTimeConstant(float tau) { tau_ = tau; }
float FitzHughNagumoDynamics::getTimeConstant() const { return tau_; }

void FitzHughNagumoDynamics::setThreshold(float threshold) { threshold_ = threshold; }
float FitzHughNagumoDynamics::getThreshold() const { return threshold_; }

void FitzHughNagumoDynamics::setRecoveryRate(float epsilon) { epsilon_ = epsilon; }
float FitzHughNagumoDynamics::getRecoveryRate() const { return epsilon_; }

void FitzHughNagumoDynamics::setRecoverySlope(float a) { a_ = a; }
float FitzHughNagumoDynamics::getRecoverySlope() const { return a_; }

void FitzHughNagumoDynamics::setRecoveryOffset(float b) { b_ = b; }
float FitzHughNagumoDynamics::getRecoveryOffset() const { return b_; }

} // namespace nlm
