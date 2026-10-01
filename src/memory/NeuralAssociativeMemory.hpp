#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralAssociativeMemory: Creates and retrieves relationships between experiences
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(class Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association based on experience
    void associateFromExperience(const struct EpisodicMemoryItem& episode);

    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;

    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;

    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);

    // Spread activation from cue pattern
    // Returns patterns that become activated through association chains
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;

    // Clear associations
    void clear();

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

    // Get number of stored patterns
    size_t getPatternCount() const { return patternNeurons_.size(); }

    // Set learning parameters
    void setLearningRate(float rate) { pImpl->learningRate = rate; }
    float getLearningRate() const { return pImpl->learningRate; }

    void setSimilarityThreshold(float threshold) { pImpl->similarityThreshold = threshold; }
    float getSimilarityThreshold() const { return pImpl->similarityThreshold; }

    void setMaxAssociationsPerPattern(size_t max) { pImpl->maxAssociationsPerPattern = max; }
    size_t getMaxAssociationsPerPattern() const { return pImpl->maxAssociationsPerPattern; }

private:
    // Find or create neuron representation for a pattern
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute similarity between two patterns (cosine similarity)
    static float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b);

    // Extract neural pattern from episodic memory
    static std::vector<float> extractNeuralPattern(const EpisodicMemoryItem& episode);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Current storage
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;

    Brain* brain_;
};

} // namespace nlm