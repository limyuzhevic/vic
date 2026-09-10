#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <unordered_map>
#include <utility>

namespace nlm {

// NeuralAssociativeMemory: Stores pattern associations for quick retrieval
// NOT a semantic memory - stores associations between patterns, not meaning
//
// Key mechanisms:
// - Distributed storage of associations
// - Heteromodal binding (different patterns in different regions)
// - Similarity-based retrieval
// - Pattern completion through spreading activation
// - Fast weight updates for learning associations

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create an association between two patterns
    // Patterns can be sensory states, actions, or internal states
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength);
    
    // Learn association from an episodic memory item
    // Extracts sensory-to-sensory and action-to-sensory associations
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    // Retrieve patterns similar to a query
    // Returns patterns ordered by similarity strength
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;
    
    // Get the strength of an association between two patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;
    
    // Update an existing association
    void updateAssociation(const std::vector<float>& patternA,
                           const std::vector<float>& patternB,
                           float delta);
    
    // Activate patterns starting from a cue
    // Implements spreading activation through the association network
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 3) const;
    
    // Store a pattern neuron with its activity pattern
    void storePattern(const std::vector<float>& pattern, NeuronId neuronId);
    
    // Compute similarity between two patterns using correlation
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
    
    // Find neuron ID for a pattern (nearest match)
    NeuronId findPatternNeuron(const std::vector<float>& pattern) const;
    
    // Clear all associations and stored patterns
    void clear();
    
    // Get all stored pattern neurons
    const std::vector<std::pair<NeuronId, std::vector<float>>>& getPatternNeurons() const;
    
    // Get all associations
    const std::vector<std::tuple<NeuronId, NeuronId, float>>& getAssociations() const;
    
    // Get number of pattern neurons stored
    size_t getPatternCount() const;
    
    // Get number of associations stored
    size_t getAssociationCount() const;
    
    // Is the association network enabled?
    bool isEnabled() const { return enabled_; }
    
    // Enable or disable the association network
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    // Get association network capacity
    size_t getCapacity() const { return capacity_; }
    
    // Set association network capacity
    void setCapacity(size_t capacity) { capacity_ = capacity; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    bool enabled_;
    size_t capacity_;
    
    // Pattern storage
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    
    // Association storage (neuron pairs with strength)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm