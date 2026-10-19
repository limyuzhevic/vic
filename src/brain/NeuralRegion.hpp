#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class RandomGenerator;

class NeuralRegion {
public:
    NeuralRegion(uint64_t id);
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Getters
    uint64_t getId() const;
    std::string getName() const;
    void setName(const std::string& name);
    
    // Statistics
    size_t getNeuronCount() const;
    size_t getSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    
    // Neuron management
    void addNeuron(std::unique_ptr<Neuron> neuron);
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    const std::vector<std::unique_ptr<Neuron>>& getNeurons() const;
    
    // Synapse management
    void addSynapse(std::unique_ptr<Synapse> synapse);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    bool hasSynapse(NeuronId source, NeuronId target) const;
    void removeSynapse(SynapseId id);
    
    // State management
    void clear();
    void initialize(RandomGenerator& rng);
    void update(float dt, RandomGenerator& rng);
    void logStatus() const;
    
    // Inter-region connections
    void connectToRegion(NeuralRegion* targetRegion, RandomGenerator& rng);
    
    // Internal utilities
    NeuronId getNeuronId(size_t index) const;
    uint64_t createUniqueId();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
