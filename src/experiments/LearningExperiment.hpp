// LearningExperiment - Demonstrates measurable synaptic changes through experience
// 
// This experiment records brain state before and after neural activity,
// then computes statistics about synaptic plasticity and learning.

#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <memory>
#include <numeric>

namespace nlm {

// Learning experiment result summary
struct LearningExperimentResult {
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    size_t strengthenedSynapses;
    size_t weakenedSynapses;
    size_t unchangedSynapses;
    size_t totalSpikes;
    bool learningOccurred;
    
    LearningExperimentResult()
        : initialMeanWeight(0.0f), finalMeanWeight(0.0f), weightChange(0.0f)
        , strengthenedSynapses(0), weakenedSynapses(0), unchangedSynapses(0)
        , totalSpikes(0), learningOccurred(false) {}
};

// Learning experiment that tracks synaptic changes
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    ~LearningExperiment();
    
    // Record initial brain state
    void recordInitialState();
    
    // Record final brain state  
    void recordFinalState();
    
    // Compute and log experiment statistics
    void computeStatistics();
    
    // Get experiment results
    const LearningExperimentResult& getResults() const { return results; }
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    // Recorded state data
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    // Computed results
    LearningExperimentResult results;
    
    // Helper methods
    void computeWeightStatistics();
    void computeSynapticChanges();
    void computeSpikeActivity();
    std::vector<float> recordWeightsFromRegion(RegionId regionId) const;
};

} // namespace nlm
