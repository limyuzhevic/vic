#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <set>
#include <algorithm>
#include <tuple>

namespace nlm {

class Brain;
class EpisodicMemoryItem;

// NeuralAssociativeMemory: Creates and manages associations between neural patterns
// Implements Hebbian-style association learning and pattern completion
// Allows inference through spread activation across connected patterns

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength);
    
    // Create association from an episodic memory item
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    // Retrieve associated patterns for a query
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 10) const;
    
    // Get strength of association between two patterns
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Spread activation through association network
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 5) const;
    
    // Clear all associations
    void clear();
    
    // Find neuron ID for a pattern (helper)
    NeuronId findPatternNeuron(const std::vector<float>& pattern) const;
    
    // Compute similarity between two patterns
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
    // Get total number of patterns
    size_t getPatternCount() const { return patternNeurons_.size(); }
    
    // Get total number of associations
    size_t getAssociationCount() const { return associations_.size(); }
    
    // Get all patterns
    const std::vector<std::pair<NeuronId, std::vector<float>>>& getAllPatterns() const {
        return patternNeurons_;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Pattern neurons (pattern -> neuron ID mapping)
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    
    // Associations (neuronA, neuronB, strength)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm