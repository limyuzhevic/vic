#pragma once

#include "brain/Brain.hpp"
#include <vector>
#include <string>

namespace nlm {

// Learning Experiment: Demonstrates measurable synaptic changes through experience
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    // Record initial state before learning
    void recordInitialState();
    
    // Record final state after learning
    void recordFinalState();
    
    // Compute and display learning statistics
    void computeStatistics();
    
    // Get learning results
    float getInitialMeanWeight() const { return initialMeanWeight; }
    float getFinalMeanWeight() const { return finalMeanWeight; }
    float getWeightChange() const { return finalMeanWeight - initialMeanWeight; }
    size_t getSynapticChanges() const { return totalSynapticChanges; }
    bool getLearningDetected() const { return learningOccurred; }
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    // Computed statistics
    float initialMeanWeight = 0.0f;
    float finalMeanWeight = 0.0f;
    size_t totalSynapticChanges = 0;
    bool learningOccurred = false;
    
    // Helper methods
    void computeMeanWeights();
    void countSynapticChanges();
};

} // namespace nlm
