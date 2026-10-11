#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralSemanticMemory: Integrates semantic knowledge with neural dynamics
// NOT traditional symbolic knowledge - neural pattern-based semantic representation
//
// Key mechanisms:
// - Neural pattern storage and retrieval
// - Pattern similarity computation
// - Associative consolidation
// - Development-dependent learning modulation

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store fact with neural pattern
    // Pattern represents the neural activation signature of this knowledge
    void storeFact(const std::string& key, const std::string& value, const std::vector<float>& pattern);

    // Retrieve fact by key
    std::string retrieveFact(const std::string& key) const;

    // Check if fact exists
    bool hasFact(const std::string& key) const;

    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;

    // Clear all semantic knowledge
    void clear();

    // Update semantic network with new patterns
    void update(const std::vector<std::vector<float>>& patterns, float learningRate);

    // Decay weak semantic traces
    void decay(float decayRate);

    // Get pattern similarity for consolidation
    void consolidateWithRelatedPatterns(size_t newIndex);

    // Find most similar pattern to query
    size_t findMostSimilarPattern(const std::vector<float>& pattern) const;

    // Store fact from specific neuron activation
    void storeFactFromNeuron(NeuronId neuron, const std::string& key);

    // Activate similar patterns for pattern completion
    std::vector<float> activateSimilarPatterns(const std::vector<float>& pattern) const;

    // Set consolidation threshold
    void setConsolidationThreshold(float threshold);

    // Set developmental modulation factor
    void setDevelopmentalModulation(float modulation);

    // Get current consolidation threshold
    float getConsolidationThreshold() const;

    // Get developmental modulation factor
    float getDevelopmentalModulation() const;

    // Calculate similarity between two patterns
    static float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                           const std::vector<float>& pattern2);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Convert neuron activation to neural pattern
    void neuronActivationToNeuralPattern(const NeuronId& neuron, std::vector<float>& pattern) const;
};

} // namespace nlm
