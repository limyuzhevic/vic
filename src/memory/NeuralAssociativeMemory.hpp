#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// NeuralAssociativeMemory: Forms connections between memory patterns
// Creates Hebbian associations between episodic memory traces
// 
// Key mechanisms:
// - Pattern completion (recall by partial cue)
// - Pattern separation (distinct patterns don't interfere)
// - Competitive learning for winner-take-all associations
// - Temporal sequence learning (Chrono-associations)

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two memory patterns
    // Used for linking episodic memories and concepts
    void associate(NeuronId patternA, NeuronId patternB, float strength);
    
    // Get associated neurons for a given pattern
    // Implements pattern completion - recall by partial cue
    std::vector<NeuronId> getAssociations(NeuronId pattern) const;
    
    // Get association strength between patterns
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association with new experience
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all associations
    void clear();
    
    // Get number of active associations
    size_t getActiveAssociationCount() const;
    
    // Get strongest associations for a pattern
    std::vector<std::pair<NeuronId, float>> getStrongestAssociations(
        NeuronId pattern, size_t count = 5) const;
    
    // Clean up old/inactive associations
    void pruneWeakAssociations(float threshold);
    
    // Form associations from working memory patterns
    void formAssociationsFromWorkingMemory(
        const std::vector<NeuronId>& neurons,
        const std::vector<float>& activations);
    
    // Rehearse associations to strengthen them
    void rehearseAssociations(const std::vector<NeuronId>& patterns);
    
private:
    // Core association storage
    struct Association {
        NeuronId patternA;
        NeuronId patternB;
        float strength;
        SimulationStep lastUpdated;
        float eligibilityTrace;  // For reward-modulated learning
        
        Association(NeuronId a, NeuronId b, float s)
            : patternA(a), patternB(b), strength(s), 
              lastUpdated(0), eligibilityTrace(0.0f) {}
    };
    
    // Forward and backward indexing for efficient lookup
    std::unordered_map<NeuronId, std::vector<Association>> forwardMap;
    std::unordered_map<NeuronId, std::vector<Association>> backwardMap;
    
    // Learning parameters
    float learningRate_;
    float consolidationRate_;
    float pruningThreshold_;
    size_t maxAssociationsPerPattern_;
    
    // Integration state
    Brain* brain_;
    SimulationStep currentStep_;
};

} // namespace nlm
