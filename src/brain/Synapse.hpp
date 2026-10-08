#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse implementation details (Pimpl idiom)
struct Synapse::Impl;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
class Synapse {
public:
    // Create synapse with IDs
    explicit Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
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
    
    // Plasticity flags
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const noexcept;
    [[nodiscard]] PlasticityFlags& getPlasticityFlags() noexcept;
    
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
    void initializeRandom(class RandomGenerator& rng);
    
private:
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

// Forward declaration needed for Pimpl implementation
namespace nlm {
    struct Synapse::Impl {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        SynapticWeight weight;
        Delay delay;
        SynapseType type;
        std::vector<Timestamp> preSpikeHistory;
        std::vector<Timestamp> postSpikeHistory;
        PlasticityFlags plasticityFlags;
        float eligibilityTrace;
        float efficacy;
        
        Impl() noexcept : id(), sourceNeuron(), destinationNeuron(), weight(1.0f),
                          delay(1), type(SynapseType::Excitatory),
                          eligibilityTrace(0.0f), efficacy(1.0f) {}
        
        // Move constructor
        Impl(Impl&& other) noexcept
            : id(std::move(other.id)), sourceNeuron(std::move(other.sourceNeuron)),
              destinationNeuron(std::move(other.destinationNeuron)), weight(std::move(other.weight)),
              delay(std::move(other.delay)), type(std::move(other.type)),
              preSpikeHistory(std::move(other.preSpikeHistory)),
              postSpikeHistory(std::move(other.postSpikeHistory)),
              plasticityFlags(std::move(other.plasticityFlags)),
              eligibilityTrace(std::move(other.eligibilityTrace)),
              efficacy(std::move(other.efficacy)) {}
        
        // Disable copy
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
    };
}
