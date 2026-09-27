#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <optional>
#include <expected>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

// Forward declaration
class Neuron;
class PlasticityRule;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
class Synapse {
    struct Impl;
    std::unique_ptr<Impl> pImpl;  // Smart pointer for RAII
    
    // Rule of five: proper resource management
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept = default;
    Synapse& operator=(Synapse&&) noexcept = default;
    
public:
    // Create synapse with IDs using optional parameters
    explicit Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Identity and connection management
    [[nodiscard]] SynapseId getId() const noexcept;
    [[nodiscard]] NeuronId getSourceNeuron() const noexcept;
    [[nodiscard]] NeuronId getDestinationNeuron() const noexcept;
    
    // Weight management with bounds checking
    [[nodiscard]] SynapticWeight getWeight() const noexcept;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay management
    [[nodiscard]] Delay getDelay() const noexcept;
    void setDelay(Delay delay) noexcept;
    
    // Connection type management
    [[nodiscard]] SynapseType getType() const noexcept;
    void setType(SynapseType type) noexcept;
    
    // Connection properties
    [[nodiscard]] bool isExcitatory() const noexcept;
    [[nodiscard]] bool isInhibitory() const noexcept;
    [[nodiscard]] bool isModulatory() const noexcept;
    
    // Activity history for plasticity (using modern container patterns)
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    [[nodiscard]] const std::vector<Timestamp>& getPreSpikeHistory() const noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getPostSpikeHistory() const noexcept;
    void clearHistory() noexcept;
    
    // Plasticity system integration
    void setPlasticityRule(std::unique_ptr<PlasticityRule> rule) noexcept;
    void updatePlasticity(Timestamp currentTime, TimestepDuration dt);
    
    // Plasticity state management
    const PlasticityFlags& getPlasticityFlags() const noexcept;
    PlasticityFlags& getPlasticityFlags() noexcept;
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) noexcept;
    
    // Eligibility trace for reward-modulated learning
    [[nodiscard]] float getEligibilityTrace() const noexcept;
    void setEligibilityTrace(float trace) noexcept;
    void decayEligibilityTrace(float decayRate) noexcept;
    
    // Synaptic efficacy (use-dependent modulation)
    [[nodiscard]] float getEfficacy() const noexcept;
    void setEfficacy(float efficacy) noexcept;
    
    // Real synaptic dynamics
    void step(Timestamp currentTime);
    
    // State management
    void reset() noexcept;
    void initializeRandom(class RandomGenerator& rng);
    
    // Factory pattern support
    static std::expected<std::unique_ptr<Synapse>, ConfigError> createSynapse(
        SynapseId id, NeuronId source, NeuronId destination,
        SynapseType type = SynapseType::Excitatory,
        const std::unordered_map<std::string, std::string>& params = {});
    
    // Builder pattern support
    class Builder {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        SynapseType type = SynapseType::Excitatory;
        std::unordered_map<std::string, std::string> params;
        
    public:
        Builder& withId(SynapseId synapseId) { id = synapseId; return *this; }
        Builder& withSource(NeuronId src) { sourceNeuron = src; return *this; }
        Builder& withDestination(NeuronId dst) { destinationNeuron = dst; return *this; }
        Builder& withType(SynapseType synapseType) { type = synapseType; return *this; }
        Builder& withParam(const std::string& key, const std::string& value) {
            params[key] = value; return *this;
        }
        
        std::expected<std::unique_ptr<Synapse>, ConfigError> build() {
            return Synapse::createSynapse(id, sourceNeuron, destinationNeuron, type, params);
        }
    };
    
    [[nodiscard]] static Builder buildSynapse() { return Builder(); }
    
    // Error types for modern C++ error handling
    enum class SynapseError {
        InvalidWeight,
        InvalidDelay,
        InvalidType,
        ConnectionError,
        PlasticityError,
        InitializationError
    };
};

} // namespace nlm
