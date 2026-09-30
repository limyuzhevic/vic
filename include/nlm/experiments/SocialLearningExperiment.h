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
 * Social Learning Experiment
 * 
 * Advanced experiment that tests social learning and interaction with other agents,
 * including imitation, observation learning, and social transmission of behaviors.
 * Demonstrates how neural networks learn from observing and interacting with others.
 */
class SocialLearningExperiment : public AdvancedExperiment {
public:
    // Social learning configuration
    struct SocialConfig {
        // Social interaction parameters
        float imitationAccuracy;
        float observationLearningRate;
        float socialTransmissionStrength;
        float cooperativeBehavior;
        float competitiveBehavior;
        
        // Group dynamics
        size_t groupSize;
        float conformityPressure;
        float socialInfluence;
        float peerPressure;
        
        // Learning parameters
        float socialLearningRate;
        float culturalTransmissionRate;
        float normLearningRate;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackImitationLearning;
        bool trackCulturalTransmission;
        bool trackSocialInfluence;
        
        SocialConfig()
            : imitationAccuracy(0.7f), observationLearningRate(0.05f),
              socialTransmissionStrength(0.8f), cooperativeBehavior(0.6f),
              competitiveBehavior(0.4f), groupSize(5), conformityPressure(0.5f),
              socialInfluence(0.7f), peerPressure(0.6f), socialLearningRate(0.03f),
              culturalTransmissionRate(0.02f), normLearningRate(0.01f),
              numTrials(50), timeLimit(300.0f), trackImitationLearning(true),
              trackCulturalTransmission(true), trackSocialInfluence(true) {}
    };
    
    // Social interaction
    struct SocialInteraction {
        uint64_t step;
        std::string interactionType; // "imitation", "observation", "cooperation", "competition"
        float interactionStrength;
        float learningOutcome;
        std::string agentBehavior;
        std::string peerBehavior;
        
        SocialInteraction() : step(0), interactionStrength(0.0f), learningOutcome(0.0f) {}
    };
    
    // Cultural transmission
    struct CulturalTransmission {
        std::string behaviorPattern;
        float transmissionStrength;
        float adoptionRate;
        float persistenceDuration;
        uint64_t lastObserved;
        
        CulturalTransmission() : transmissionStrength(0.0f), adoptionRate(0.0f),
                               persistenceDuration(0), lastObserved(0) {}
    };
    
    SocialLearningExperiment();
    ~SocialLearningExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setSocialConfig(const SocialConfig& config);
    const SocialConfig& getSocialConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Social learning specific methods
    void runSocialLearningTrial(size_t trialNumber);
    void simulateSocialInteraction(size_t trialNumber);
    void processImitationLearning(size_t trialNumber);
    void processObservationLearning(size_t trialNumber);
    void processCooperativeLearning(size_t trialNumber);
    void processCompetitiveLearning(size_t trialNumber);
    void updateCulturalTransmission(size_t trialNumber);
    
    // Results management
    const std::vector<SocialInteraction>& getSocialInteractions() const;
    const std::vector<CulturalTransmission>& getCulturalTransmissions() const;
    double calculateImitationAccuracy() const;
    double calculateSocialLearningEfficiency() const;
    double calculateCulturalTransmissionStrength() const;
    
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
    
    // Social learning specific getters
    float getSocialCooperationLevel() const;
    float getSocialConformityPressure() const;
    size_t getGroupSize() const;
    
private:
    // Configuration
    SocialConfig config;
    
    // State tracking
    std::vector<SocialInteraction> socialInteractions;
    std::vector<CulturalTransmission> culturalTransmissions;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Social state
    size_t currentGroupSize;
    float socialCooperationLevel;
    float socialConformityPressure;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> socialDist;
    std::uniform_real_distribution<double> interactionDist;
    std::uniform_real_distribution<double> adoptionDist;
};

} // namespace nlm