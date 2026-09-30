#pragma once

#include "AdvancedExperiment.h"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <unordered_map>

namespace nlm {

/**
 * Adaptive Learning Experiment
 * 
 * Advanced experiment that tests adaptive learning capabilities with
 * dynamic parameter adjustment and environmental complexity changes.
 * Demonstrates how neural networks can adapt to changing environments
 * and learn optimal strategies through experience.
 */
class AdaptiveLearningExperiment : public AdvancedExperiment {
public:
    // Adaptive learning configuration
    struct AdaptiveConfig {
        // Learning parameters
        float learningRate;
        float noveltyThreshold;
        float energyThreshold;
        float curiosityThreshold;
        
        // Environmental dynamics
        float environmentComplexity;
        float adaptationSpeed;
        float rewardVariance;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        size_t explorationProbability;
        
        // Performance tracking
        bool trackPerformance;
        bool trackAdaptation;
        bool trackExploration;
        
        AdaptiveConfig()
            : learningRate(0.01f), noveltyThreshold(0.5f), energyThreshold(0.1f),
              curiosityThreshold(0.3f), environmentComplexity(5.0f), 
              adaptationSpeed(0.1f), rewardVariance(0.1f), numTrials(100),
              timeLimit(300.0f), explorationProbability(20), trackPerformance(true),
              trackAdaptation(true), trackExploration(true) {}
    };
    
    // Adaptive trial results
    struct AdaptiveTrialResult {
        size_t trialNumber;
        double reward;
        double novelty;
        double curiosity;
        double energy;
        double predictionError;
        std::string action;
        float adaptationFactor;
        double environmentalComplexity;
        
        AdaptiveTrialResult() : trialNumber(0), reward(0.0), novelty(0.0), 
                               curiosity(0.0), energy(0.0), predictionError(0.0),
                               adaptationFactor(1.0f), environmentalComplexity(0.0f) {}
    };
    
    AdaptiveLearningExperiment();
    ~AdaptiveLearningExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setAdaptiveConfig(const AdaptiveConfig& config);
    const AdaptiveConfig& getAdaptiveConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Adaptive learning specific methods
    void runAdaptiveTrial(size_t trialNumber);
    void adaptParameters(size_t trialNumber);
    void updateEnvironmentalComplexity(size_t trialNumber);
    
    // Results management
    const std::vector<AdaptiveTrialResult>& getTrialResults() const;
    double calculateAdaptationRate() const;
    double calculateExplorationRate() const;
    double calculateLearningEfficiency() const;
    
    // Brain and environment setup
    std::shared_ptr<Brain> createBrain() override;
    std::shared_ptr<Environment> createEnvironment() override;
    
    // Experiment execution
    bool runExperiment(std::shared_ptr<Brain> brain, 
                       std::shared_ptr<Environment> environment) override;
    
    // Results saving/loading
    bool saveResults(const std::string& filepath) override;
    bool loadResults(const std::string& filepath) override;
    
    // Progress reporting
    double getProgress() const override;
    std::string getStatus() const override;
    
    // Adaptive learning specific getters
    double getAverageReward() const;
    double getAverageNovelty() const;
    double getAverageCuriosity() const;
    size_t getSuccessfulAdaptations() const;
    
private:
    // Configuration
    AdaptiveConfig config;
    
    // State tracking
    std::vector<AdaptiveTrialResult> trialResults;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Adaptive learning state
    float currentLearningRate;
    float currentEnvironmentalComplexity;
    size_t successfulAdaptations;
    size_t totalAdaptations;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> rewardDist;
    std::uniform_real_distribution<double> noveltyDist;
    std::uniform_real_distribution<double> actionDist;
};

} // namespace nlm