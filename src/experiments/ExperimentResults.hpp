#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <limits>

namespace nlm {

// Generic experiment results template
struct ExperimentResults {
    uint64_t timestamp;
    std::string experimentName;
    uint64_t seed;
    std::string description;
    
    // Standard metric categories
    struct {
        std::vector<double> performance;
        std::vector<double> accuracy;
        std::vector<double> error;
        std::vector<double> efficiency;
        std::vector<double> stability;
    } metrics;
    
    // Additional metadata fields
    std::vector<std::pair<std::string, std::string>> config;
    std::vector<std::string> notes;
    std::vector<std::pair<std::string, double>> stepData;
    
    ExperimentResults() : timestamp(0), seed(0) {}
    
    // Type-safe metric access
    template<typename T>
    void setMetric(const std::string& name, const T& value);
    
    template<typename T>
    T getMetric(const std::string& name) const;
    
    bool hasMetric(const std::string& name) const;
    
    // Serialization methods
    std::string toCSV() const;
    std::string toJSON() const;
    
    // Validation
    bool isValid() const;
    
    // Reset for reuse
    void reset();
};

// Specialized result types
struct Phase3Results : public ExperimentResults {
    // Phase 3 specific metrics
    float totalReward;
    int steps;
    float finalNovelty;
    float finalCuriosity;
    float predictionError;
    float developmentalAge;
    std::string developmentalStage;
    size_t totalSynapses;
    float averageWeight;
    float maxWeight;
    size_t totalSpikes;
    float averageFiringRate;
    int forwardCount;
    int backwardCount;
    int turnLeftCount;
    int turnRightCount;
    int interactCount;
    int waitCount;
    
    Phase3Results() : totalReward(0.0f), steps(0), finalNovelty(0.0f), 
                      finalCuriosity(0.0f), predictionError(0.0f),
                      developmentalAge(0.0f), totalSynapses(0), 
                      averageWeight(0.0f), maxWeight(0.0f),
                      totalSpikes(0), averageFiringRate(0.0f) {}
    
    std::string toCSV() const override;
    std::string toJSON() const override;
    void reset() override;
};

struct Phase4Results : public ExperimentResults {
    // Phase 4 specific metrics from original implementation
    float predictionErrorInitial;
    float predictionErrorFinal;
    float predictionAccuracyImprovement;
    float workingMemoryRetentionInitial;
    float workingMemoryRetentionFinal;
    float workingMemoryCapacity;
    float episodicRecallAccuracy;
    size_t episodesStored;
    float episodicInfluence;
    float associationStrength;
    float associationTransfer;
    size_t conceptsFormed;
    float conceptStability;
    float generalizationAbility;
    float attentionSelectivity;
    float distractionResistance;
    float planningAccuracy;
    float multiStepSuccess;
    float planningConfidence;
    float selfPredictionAccuracy;
    float bodyAwareness;
    float selfOtherDistinction;
    float imitationAccuracy;
    float socialPredictionAccuracy;
    size_t observationsFromOthers;
    float totalReward;
    float behaviorImprovement;
    float transferPerformance;
    
    Phase4Results() : predictionErrorInitial(0.0f), predictionErrorFinal(0.0f),
                      predictionAccuracyImprovement(0.0f), workingMemoryRetentionInitial(0.0f),
                      workingMemoryRetentionFinal(0.0f), workingMemoryCapacity(0.0f),
                      episodicRecallAccuracy(0.0f), episodesStored(0), episodicInfluence(0.0f),
                      associationStrength(0.0f), associationTransfer(0.0f), conceptsFormed(0),
                      conceptStability(0.0f), generalizationAbility(0.0f), attentionSelectivity(0.0f),
                      distractionResistance(0.0f), planningAccuracy(0.0f), multiStepSuccess(0.0f),
                      planningConfidence(0.0f), selfPredictionAccuracy(0.0f), bodyAwareness(0.0f),
                      selfOtherDistinction(0.0f), imitationAccuracy(0.0f), socialPredictionAccuracy(0.0f),
                      observationsFromOthers(0), totalReward(0.0f), behaviorImprovement(0.0f),
                      transferPerformance(0.0f) {}
    
    std::string toCSV() const override;
    std::string toJSON() const override;
    void reset() override;
};

} // namespace nlm
