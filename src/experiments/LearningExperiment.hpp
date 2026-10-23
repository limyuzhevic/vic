// NLM Learning Experiment
// Phase 2: Real Neural Computation
// 
// This class demonstrates measurable synaptic changes through experience
// by tracking neural activity and synaptic weight changes over time.

#pragma once

#include "brain/Brain.hpp"
#include <vector>
#include <string>
#include <numeric>
#include "core/Logger/Logger.hpp"

namespace nlm {

class LearningExperiment {
public:
    // Constructor initializes experiment with brain reference and seed
    explicit LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    // Record initial brain state before learning
    void recordInitialState();
    
    // Record final brain state after learning
    void recordFinalState();
    
    // Compute and display learning statistics
    void computeStatistics() const;
    
    // Get learning metrics for programmatic access
    double getInitialMeanWeight() const { return initialMeanWeight; }
    double getFinalMeanWeight() const { return finalMeanWeight; }
    double getWeightChange() const { return finalMeanWeight - initialMeanWeight; }
    size_t getStrengthenedSynapses() const { return strengthenedCount; }
    size_t getWeakenedSynapses() const { return weakenedCount; }
    size_t getUnchangedSynapses() const { return unchangedCount; }
    bool wasLearningDetected() const { return learningOccurred; }
    
private:
    std::shared_ptr<Brain> brain;           // Reference to the brain being tested
    uint64_t seed;                          // Random seed for reproducibility
    size_t initialSynapseCount;             // Initial number of synapses
    std::vector<float> initialWeights;      // Initial synaptic weights
    std::vector<float> finalWeights;        // Final synaptic weights
    std::vector<NeuronId> mostActiveNeurons; // Most active neurons during experiment
    
    // Computed statistics
    double initialMeanWeight;               // Mean initial synaptic weight
    double finalMeanWeight;                 // Mean final synaptic weight
    size_t strengthenedCount;               // Synapses that increased weight significantly
    size_t weakenedCount;                   // Synapses that decreased weight significantly
    size_t unchangedCount;                  // Synapses that remained unchanged
    bool learningOccurred;                  // Whether significant learning was detected
};

} // namespace nlm