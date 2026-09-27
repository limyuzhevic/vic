#pragma once

#include "ReconstructionError.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

namespace nlm {

// ReconstructionExperiment: Demonstrates reconstruction error learning
// Shows how the brain learns to reconstruct target patterns through
// auto-encoder style learning and reconstruction error minimization

class ReconstructionExperiment {
public:
    ReconstructionExperiment(std::shared_ptr<Config> config, size_t duration = 1000);
    ~ReconstructionExperiment();
    
    // Run the reconstruction experiment
    void run();
    
    // Generate random target patterns for reconstruction
    std::vector<float> generateTargetPattern(size_t size);
    
    // Inject pattern into brain as sensory input
    void injectPattern(const std::vector<float>& pattern);
    
    // Get current brain state as reconstruction
    std::vector<float> getCurrentReconstruction() const;
    
    // Get experiment statistics
    struct Statistics {
        size_t stepsCompleted;
        size_t targetsGenerated;
        float averageError;
        float bestError;
        float learningProgress;
        std::vector<float> errorHistory;
        std::vector<float> reconstructionHistory;
    };
    
    Statistics getStatistics() const { return stats; }
    
    // Get experiment results summary
    void printResults() const;
    
private:
    std::shared_ptr<Config> config;
    size_t duration;
    std::shared_ptr<Brain> brain;
    ReconstructionError reconstructionError;
    Statistics stats;
    
    // Pattern generation
    RandomGenerator randomGen;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    
    // Pattern history
    std::vector<std::vector<float>> targetHistory;
    
    void initialize();
    void runStep(size_t step);
    void updateStatistics();
    void generateNewTarget();
};

} // namespace nlm