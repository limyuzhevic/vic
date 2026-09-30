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
#include <algorithm>

namespace nlm {

/**
 * Cross-Modal Experiment
 * 
 * Advanced experiment that tests cross-modal learning and sensory integration,
 * combining visual, auditory, tactile, and other sensory inputs to create
 * a unified representation of the environment.
 * Demonstrates how different sensory modalities can be integrated
 * to improve learning and perception.
 */
class CrossModalExperiment : public AdvancedExperiment {
public:
    // Cross-modal configuration
    struct CrossModalConfig {
        // Sensory modality parameters
        float visualSensoryWeight;
        float auditorySensoryWeight;
        float tactileSensoryWeight;
        float olfactorySensoryWeight;
        float proprioceptiveSensoryWeight;
        
        // Integration parameters
        float crossModalIntegrationStrength;
        float sensoryConflictResolution;
        float multimodalBindingStrength;
        
        // Learning parameters
        float crossModalLearningRate;
        float sensoryEncodingCapacity;
        float multimodalRepresentationSize;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackSensoryIntegration;
        bool trackCrossModalLearning;
        bool trackMultisensoryRepresentation;
        
        CrossModalConfig()
            : visualSensoryWeight(0.3f), auditorySensoryWeight(0.25f),
              tactileSensoryWeight(0.2f), olfactorySensoryWeight(0.1f),
              proprioceptiveSensoryWeight(0.15f), crossModalIntegrationStrength(1.0f),
              sensoryConflictResolution(0.8f), multimodalBindingStrength(0.9f),
              crossModalLearningRate(0.01f), sensoryEncodingCapacity(100.0f),
              multimodalRepresentationSize(50), numTrials(50), timeLimit(300.0f),
              trackSensoryIntegration(true), trackCrossModalLearning(true),
              trackMultisensoryRepresentation(true) {}
    };
    
    // Sensory input
    struct SensoryInput {
        std::string modality;
        std::vector<float> features;
        float intensity;
        float reliability;
        float timestamp;
        
        SensoryInput() : intensity(0.0f), reliability(0.0f), timestamp(0.0f) {}
    };
    
    // Multisensory representation
    struct MultisensoryRepresentation {
        std::vector<float> integratedFeatures;
        std::vector<std::string> contributingModalities;
        float integrationConfidence;
        float bindingStrength;
        
        MultisensoryRepresentation() : integrationConfidence(0.0f), bindingStrength(0.0f) {}
    };
    
    CrossModalExperiment();
    ~CrossModalExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setCrossModalConfig(const CrossModalConfig& config);
    const CrossModalConfig& getCrossModalConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Cross-modal specific methods
    void runCrossModalTrial(size_t trialNumber);
    void collectSensoryInput(size_t trialNumber);
    MultisensoryRepresentation integrateSensoryInputs(const std::vector<SensoryInput>& inputs);
    void updateCrossModalLearning(const MultisensoryRepresentation& representation);
    void resolveSensoryConflicts(const std::vector<SensoryInput>& inputs);
    
    // Results management
    const std::vector<SensoryInput>& getSensoryInputs() const;
    const std::vector<MultisensoryRepresentation>& getMultisensoryRepresentations() const;
    double calculateSensoryIntegrationAccuracy() const;
    double calculateCrossModalLearningEfficiency() const;
    double calculateMultisensoryBindingStrength() const;
    
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
    
    // Cross-modal specific getters
    float getSensoryIntegrationStrength() const;
    float getMultisensoryBindingQuality() const;
    
private:
    // Configuration
    CrossModalConfig config;
    
    // State tracking
    std::vector<SensoryInput> sensoryInputs;
    std::vector<MultisensoryRepresentation> multisensoryRepresentations;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Cross-modal state
    float sensoryIntegrationStrength;
    float multisensoryBindingQuality;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> sensoryDist;
    std::uniform_real_distribution<double> integrationDist;
    std::uniform_real_distribution<double> featureDist;
};

} // namespace nlm