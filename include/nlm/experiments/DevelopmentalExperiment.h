#pragma once

#include "AdvancedExperiment.h"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <unordered_map>
#include <cmath>

namespace nlm {

/**
 * Developmental Experiment
 * 
 * Advanced experiment that tests developmental stages and maturation
 * with different life stages, growth patterns, and developmental trajectories.
 * Demonstrates how neural networks develop through stages of maturation
 * and adaptation throughout their "lifetime".
 */
class DevelopmentalExperiment : public AdvancedExperiment {
public:
    // Developmental configuration
    struct DevelopmentalConfig {
        // Life stage parameters
        float infancyRate;
        float childhoodRate;
        float adolescenceRate;
        float adulthoodRate;
        float senescenceRate;
        
        // Developmental processes
        float synaptogenesisRate;
        float pruningRate;
        float myelinationRate;
        float neurogenesisRate;
        
        // Learning parameters
        float developmentalLearningRate;
        float stageSpecificLearning;
        float criticalPeriodStrength;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackDevelopmentalStages;
        bool trackMaturationProgress;
        bool trackLearningVelocity;
        
        DevelopmentalConfig()
            : infancyRate(0.3f), childhoodRate(0.25f), adolescenceRate(0.2f),
              adulthoodRate(0.15f), senescenceRate(0.1f), synaptogenesisRate(0.8f),
              pruningRate(0.4f), myelinationRate(0.6f), neurogenesisRate(0.05f),
              developmentalLearningRate(0.01f), stageSpecificLearning(0.9f),
              criticalPeriodStrength(0.7f), numTrials(50), timeLimit(300.0f),
              trackDevelopmentalStages(true), trackMaturationProgress(true),
              trackLearningVelocity(true) {}
    };
    
    // Developmental stage
    struct DevelopmentalStage {
        std::string stageName;
        uint64_t stageStart;
        uint64_t stageEnd;
        float developmentalProgress;
        float learningVelocity;
        float neuralMaturation;
        float synapticDensity;
        float connectivityStrength;
        
        DevelopmentalStage() : stageStart(0), stageEnd(0), developmentalProgress(0.0f),
                              learningVelocity(0.0f), neuralMaturation(0.0f),
                              synapticDensity(0.0f), connectivityStrength(0.0f) {}
    };
    
    // Learning trajectory
    struct LearningTrajectory {
        uint64_t step;
        float performanceScore;
        float adaptationSpeed;
        float memoryRetention;
        float predictionAccuracy;
        float explorationRate;
        
        LearningTrajectory() : step(0), performanceScore(0.0f), adaptationSpeed(0.0f),
                             memoryRetention(0.0f), predictionAccuracy(0.0f),
                             explorationRate(0.0f) {}
    };
    
    DevelopmentalExperiment();
    ~DevelopmentalExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setDevelopmentalConfig(const DevelopmentalConfig& config);
    const DevelopmentalConfig& getDevelopmentalConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Developmental specific methods
    void runDevelopmentalTrial(size_t trialNumber);
    void updateLifeStage(size_t trialNumber);
    void simulateDevelopmentProcesses(size_t trialNumber);
    void trackMaturationProgress(size_t trialNumber);
    void applyCriticalPeriodLearning(size_t trialNumber);
    
    // Results management
    const std::vector<DevelopmentalStage>& getDevelopmentalStages() const;
    const std::vector<LearningTrajectory>& getLearningTrajectories() const;
    double calculateDevelopmentalMaturation() const;
    double calculateLearningTrajectoryEfficiency() const;
    double calculateCriticalPeriodEffectiveness() const;
    
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
    
    // Developmental specific getters
    std::string getCurrentLifeStage() const;
    float getDevelopmentalProgress() const;
    float getLearningVelocity() const;
    
private:
    // Configuration
    DevelopmentalConfig config;
    
    // State tracking
    std::vector<DevelopmentalStage> developmentalStages;
    std::vector<LearningTrajectory> learningTrajectories;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Developmental state
    std::string currentLifeStage;
    uint64_t currentStep;
    float developmentalProgress;
    float learningVelocity;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> stageDist;
    std::uniform_real_distribution<double> performanceDist;
    std::uniform_real_distribution<double> trajectoryDist;
};

} // namespace nlm