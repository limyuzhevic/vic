#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class NeuronState {
public:
    NeuronState();
    
    // State access
    const NeuronState& getState() const { return state_; }
    NeuronState& getState() { return state_; }
    
    // Membrane potential
    MembranePotential getMembranePotential() const { return state_.membranePotential; }
    void setMembranePotential(MembranePotential potential) { state_.membranePotential = potential; }
    void addToMembranePotential(MembranePotential delta) { state_.membranePotential += delta; }
    
    // Threshold
    MembranePotential getThreshold() const { return state_.threshold; }
    void setThreshold(MembranePotential threshold) { state_.threshold = threshold; }
    
    // Firing state
    bool isFiring() const { return state_.firingState == FiringState::Active || state_.firingState == FiringState::Refractory; }
    bool isRefractory() const { return state_.firingState == FiringState::Refractory; }
    void setFiringState(FiringState state) { state_.firingState = state; }
    void setRefractoryPeriod(uint32_t steps) { state_.refractoryPeriod = steps; }
    void decrementRefractory() { 
        if (state_.refractoryRemaining > 0) {
            --state_.refractoryRemaining;
            if (state_.refractoryRemaining == 0) {
                state_.firingState = FiringState::Resting;
            }
        }
    }
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const { return state_.firingRate; }
    void setFiringRate(FiringRate rate) { state_.firingRate = rate; }
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance) { state_.leakConductance = conductance; }
    MembranePotential getLeakConductance() const { return state_.leakConductance; }
    uint32_t getRefractoryPeriod() const { return state_.refractoryPeriod; }
    void setRestingPotential(MembranePotential potential) { state_.restingPotential = potential; }
    MembranePotential getRestingPotential() const { return state_.restingPotential; }
    void setResetPotential(MembranePotential potential) { state_.resetPotential = potential; }
    
    // Spike detection
    bool checkThreshold() const { return state_.membranePotential >= state_.threshold; }
    float getLastSpikeTime() const { return state_.lastSpikeTime; }
    void setLastSpikeTime(float time) { state_.lastSpikeTime = time; }
    
    // Access the underlying state for direct manipulation
    const NeuronState& getStateInternal() const { return state_; }
    NeuronState& getStateInternal() { return state_; }
    
private:
    NeuronState state_;
};

} // namespace nlm