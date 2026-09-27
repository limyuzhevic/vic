#pragma once

#include "PredictiveCoding.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <iomanip>

namespace nlm {

// PredictiveCodingExperiment: Demonstrates predictive coding and forward/backward models
// Shows how the brain generates predictions about sensory input and minimizes
// prediction error through forward and backward model learning

class PredictiveCodingExperiment {
public:
    PredictiveCodingExperiment(std::shared_ptr<Config> config, size_t duration = 1000);
    ~PredictiveCodingExperiment();
    
    // Run the predictive coding experiment
    void run();
    
    // Generate random state patterns for prediction
    std::vector<float> generateStatePattern(size_t size);
    
    // Set state in predictive coding system
    void setState(const std::vector<float>& state);
    
    // Get current predictions
    std::vector<float> getCurrentPredictions() const;
    
    // Get experiment statistics
    struct Statistics {
        size_t stepsCompleted;
        size_t samplesProcessed;
        float averagePredictionError;
        float bestPredictionError;
        float learningProgress;
        std::vector<float> errorHistory;
        std::vector<std::vector<float>> predictionHistory;
    };
    
    Statistics getStatistics() const { return stats; }
    
    // Get experiment results summary
    void printResults() const;
    
private:
    std::shared_ptr<Config> config;
    size_t duration;
    std::shared_ptr<Brain> brain;
    PredictiveCoding predictiveCoding;
    Statistics stats;
    
    // Pattern generation
    RandomGenerator randomGen;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    
    // State history
    std::vector<std::vector<float>> stateHistory;
    
    void initialize();
    void runStep(size_t step);
    void updateStatistics();
    void generateNewState();
};

} // namespace nlm