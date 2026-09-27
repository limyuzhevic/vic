#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <optional>
#include <variant>
#include <expected>
#include <memory>
#include <type_traits>
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;
class PlasticityRule;

// Neuron implementation using Pimpl with modern C++ patterns
class Neuron {
    struct Impl;
    std::unique_ptr<Impl> pImpl;  // Smart pointer for RAII
    
    // Rule of five: proper resource management
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept = default;
    Neuron& operator=(Neuron&&) noexcept = default;
    
public:
    // Constructor with optional parameters using initializer list
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Identity and type information
    [[nodiscard]] NeuronId getId() const noexcept;
    NeuronType getType() const noexcept;
    void setType(NeuronType type) noexcept;
    
    // State management with structured bindings support
    [[nodiscard]] const NeuronState& getState() const noexcept;
    NeuronState& getState() noexcept;
    
    // Membrane potential operations
    [[nodiscard]] MembranePotential getMembranePotential() const noexcept;
    void setMembranePotential(MembranePotential potential) noexcept;
    void addToMembranePotential(MembranePotential delta) noexcept;
    
    // Threshold management
    [[nodiscard]] MembranePotential getThreshold() const noexcept;
    void setThreshold(MembranePotential threshold) noexcept;
    
    // Firing state management
    [[nodiscard]] bool isFiring() const noexcept;
    [[nodiscard]] bool isRefractory() const noexcept;
    void setFiringState(FiringState state) noexcept;
    void setRefractoryPeriod(uint32_t steps) noexcept;
    void decrementRefractory() noexcept;
    
    // Firing rate management
    [[nodiscard]] FiringRate getFiringRate() const noexcept;
    void setFiringRate(FiringRate rate) noexcept;
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance) noexcept;
    [[nodiscard]] MembranePotential getLeakConductance() const noexcept;
    [[nodiscard]] uint32_t getRefractoryPeriod() const noexcept;
    void setRestingPotential(MembranePotential potential) noexcept;
    [[nodiscard]] MembranePotential getRestingPotential() const noexcept;
    void setResetPotential(MembranePotential potential) noexcept;
    
    // Spike detection and management
    [[nodiscard]] bool checkThreshold() const noexcept;
    [[nodiscard]] float getLastSpikeTime() const noexcept;
    
    // Enhanced LIF dynamics with modern C++ algorithms
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Enhanced input handling with overload resolution
    void receiveInput(const std::string& inputType, MembranePotential amplitude) noexcept;
    void receiveExcitatoryInput(MembranePotential amplitude) noexcept;
    void receiveInhibitoryInput(MembranePotential amplitude) noexcept;
    void receiveModulatoryInput(MembranePotential amplitude) noexcept;
    
    // External current injection
    void injectCurrent(MembranePotential current) noexcept;
    [[nodiscard]] MembranePotential getTotalCurrent() const noexcept;
    void clearTotalCurrent() noexcept;
    
    // Enhanced spike history with range-based for loop support
    void recordSpike(Timestamp timestamp);
    [[nodiscard]] const std::vector<Timestamp>& getSpikeHistory() const noexcept;
    void clearSpikeHistory() noexcept;
    
    // Synapse management using modern container patterns
    void addIncomingSynapse(SynapseHandle handle) noexcept;
    void addOutgoingSynapse(SynapseHandle handle) noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getIncomingSynapses() const noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getOutgoingSynapses() const noexcept;
    
    // Plasticity system integration
    void setPlasticityRule(std::unique_ptr<PlasticityRule> rule) noexcept;
    void updatePlasticity(const SynapseId& synapseId, const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    
    // Plasticity state management
    const PlasticityFlags& getPlasticityFlags() const noexcept;
    PlasticityFlags& getPlasticityFlags() noexcept;
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) noexcept;
    
    // Region/population membership
    void setRegionId(RegionId region) noexcept;
    [[nodiscard]] RegionId getRegionId() const noexcept;
    void setPopulationId(PopulationId population) noexcept;
    [[nodiscard]] PopulationId getPopulationId() const noexcept;
    
    // Simulation step with error handling
    void step(Timestamp currentTime);
    
    // State management
    void reset() noexcept;
    void initializeRandom(RandomGenerator& rng);
    
    // Factory pattern support
    static std::expected<std::unique_ptr<Neuron>, ConfigError> createNeuron(
        NeuronId id, NeuronType type = NeuronType::Internal,
        const std::unordered_map<std::string, std::string>& params = {});
    
    // Builder pattern support
    class Builder {
        NeuronId id;
        NeuronType type = NeuronType::Internal;
        std::unordered_map<std::string, std::string> params;
        
    public:
        Builder& withId(NeuronId neuronId) { id = neuronId; return *this; }
        Builder& withType(NeuronType neuronType) { type = neuronType; return *this; }
        Builder& withParam(const std::string& key, const std::string& value) {
            params[key] = value; return *this;
        }
        
        std::expected<std::unique_ptr<Neuron>, ConfigError> build() {
            return Neuron::createNeuron(id, type, params);
        }
    };
    
    [[nodiscard]] static Builder buildNeuron() { return Builder(); }
};

} // namespace nlm
