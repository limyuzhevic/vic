// LearningExperiment.hpp
// Header for the refactored LearningExperiment class
// Provides clean interface with improved separation of concerns
// Maintains backward compatibility with existing usage

#pragma once

#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * @class LearningExperiment
 * @brief Conducts neural learning experiments and analyzes synaptic changes
 * 
 * This class demonstrates measurable synaptic changes through experience by:
 * - Recording initial and final synaptic weights
 * - Computing learning statistics
 * - Detecting if learning occurred through weight changes
 * 
 * The class maintains backward compatibility with the original struct interface
 * while providing better separation of concerns, error handling, and documentation.
 */
class LearningExperiment {
private:
    // Data storage with private access
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
    
    // Helper function to record weights from a specific brain region
    void recordWeightsFromRegion(std::vector<float>& weights, RegionId regionId) const;
    
    // Helper function to compute mean of weights vector
    float computeMean(const std::vector<float>& weights) const;
    
    // Helper function to compare two weights vectors and count changes
    void countWeightChanges(const std::vector<float>& initial, 
                          const std::vector<float>& final,
                          size_t& strengthened,
                          size_t& weakened,
                          size_t& unchanged) const;
    
    // Helper function to validate that weight data exists
    bool validateWeightData() const;
    
    // Helper function to log experimental results
    void logExperimentResults(float initialMean, float finalMean,
                              size_t strengthened, size_t weakened, size_t unchanged) const;
    
    // Constants
    static constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;
    static constexpr float LEARNING_THRESHOLD = 0.001f;
    static constexpr size_t ACTIVE_NEURONS_COUNT = 10;

public:
    // Constructor - maintains backward compatibility
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    // Destructor
    ~LearningExperiment();
    
    // Original public interface - maintains backward compatibility
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    
    // Getters for data members (maintains access where needed)
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    uint64_t getSeed() const { return seed_; }
    size_t getInitialSynapseCount() const { return initialSynapseCount_; }
    const std::vector<float>& getInitialWeights() const { return initialWeights_; }
    const std::vector<float>& getFinalWeights() const { return finalWeights_; }
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons_; }
    
    // Enhanced methods with better error handling and validation
    bool isValid() const;
    float getInitialMeanWeight() const;
    float getFinalMeanWeight() const;
    float getWeightChange() const;
    bool learningOccurred() const;
};

} // namespace nlm