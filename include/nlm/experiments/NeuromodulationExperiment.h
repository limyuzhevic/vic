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
#include <cmath>

namespace nlm {

/**
 * Neuromodulation Experiment
 * 
 * Advanced experiment that tests neuromodulation and learning signals
 * with dopamine, serotonin, norepinephrine, and other neuromodulators.
 * Demonstrates how neuromodulatory systems affect learning, memory,
 * and decision-making in neural networks.
 */
class NeuromodulationExperiment : public AdvancedExperiment {
public:
    // Neuromodulation configuration
    struct NeuromodulationConfig {
        // Neuromodulator parameters
        float dopamineLevel;
        float serotoninLevel;
        float norepinephrineLevel;
        float acetylcholineLevel;
        
        // Learning signal parameters
        float rewardPredictionError;
        float noveltySignal;
        float curiositySignal;
        float uncertainty;
        
        // Neuromodulation effects
        float dopamineLearningRate;
        float serotoninPlasticityModulator;
        float norepinephrineAttentionModulator;
        float acetylcholineMemoryModulator;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackNeuromodulation;
        bool trackLearningSignals;
        bool trackMemoryEffects;
        
        NeuromodulationConfig()
            : dopamineLevel(0.5f), serotoninLevel(0.3f), norepinephrineLevel(0.4f),
              acetylcholineLevel(0.6f), rewardPredictionError(0.1f), noveltySignal(0.2f),
              curiositySignal(0.3f), uncertainty(0.1f), dopamineLearningRate(0.01f),
              serotoninPlasticityModulator(1.0f), norepinephrineAttentionModulator(1.5f),
              acetylcholineMemoryModulator(0.8f), numTrials(100), timeLimit(300.0f),
              trackNeuromodulation(true), trackLearningSignals(true), 
              trackMemoryEffects(true) {}
    };
    
    // Neuromodulation event
    struct NeuromodulationEvent {
        std::chrono::steady_clock::time_point timestamp;
        std::string neuromodulator;
        float level;
        float effect;
        std::string trigger;
        
        NeuromodulationEvent() : level(0.0f), effect(0.0f) {}
    };
    
    // Learning signal measurement
    struct LearningSignal {
        uint64_t step;
        float rewardPredictionError;
        float novelty;
        float curiosity;
        float uncertainty;
        float learningRate;
        
        LearningSignal() : step(0), rewardPredictionError(0.0f), novelty(0.0f),
                          curiosity(0.0f), uncertainty(0.0f), learningRate(0.0f) {}
    };
    
    NeuromodulationExperiment();
    ~NeuromodulationExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setNeuromodulationConfig(const NeuromodulationConfig& config);
    const NeuromodulationConfig& getNeuromodulationConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Neuromodulation specific methods
    void runNeuromodulationTrial(size_t trialNumber);
    void updateNeuromodulators(const LearningSignal& signal);
    void applyNeuromodulationEffects();
    void recordNeuromodulationEvent(const std::string& neuromodulator, float level, 
                                   const std::string& trigger);
    
    // Results management
    const std::vector<NeuromodulationEvent>& getNeuromodulationEvents() const;
    const std::vector<LearningSignal>& getLearningSignals() const;
    double calculateNeuromodulationEffect(const std::string& neuromodulator) const;
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
    
    // Neuromodulation specific getters
    float getAverageDopamineLevel() const;
    float getAverageSerotoninLevel() const;
    float getAverageNorepinephrineLevel() const;
    float getAverageAcetylcholineLevel() const;
    
private:
    // Configuration
    NeuromodulationConfig config;
    
    // State tracking
    std::vector<NeuromodulationEvent> neuromodulationEvents;
    std::vector<LearningSignal> learningSignals;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Neuromodulation state
    float currentDopamineLevel;
    float currentSerotoninLevel;
    float currentNorepinephrineLevel;
    float currentAcetylcholineLevel;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> rewardDist;
    std::uniform_real_distribution<double> noveltyDist;
    std::uniform_real_distribution<double> neuromodulatorDist;
};

} // namespace nlm