#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class Brain;

class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    ~LearningExperiment() = default;
    
    // Record initial state of the brain
    void recordInitialState();
    
    // Record final state after learning
    void recordFinalState();
    
    // Compute and display learning statistics
    void computeStatistics();
    
    // Get initial weights for analysis
    const std::vector<float>& getInitialWeights() const { return initialWeights_; }
    
    // Get final weights for analysis
    const std::vector<float>& getFinalWeights() const { return finalWeights_; }
    
    // Get most active neurons during learning
    const std::vector<uint64_t>& getMostActiveNeurons() const { return mostActiveNeurons_; }
    
    // Get total spike count for the experiment
    uint64_t getTotalSpikeCount() const;
    
    // Check if learning occurred (weights changed significantly)
    bool didLearningOccur() const;
    
    // Get experiment description
    std::string getDescription() const;
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    size_t initialSynapseCount_ = 0;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<uint64_t> mostActiveNeurons_;
};

} // namespace nlm