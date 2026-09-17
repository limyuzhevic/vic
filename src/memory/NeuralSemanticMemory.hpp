#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace nlm {

// NeuralSemanticMemory: Stores learned knowledge as neural activity patterns
// NOT a simple key-value store - patterns are encoded in synaptic weights and neuronal assemblies
//
// Key mechanisms:
// - Knowledge encoded as distributed neural activity patterns
// - Pattern completion via recurrent connections
// - Associative learning (Hebbian) to strengthen relevant connections
// - Interference management to prevent memory corruption
// - Semantic consolidation through replay and rehearsal

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store knowledge as a neural pattern
    // The knowledge is encoded in synaptic connections and neuron activations
    void storeFact(const std::string& key, const std::vector<float>& pattern);

    // Retrieve knowledge as neural pattern
    // Uses pattern completion to retrieve full pattern from partial cues
    std::vector<float> retrieveFact(const std::string& key) const;

    // Check if knowledge exists
    bool hasFact(const std::string& key) const;

    // Get all knowledge patterns
    std::vector<std::pair<std::string, std::vector<float>>> getAllFacts() const;

    // Update knowledge pattern
    void updateFact(const std::string& key, const std::vector<float>& pattern);

    // Delete knowledge
    void deleteFact(const std::string& key);

    void updateFact(const std::string& key, const std::vector<float>& pattern);

    // Delete knowledge
    void deleteFact(const std::string& key);

    // Get neuron activations for a fact
    std::vector<NeuronId> getFactNeurons(const std::string& key) const;

    // Get activation strength for a neuron in fact representation
    float getFactActivation(const std::string& key, NeuronId neuron) const;

    // Create recurrent connections for pattern maintenance
    void createFactRecurrentConnection(const std::string& key);

    // Decay weak fact representations
    void decayWeakFacts();

    // Resolve interference between similar patterns
    void resolveInterference();

    // Get number of knowledge facts
    size_t getFactCount() const { return pImpl->patterns.size(); }

    // Get capacity
    size_t getCapacity() const { return pImpl->capacity_; }
    void setCapacity(size_t cap) { pImpl->capacity_ = cap; }

    // Update memory (consolidation, decay, interference management)
    void update(TimestepDuration dt);

    // Clear all knowledge
    void clear();

    // Get neuron activations for a fact
    std::vector<NeuronId> getFactNeurons(const std::string& key) const;

    // Get activation strength for a neuron in fact representation
    float getFactActivation(const std::string& key, NeuronId neuron) const;

private:
    // Store neural pattern for a fact
    void storeNeuralPattern(const std::string& key, const std::vector<float>& pattern);

    // Retrieve neurons associated with a fact
    std::vector<NeuronId> getAssociatedNeurons(const std::string& key) const;

    // Create recurrent connections for pattern maintenance
    void createFactRecurrentConnection(const std::string& key);

    // Decay weak fact representations
    void decayWeakFacts();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

} // namespace nlm