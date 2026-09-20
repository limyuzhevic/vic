// Brain system experiment integration
// Moved from Phase4Experiment.cpp - TemporalPredictionExperiment, WorkingMemoryExperiment, etc.

#pragma once

#include "../brain/NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Temporal Prediction Learning Experiment
// Tests if NLM can learn that state A predicts state B
class TemporalPredictionExperiment {
public:
    TemporalPredictionExperiment();
    ~TemporalPredictionExperiment();
    
    // Run the experiment
    Phase4Results run(Brain* brain, size_t numTrials = 100);
    
    // Get prediction error over time
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    
private:
    std::vector<float> errorHistory_;
};

// Working Memory Test
// Tests if information can be maintained after stimulus disappears
class WorkingMemoryExperiment {
public:
    WorkingMemoryExperiment();
    ~WorkingMemoryExperiment();
    
    // Run delayed response test
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    // Get retention over delay periods
    std::vector<float> getRetentionByDelay() const { return retentionByDelay_; }
    
private:
    std::vector<float> retentionByDelay_;
};

// Episodic Memory Recall Test
// Tests if past experiences influence future behavior
class EpisodicRecallExperiment {
public:
    EpisodicRecallExperiment();
    ~EpisodicRecallExperiment();
    
    // Run episodic memory test
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    // Get recall accuracy over time
    std::vector<float> getRecallAccuracyHistory() const { return recallHistory_; }
    
private:
    std::vector<float> recallHistory_;
};

// Concept Formation Test
// Tests if recurring patterns form stable representations
class ConceptFormationExperiment {
public:
    ConceptFormationExperiment();
    ~ConceptFormationExperiment();
    
    // Run concept formation test
    Phase4Results run(Brain* brain, size_t numTrials = 100);
    
    // Get concept stability over time
    std::vector<float> getConceptStabilityHistory() const { return stabilityHistory_; }
    
private:
    std::vector<float> stabilityHistory_;
};

// Attention Selection Test
// Tests if competing signals produce selective processing
class AttentionExperiment {
public:
    AttentionExperiment();
    ~AttentionExperiment();
    
    // Run attention test
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    // Get attention selectivity over time
    std::vector<float> getSelectivityHistory() const { return selectivityHistory_; }
    
private:
    std::vector<float> selectivityHistory_;
};

// Multi-Step Planning Test
// Tests if NLM can plan multi-step sequences to achieve goals
class PlanningExperiment {
public:
    PlanningExperiment();
    ~PlanningExperiment();
    
    // Run planning test
    Phase4Results run(Brain* brain, size_t numTrials = 30);
    
    // Get planning success over time
    std::vector<float> getSuccessHistory() const { return successHistory_; }
    
private:
    std::vector<float> successHistory_;
};

// Self-Action Prediction Test
// Tests if NLM learns that its actions cause predictable sensory changes
class SelfModelExperiment {
public:
    SelfModelExperiment();
    ~SelfModelExperiment();
    
    // Run self-model test
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    // Get self-prediction accuracy over time
    std::vector<float> getSelfPredictionHistory() const { return selfPredHistory_; }
    
private:
    std::vector<float> selfPredHistory_;
};

// Social Observation Test
// Tests if NLM can learn from observing another agent
class SocialLearningExperiment {
public:
    SocialLearningExperiment();
    ~SocialLearningExperiment();
    
    // Run social learning test
    Phase4Results run(Brain* brain, size_t numTrials = 30);
    
    // Get imitation accuracy over time
    std::vector<float> getImitationHistory() const { return imitationHistory_; }
    
private:
    std::vector<float> imitationHistory_;
};

// Representation Analysis Tools
class RepresentationAnalyzer {
public:
    RepresentationAnalyzer();
    ~RepresentationAnalyzer();
    
    // Compute neural pattern similarity
    static float computePatternSimilarity(const std::vector<float>& a,
                                         const std::vector<float>& b);
    
    // Measure representational stability
    static float measureRepresentationalStability(
        const std::vector<std::vector<float>>& patterns);
    
    // Measure clustering coefficient
    static float measureClustering(
        const std::vector<std::vector<float>>& patterns,
        size_t numClusters);
    
    // Measure selectivity of neural responses
    static float measureSelectivity(const std::vector<std::vector<float>>& responses);
    
    // Compute population vector for a pattern
    static std::vector<float> computePopulationVector(
        const std::vector<std::pair<NeuronId, float>>& neuronActivities);
    
    // Measure temporal precision of neural patterns
    static float measureTemporalPrecision(
        const std::vector<std::vector<float>>& temporalPatterns);
};

} // namespace nlm
