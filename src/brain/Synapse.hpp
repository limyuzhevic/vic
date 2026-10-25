#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse final {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept = default;
    
    // Identity
    [[nodiscard]] SynapseId getId() const noexcept;
    
    // Connection
    [[nodiscard]] NeuronId getSourceNeuron() const noexcept;
    [[nodiscard]] NeuronId getDestinationNeuron() const noexcept;
    
    // Weight
    [[nodiscard]] SynapticWeight getWeight() const noexcept;
    void setWeight([[maybe_unused]] SynapticWeight weight) noexcept;
    void addToWeight([[maybe_unused]] SynapticWeight delta) noexcept;
    
    // Delay (in simulation steps)
    [[nodiscard]] Delay getDelay() const noexcept;
    void setDelay([[maybe_unused]] Delay delay) noexcept;
    
    // Type
    [[nodiscard]] SynapseType getType() const noexcept;
    void setType([[maybe_unused]] SynapseType type) noexcept;
    
    // Excitatory/Inhibitory
    [[nodiscard]] bool isExcitatory() const noexcept;
    [[nodiscard]] bool isInhibitory() const noexcept;
    
    // Activity history for plasticity
    void recordPreSpike([[maybe_unused]] Timestamp timestamp) noexcept;
    void recordPostSpike([[maybe_unused]] Timestamp timestamp) noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getPreSpikeHistory() const noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getPostSpikeHistory() const noexcept;
    void clearHistory() noexcept;
    
    // Plasticity flags
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const noexcept;
    [[nodiscard]] PlasticityFlags& getPlasticityFlags() noexcept;
    
    // Enable/disable plasticity rules
    void enablePlasticity([[maybe_unused]] bool hebbian, [[maybe_unused]] bool stdp, [[maybe_unused]] bool rewardModulated) noexcept;
    
    // Eligibility trace (for reward-modulated learning)
    [[nodiscard]] float getEligibilityTrace() const noexcept;
    void setEligibilityTrace([[maybe_unused]] float trace) noexcept;
    void decayEligibilityTrace([[maybe_unused]] float decayRate) noexcept;
    
    // Synaptic efficacy (use-dependent modulation)
    [[nodiscard]] float getEfficacy() const noexcept;
    void setEfficacy([[maybe_unused]] float efficacy) noexcept;
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step([[maybe_unused]] Timestamp currentTime) noexcept;
    
    // Reset to initial state
    void reset() noexcept;
    
    // Initialize with random parameters
    void initializeRandom([[maybe_unused]] class RandomGenerator& rng) noexcept;
    
private:
    struct Impl;
    [[maybe_unused]] Impl* pImpl;
};

} // namespace nlm
