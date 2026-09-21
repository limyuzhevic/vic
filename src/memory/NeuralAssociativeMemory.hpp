#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// NeuralAssociativeMemory: Creates associations between neural patterns
// NOT a simple lookup table - associations are formed through neural activity
//
// Key mechanisms:
// - Hebbian learning strengthens connections between co-active neurons
// - Pattern completion through spreading activation
// - Memory consolidation through synaptic weight updates
// - Cross-talk between working and episodic memory

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create an association between two patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength);

    // Create association from episodic memory item
    void associateFromExperience(const EpisodicMemoryItem& episode);

    // Retrieve patterns associated with query
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults) const;

    // Update association with new experience
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float newStrength);

    // Spread activation from one pattern to related patterns
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& sourcePattern,
        float activationRadius) const;

    // Get association strength between patterns
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;

    // Clear all associations
    void clear();

    // Get all pattern neurons
    const std::vector<std::pair<NeuronId, std::vector<float>>>&
        getPatternNeurons() const { return patternNeurons_; }

    // Get all associations
    const std::vector<std::tuple<NeuronId, NeuronId, float>>&
        getAssociations() const { return associations_; }

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

    // Find neuron representing pattern
    NeuronId findPatternNeuron(const std::vector<float>& pattern) const;

    // Compute similarity between patterns
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;

private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    
    // Pattern storage: map neurons to pattern vectors
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    
    // Associations: (neuronA, neuronB, strength)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm
