#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralAssociativeMemory: Stores and retrieves relationships between neural patterns
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall
// - Pattern storage and recall capabilities

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Store pattern for recall
    // Creates or updates pattern neuron for later recall
    void storePattern(const std::vector<float>& pattern, float strength = 1.0f);

    // Recall pattern using query
    // Returns the most similar stored pattern(s)
    std::vector<float> recallPattern(const std::vector<float>& query) const;

    // Create association based on experience
    void associateFromExperience(const EpisodicMemoryItem& episode);

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

    // Clear all patterns and associations
    void clear();

    // Clear memory of patterns below threshold strength
    void clearWeakPatterns(float threshold);

    // Decay all patterns over time
    void decayPatterns(float decayRate);

    // Strengthen patterns based on use
    void strengthenUsedPatterns(float factor);

    // Get number of patterns
    size_t getPatternCount() const { return patternNeurons_.size(); }

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

    // Get max patterns capacity
    size_t getMaxPatterns() const { return maxPatterns_; }

    // Set max patterns capacity
    void setMaxPatterns(size_t max) { maxPatterns_ = max; }

    // Get current number of active traces
    size_t getActiveTraces() const { return activeTraces_; }

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Capacity management
    size_t maxPatterns_;
    float decayRate_;
    
    // Statistics
    size_t activeTraces_;
    float totalAssociationStrength_;
};

} // namespace nlm