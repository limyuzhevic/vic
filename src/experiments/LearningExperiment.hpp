#pragma once

/**
 * LearningExperiment - A utility class for conducting neural learning experiments
 * 
 * This class provides functionality to:
 * - Record initial and final brain states
 * - Calculate synaptic weight changes
 * - Analyze spike activity patterns
 * - Determine if learning occurred through experience
 * 
 * Designed for use in Phase 2 and later neural computation experiments.
 */

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <numeric>
#include <cmath>

namespace nlm {

class LearningExperiment {
public:
    // Constructor
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    // Core experiment functionality
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    
    // Configuration
    void enableDetailedLogging(bool enable) { detailedLogging_ = enable; }
    void setLearningThreshold(float threshold) { learningThreshold_ = threshold; }
    
    // Results access
    float getInitialMeanWeight() const { return initialMeanWeight_; }
    float getFinalMeanWeight() const { return finalMeanWeight_; }
    float getWeightChange() const { return weightChange_; }
    bool getLearningDetected() const { return learningDetected_; }
    size_t getStrengthenedSynapses() const { return strengthenedSynapses_; }
    size_t getWeakenedSynapses() const { return weakenedSynapses_; }
    
    // Convenience methods
    std::string getSummaryReport() const;
    bool learningOccurred() const { return learningDetected_; }
    
private:
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Experimental parameters
    uint64_t seed_;
    bool detailedLogging_;
    float learningThreshold_;
    
    // State recording
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    float initialMeanWeight_;
    
    std::vector<float> finalWeights_;
    float finalMeanWeight_;
    
    std::vector<NeuronId> mostActiveNeurons_;
    
    // Analysis results
    float weightChange_;
    size_t strengthenedSynapses_;
    size_t weakenedSynapses_;
    size_t unchangedSynapses_;
    bool learningDetected_;
    
    // Helper methods
    float calculateMean(const std::vector<float>& weights) const;
    void logWeightDistribution(const std::string& label, const std::vector<float>& weights) const;
    bool isSignificantChange(float delta) const;
};

} // namespace nlm
