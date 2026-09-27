#pragma once

#include "PolicyGradient.hpp"
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

// PolicyGradientExperiment: Demonstrates policy gradient learning
// Shows how the brain learns optimal policies through rewards
// and exploration strategies

class PolicyGradientExperiment {
public:
    PolicyGradientExperiment(std::shared_ptr<Config> config, size_t duration = 1000);
    ~PolicyGradientExperiment();
    
    // Run the policy gradient experiment
    void run();
    
    // Generate random states for policy learning
    std::vector<float> generateState(size_t size);
    
    // Get current action distribution
    std::vector<float> getActionProbabilities(const std::vector<float>& state) const;
    
    // Get experiment statistics
    struct Statistics {
        size_t stepsCompleted;
        size_t actionsSelected;
        float averageReward;
        float bestReward;
        float policyGradientMagnitude;
        float explorationRate;
        std::vector<float> rewardHistory;
        std::vector<size_t> actionHistory;
    };
    
    Statistics getStatistics() const { return stats; }
    
    // Get experiment results summary
    void printResults() const;
    
private:
    std::shared_ptr<Config> config;
    size_t duration;
    std::shared_ptr<Brain> brain;
    PolicyGradient policyGradient;
    Statistics stats;
    
    // Pattern generation
    RandomGenerator randomGen;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    
    // Experience history
    std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>> experienceHistory;
    
    void initialize();
    void runStep(size_t step);
    void updateStatistics();
    void generateNewState();
};

} // namespace nlm