#pragma once

#include "../agent/AgentBody.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/NeuralSelfModel.hpp"
#include "../cognition/NeuralSocialLearning.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

namespace nlm {

// Phase 4 Experiment Results - consolidated structure
struct Phase4Results {
    // Prediction metrics
    float predictionErrorInitial;
    float predictionErrorFinal;
    float predictionAccuracyImprovement;
    
    // Working memory metrics
    float workingMemoryRetentionInitial;
    float workingMemoryRetentionFinal;
    float workingMemoryCapacity;
    
    // Episodic memory metrics
    float episodicRecallAccuracy;
    size_t episodesStored;
    float episodicInfluence;
    
    // Associative memory metrics
    float associationStrength;
    float associationTransfer;
    
    // Concept formation metrics
    size_t conceptsFormed;
    float conceptStability;
    float generalizationAbility;
    
    // Attention metrics
    float attentionSelectivity;
    float distractionResistance;
    
    // Planning metrics
    float planningAccuracy;
    float multiStepSuccess;
    float planningConfidence;
    
    // Self-model metrics
    float selfPredictionAccuracy;
    float bodyAwareness;
    float selfOtherDistinction;
    
    // Social learning metrics
    float imitationAccuracy;
    float socialPredictionAccuracy;
    size_t observationsFromOthers;
    
    // Overall metrics
    float totalReward;
    float behaviorImprovement;
    float transferPerformance;
    
    Phase4Results();
    void reset();
    std::string toCSV() const;
};

// Base class for all Phase 4 experiments
class BasePhase4Experiment {
protected:
    std::vector<float> errorHistory_;
    std::vector<float> retentionByDelay_;
    std::vector<float> recallHistory_;
    std::vector<float> stabilityHistory_;
    std::vector<float> selectivityHistory_;
    std::vector<float> successHistory_;
    std::vector<float> selfPredHistory_;
    std::vector<float> imitationHistory_;
    std::vector<float> taskPerformance_;
    float transferRatio_;
    std::vector<std::vector<float>> searchBehaviorHistory_;
    std::vector<Phase4Results> episodeResults_;

public:
    virtual ~BasePhase4Experiment() = default;
    
    virtual Phase4Results run(Brain* brain, size_t numTrials = 100) {
        Phase4Results results;
        return results;
    }
    
    // Common utility methods for all experiments
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
};

// Individual experiment classes - simplified and consolidated
class TemporalPredictionExperiment : public BasePhase4Experiment {
public:
    TemporalPredictionExperiment();
    ~TemporalPredictionExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 100) override;
};

class WorkingMemoryExperiment : public BasePhase4Experiment {
public:
    WorkingMemoryExperiment();
    ~WorkingMemoryExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
};

class EpisodicRecallExperiment : public BasePhase4Experiment {
public:
    EpisodicRecallExperiment();
    ~EpisodicRecallExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
};

class ConceptFormationExperiment : public BasePhase4Experiment {
public:
    ConceptFormationExperiment();
    ~ConceptFormationExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 100) override;
};

class AttentionExperiment : public BasePhase4Experiment {
public:
    AttentionExperiment();
    ~AttentionExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
};

class PlanningExperiment : public BasePhase4Experiment {
public:
    PlanningExperiment();
    ~PlanningExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 30) override;
};

class SelfModelExperiment : public BasePhase4Experiment {
public:
    SelfModelExperiment();
    ~SelfModelExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
};

class ObjectPermanenceExperiment : public BasePhase4Experiment {
public:
    ObjectPermanenceExperiment();
    ~ObjectPermanenceExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 30) override;
};

class SocialLearningExperiment : public BasePhase4Experiment {
public:
    SocialLearningExperiment();
    ~SocialLearningExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 30) override;
};

class ContinualLearningExperiment : public BasePhase4Experiment {
public:
    ContinualLearningExperiment();
    ~ContinualLearningExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 100) override;
};

class GeneralizationExperiment : public BasePhase4Experiment {
public:
    GeneralizationExperiment();
    ~GeneralizationExperiment();
    
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
};

class Phase4IntegratedExperiment {
public:
    Phase4IntegratedExperiment();
    ~Phase4IntegratedExperiment();
    
    Phase4Results run(Brain* brain, size_t numEpisodes = 100, size_t stepsPerEpisode = 200);
    Phase4Results compareBeforeAfter() const;
    
private:
    std::vector<Phase4Results> episodeResults_;
};

class AblationExperiment {
public:
    AblationExperiment();
    ~AblationExperiment();
    
    struct AblationResult {
        std::string mechanism;
        Phase4Results withMechanism;
        Phase4Results withoutMechanism;
        float contribution;
    };
    
    std::vector<AblationResult> run(Brain* brain, size_t numTrials = 30);
    
private:
    std::vector<AblationResult> results_;
};

} // namespace nlm