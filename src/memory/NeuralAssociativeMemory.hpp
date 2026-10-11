#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <memory>
#include <queue>

namespace nlm {

// NeuralAssociativeMemory: Stores associations between neural patterns using Hebbian learning
// NOT traditional symbolic associations - neural pattern-based relationship representation
//
// Key mechanisms:
// - Hebbian learning for pattern co-activation
// - Pattern-based associations instead of symbolic pairs
// - Dynamic association strength adjustment
// - Pattern consolidation and decay

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neuron patterns
    void associate(NeuronId a, NeuronId b, float strength);

    // Create association between entire neural patterns
    void associatePatterns(const std::vector<NeuronId>& patternA,
                           const std::vector<NeuronId>& patternB,
                           float baseStrength = 0.5f);

    // Get neurons associated with given neuron
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;

    // Get association strength between two neurons
    float getAssociationStrength(NeuronId a, NeuronId b) const;

    // Update association strength based on new experience
    void updateAssociation(const std::vector<NeuronId>& pattern1,
                           const std::vector<NeuronId>& pattern2,
                           float delta);

    // Update associative memory with new patterns and rewards
    void update(const std::vector<NeuronId>& patternA,
                const std::vector<NeuronId>& patternB,
                float learningRate);

    // Decay weak associations over time
    void decay(float decayRate);

    // Update pattern activations based on current state
    void updatePatternActivations(const std::vector<NeuronId>& pattern,
                                  float activationLevel);

    // Remove weak associations
    void pruneWeakAssociations(float threshold);

    // Rebuild association matrix from current neuron connections
    void updateAssociationMatrix();

    // Normalize association matrix to prevent runaway growth
    void normalizeAssociationMatrix(size_t indexA, size_t indexB);

    // Find or create pattern for given neural pattern
    size_t findOrCreatePattern(const std::vector<float>& pattern);

    // Find index of pattern matching given neuron pattern
    size_t findPatternIndex(const std::vector<NeuronId>& pattern) const;

    // Find most similar pattern to query pattern
    size_t findMostSimilarPattern(const std::vector<float>& pattern) const;

    // Convert neuron IDs to neural pattern representation
    void convertNeuronIdsToNeuralPattern(const std::vector<NeuronId>& neuronIds,
                                         std::vector<float>& neuralPattern);

    // Calculate similarity between two neural patterns
    static float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                           const std::vector<float>& pattern2);

    // Calculate association strength between patterns
    static float calculatePatternAssociation(const std::vector<float>& pattern1,
                                            const std::vector<float>& pattern2);

    // Update individual neuron association
    void updateNeuronAssociation(NeuronId neuronA, NeuronId neuronB, float delta);

    // Set Hebbian learning rate
    void setHebbianLearningRate(float rate);

    // Set decay rate
    void setDecayRate(float rate);

    // Set developmental modulation factor
    void setDevelopmentalModulation(float modulation);

    // Get Hebbian learning rate
    float getHebbianLearningRate() const;

    // Get decay rate
    float getDecayRate() const;

    // Get developmental modulation factor
    float getDevelopmentalModulation() const;

    // Activate pattern based on neuron pattern
    void activatePattern(const std::vector<NeuronId>& pattern);

    // Get indices of patterns with significant activation
    std::vector<size_t> getActivePatternIndices() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
};

} // namespace nlm
