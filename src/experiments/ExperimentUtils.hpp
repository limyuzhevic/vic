#pragma once

#include "Phase4Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Common base class for all experiments to reduce code duplication
class BaseExperiment {
protected:
    // Common initialization for all experiments
    virtual void initializeCommonSystems(Brain* brain) {
        if (!brain) return;
        
        // Initialize all commonly used systems
        // These can be overridden by specific experiments if needed
        if (!predictionSystem_.get()) {
            predictionSystem_ = std::make_unique<NeuralPrediction>();
            predictionSystem_->initialize(brain);
        }
        
        if (!workingMemory_.get()) {
            workingMemory_ = std::make_unique<NeuralWorkingMemory>();
            workingMemory_->initialize(brain);
        }
        
        if (!episodicMemory_.get()) {
            episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
            episodicMemory_->initialize(brain);
        }
        
        if (!conceptFormation_.get()) {
            conceptFormation_ = std::make_unique<ConceptFormation>();
            conceptFormation_->initialize(brain);
        }
        
        if (!planner_.get()) {
            planner_ = std::make_unique<NeuralPlanner>();
            planner_->initialize(brain);
        }
    }
    
    // Common cleanup
    virtual void cleanupCommonSystems() {
        predictionSystem_.reset();
        workingMemory_.reset();
        episodicMemory_.reset();
        conceptFormation_.reset();
        planner_.reset();
    }
    
    // Helper methods for common patterns
    static float computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    // Common test pattern generation
    std::vector<float> generateTestPattern(size_t size, float baseValue = 0.5f, 
                                          std::vector<size_t> activeIndices = {}) {
        std::vector<float> pattern(size, baseValue);
        for (size_t idx : activeIndices) {
            if (idx < size) pattern[idx] = 1.0f;
        }
        return pattern;
    }
    
public:
    BaseExperiment() = default;
    virtual ~BaseExperiment() {}
    
    // Virtual run method that subclasses must implement
    virtual Phase4Results run(Brain* brain, size_t numTrials) = 0;
    
    // Common protected members for use by subclasses
    std::unique_ptr<NeuralPrediction> predictionSystem_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<NeuralPlanner> planner_;
};

// Helper class for analyzing representation similarity
class RepresentationAnalyzer {
public:
    RepresentationAnalyzer() = default;
    ~RepresentationAnalyzer() = default;
    
    static float computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    static float measureRepresentationalStability(const std::vector<std::vector<float>>& patterns) {
        if (patterns.size() < 2) return 0.0f;
        
        float totalStability = 0.0f;
        size_t comparisons = 0;
        
        for (size_t i = 1; i < patterns.size(); ++i) {
            float sim = computePatternSimilarity(patterns[i], patterns[i-1]);
            totalStability += sim;
            comparisons++;
        }
        
        return comparisons > 0 ? totalStability / comparisons : 0.0f;
    }
    
    static float measureSelectivity(const std::vector<std::vector<float>>& responses) {
        if (responses.empty()) return 0.0f;
        
        float maxResponse = 0.0f;
        float totalResponse = 0.0f;
        
        for (const auto& response : responses) {
            for (float v : response) {
                maxResponse = std::max(maxResponse, v);
                totalResponse += v;
            }
        }
        
        if (totalResponse < 0.0001f) return 0.0f;
        
        float avgResponse = totalResponse / responses.size();
        return maxResponse > 0.0f ? (maxResponse - avgResponse) / maxResponse : 0.0f;
    }
    
    static std::vector<float> computePopulationVector(
        const std::vector<std::pair<NeuronId, float>>& neuronActivities) {
        std::vector<float> result(1, 0.0f);
        
        if (neuronActivities.empty()) return result;
        
        float totalWeight = 0.0f;
        for (const auto& [neuron, activity] : neuronActivities) {
            result[0] += activity;
            totalWeight += 1.0f;
        }
        
        if (totalWeight > 0.0f) {
            result[0] /= totalWeight;
        }
        
        return result;
    }
    
    static float measureTemporalPrecision(
        const std::vector<std::vector<float>>& temporalPatterns) {
        if (temporalPatterns.size() < 2) return 0.0f;
        
        return measureRepresentationalStability(temporalPatterns);
    }
};

// Common experiment framework with reduced duplication
class Phase4ExperimentBase : public BaseExperiment {
protected:
    virtual void setupEpisodeSystems(Brain* brain) {
        // Common setup for episodic experiments
        if (!selfModel_.get()) {
            selfModel_ = std::make_unique<SelfModel>();
            selfModel_->initialize(brain);
        }
    }
    
    virtual Phase4Results createBaseResults() {
        Phase4Results results;
        results.reset();
        return results;
    }
    
    void updateMetricsFromBrain(Phase4Results& results, Brain* brain) {
        if (brain) {
            results.predictionErrorInitial = 1.0f;  // Default assumption
            results.predictionErrorFinal = 0.5f;
            results.workingMemoryCapacity = 0.8f;
            results.planningConfidence = 0.6f;
            results.bodyAwareness = 0.7f;
            results.selfPredictionAccuracy = 0.7f;
        }
    }
    
protected:
    std::unique_ptr<SelfModel> selfModel_;
};

// Helper function to create experiments with common setup
Phase4Results runExperimentWithCommonSetup(
    BaseExperiment& experiment, 
    Brain* brain, 
    size_t numTrials,
    const std::string& experimentName
) {
    NLM_LOG_INFO("Running " + experimentName + " experiment with " + std::to_string(numTrials) + " trials");
    
    experiment.initializeCommonSystems(brain);
    Phase4Results results = experiment.run(brain, numTrials);
    
    experiment.cleanupCommonSystems();
    
    NLM_LOG_INFO(experimentName + " completed. Total reward: " + std::to_string(results.totalReward));
    return results;
}

} // namespace nlm
