#pragma once

#include "MetaLearning.hpp"
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

// MetaLearningExperiment: Demonstrates meta-learning and learning-to-learn mechanisms
// Shows how the brain learns how to learn more efficiently

class MetaLearningExperiment {
public:
    MetaLearningExperiment(std::shared_ptr<Config> config, size_t duration = 1000);
    ~MetaLearningExperiment();
    
    // Run the meta-learning experiment
    void run();
    
    // Generate random states for meta-learning
    std::vector<float> generateState(size_t size);
    
    // Calculate learning performance based on state and actions
    float calculateLearningPerformance(const std::vector<float>& state);
    
    // Get experiment statistics
    struct Statistics {
        size_t stepsCompleted;
        size_t samplesProcessed;
        float averageLearningPerformance;
        float bestLearningPerformance;
        float adaptationSpeed;
        float learningEfficiency;
        std::vector<float> performanceHistory;
    };
    
    Statistics getStatistics() const { return stats; }
    
    // Get experiment results summary
    void printResults() const;
    
private:
    std::shared_ptr<Config> config;
    size_t duration;
    std::shared_ptr<Brain> brain;
    MetaLearning metaLearning;
    Statistics stats;
    
    // Pattern generation
    RandomGenerator randomGen;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    
    void initialize();
    void runStep(size_t step);
    void updateStatistics();
    void generateNewState();
};

} // namespace nlm