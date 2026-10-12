#pragma once

#include "ExperimentBase.hpp"
#include "ExperimentResults.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>

namespace nlm {

// Phase 4 experiment results using new infrastructure
using Phase4Results = nlm::Phase4Results;

// Base class for Phase 4 experiments
class Phase4ExperimentBase : public ExperimentBase {
public:
    Phase4ExperimentBase(const std::string& name);
    ~Phase4ExperimentBase() = default;
    
    // Lifecycle management
    bool initialize() override;
    bool run() override;
    void cleanup() override;
    bool isRunning() const override;
    
protected:
    virtual Phase4Results runExperiment(Brain* brain, size_t numTrials) = 0;
};

// Phase 4 Experiment: Temporal Prediction Learning
// Tests if NLM can learn that state A predicts state B
class TemporalPredictionExperiment : public Phase4ExperimentBase {
public:
    TemporalPredictionExperiment();
    ~TemporalPredictionExperiment() = default;
    
    // Run the experiment
    Phase4Results run(Brain* brain, size_t numTrials = 100) override;
    
    // Get prediction error over time
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    
private:
    std::vector<float> errorHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Working Memory Test
// Tests if information can be maintained after stimulus disappears
class WorkingMemoryExperiment : public Phase4ExperimentBase {
public:
    WorkingMemoryExperiment();
    ~WorkingMemoryExperiment() = default;
    
    // Run delayed response test
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
    
    // Get retention over delay periods
    std::vector<float> getRetentionByDelay() const { return retentionByDelay_; }
    
private:
    std::vector<float> retentionByDelay_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Episodic Memory Recall
// Tests if past experiences influence future behavior
class EpisodicRecallExperiment : public Phase4ExperimentBase {
public:
    EpisodicRecallExperiment();
    ~EpisodicRecallExperiment() = default;
    
    // Run episodic memory test
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
    
    // Get recall accuracy over time
    std::vector<float> getRecallAccuracyHistory() const { return recallHistory_; }
    
private:
    std::vector<float> recallHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Concept Formation
// Tests if recurring patterns form stable representations
class ConceptFormationExperiment : public Phase4ExperimentBase {
public:
    ConceptFormationExperiment();
    ~ConceptFormationExperiment() = default;
    
    // Run concept formation test
    Phase4Results run(Brain* brain, size_t numTrials = 100) override;
    
    // Get concept stability over time
    std::vector<float> getConceptStabilityHistory() const { return stabilityHistory_; }
    
private:
    std::vector<float> stabilityHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Attention Selection
// Tests if competing signals produce selective processing
class AttentionExperiment : public Phase4ExperimentBase {
public:
    AttentionExperiment();
    ~AttentionExperiment() = default;
    
    // Run attention test
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
    
    // Get attention selectivity over time
    std::vector<float> getSelectivityHistory() const { return selectivityHistory_; }
    
private:
    std::vector<float> selectivityHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Multi-Step Planning
// Tests if NLM can plan multi-step sequences to achieve goals
class PlanningExperiment : public Phase4ExperimentBase {
public:
    PlanningExperiment();
    ~PlanningExperiment() = default;
    
    // Run planning test
    Phase4Results run(Brain* brain, size_t numTrials = 30) override;
    
    // Get planning success over time
    std::vector<float> getSuccessHistory() const { return successHistory_; }
    
private:
    std::vector<float> successHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Self-Action Prediction
// Tests if NLM learns that its actions cause predictable sensory changes
class SelfModelExperiment : public Phase4ExperimentBase {
public:
    SelfModelExperiment();
    ~SelfModelExperiment() = default;
    
    // Run self-model test
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
    
    // Get self-prediction accuracy over time
    std::vector<float> getSelfPredictionHistory() const { return selfPredHistory_; }
    
private:
    std::vector<float> selfPredHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Object Permanence
// Tests if hidden objects remain relevant to behavior
class ObjectPermanenceExperiment : public Phase4ExperimentBase {
public:
    ObjectPermanenceExperiment();
    ~ObjectPermanenceExperiment() = default;
    
    // Run object permanence test
    Phase4Results run(Brain* brain, size_t numTrials = 30) override;
    
private:
    std::vector<float> searchBehaviorHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Social Observation
// Tests if NLM can learn from observing another agent
class SocialLearningExperiment : public Phase4ExperimentBase {
public:
    SocialLearningExperiment();
    ~SocialLearningExperiment() = default;
    
    // Run social learning test
    Phase4Results run(Brain* brain, size_t numTrials = 30) override;
    
    // Get imitation accuracy over time
    std::vector<float> getImitationHistory() const { return imitationHistory_; }
    
private:
    std::vector<float> imitationHistory_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Continual Learning
// Tests if NLM can learn new tasks without forgetting old ones
class ContinualLearningExperiment : public Phase4ExperimentBase {
public:
    ContinualLearningExperiment();
    ~ContinualLearningExperiment() = default;
    
    // Run continual learning test
    Phase4Results run(Brain* brain, size_t numTrials = 100) override;
    
    // Get performance across tasks
    std::vector<float> getTaskPerformance() const { return taskPerformance_; }
    
private:
    std::vector<float> taskPerformance_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Experiment: Generalization
// Tests if learned knowledge transfers to new situations
class GeneralizationExperiment : public Phase4ExperimentBase {
public:
    GeneralizationExperiment();
    ~GeneralizationExperiment() = default;
    
    // Run generalization test
    Phase4Results run(Brain* brain, size_t numTrials = 50) override;
    
    // Get transfer performance
    float getTransferRatio() const { return transferRatio_; }
    
private:
    float transferRatio_;
    
    Phase4Results runExperiment(Brain* brain, size_t numTrials) override;
};

// Integrated Phase 4 Experiment
// Combines all Phase 4 mechanisms in a complex environment
class Phase4IntegratedExperiment {
public:
    Phase4IntegratedExperiment();
    ~Phase4IntegratedExperiment();
    
    // Run integrated experiment
    Phase4Results run(Brain* brain, size_t numEpisodes = 100, size_t stepsPerEpisode = 200);
    
    // Get metrics over time
    const std::vector<Phase4Results>& getEpisodeResults() const { return episodeResults_; }
    
    // Get before/after comparison
    Phase4Results compareBeforeAfter() const;
    
private:
    std::vector<Phase4Results> episodeResults_;
};

// Ablation Experiment
// Tests which mechanisms contribute to performance
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
    
    // Run ablation study
    std::vector<AblationResult> run(Brain* brain, size_t numTrials = 30);
    
private:
    std::vector<AblationResult> results_;
};

} // namespace nlm