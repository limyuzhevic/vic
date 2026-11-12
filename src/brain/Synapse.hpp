#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    [[nodiscard]] SynapseId getId() const noexcept;
    
    // Connection
    [[nodiscard]] NeuronId getSourceNeuron() const noexcept;
    [[nodiscard]] NeuronId getDestinationNeuron() const noexcept;
    
    // Weight
    [[nodiscard]] SynapticWeight getWeight() const noexcept;
    void setWeight(SynapticWeight weight) noexcept;
    void addToWeight(SynapticWeight delta) noexcept;
    
    // Delay (in simulation steps)
    [[nodiscard]] Delay getDelay() const noexcept;
    void setDelay(Delay delay) noexcept;
    
    // Type
    [[nodiscard]] SynapseType getType() const noexcept;
    void setType(SynapseType type) noexcept;
    
    // Excitatory/Inhibitory
    [[nodiscard]] bool isExcitatory() const noexcept;
    [[nodiscard]] bool isInhibitory() const noexcept;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp) noexcept;
    void recordPostSpike(Timestamp timestamp) noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getPreSpikeHistory() const noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getPostSpikeHistory() const noexcept;
    void clearHistory() noexcept;
    // Phase 5 checkpoint support: restore histories recorded by
    // recordPre/PostSpike (capped at the same MAX_SPIKE_HISTORY).
    void setPreSpikeHistory(const std::vector<Timestamp>& history) noexcept;
    void setPostSpikeHistory(const std::vector<Timestamp>& history) noexcept;
    
    // Plasticity flags
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const noexcept;
    PlasticityFlags& getPlasticityFlags() noexcept;
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) noexcept;
    
    // Eligibility trace (for reward-modulated learning)
    [[nodiscard]] float getEligibilityTrace() const noexcept;
    void setEligibilityTrace(float trace) noexcept;
    void decayEligibilityTrace(float decayRate) noexcept;
    
    // Synaptic efficacy (use-dependent modulation)
    [[nodiscard]] float getEfficacy() const noexcept;
    void setEfficacy(float efficacy) noexcept;
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime) noexcept;
    
    // Reset to initial state
    void reset() noexcept;
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng) noexcept;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm