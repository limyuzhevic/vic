#pragma once

#include "../agent/AgentBody.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../experiments/ExperimentPresets.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Phase 4 Experiment Results
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

/**
 * Advanced Phase 4 experiment configuration
 * Extends basic Phase 4 with multi-objective optimization and parameter sweeps
 */
struct AdvancedPhase4Config {
    // Experiment scale
    size_t numTrials;
    size_t stepsPerTrial;
    uint64_t seed;
    
    // Performance optimization
    bool enableMultiObjectiveOptimization;
    std::vector<std::string> optimizationObjectives;
    float convergenceThreshold;
    size_t maxIterations;
    
    // Parameter sweeps
    bool enableParameterSweeps;
    std::unordered_map<std::string, std::vector<float>> parameterRanges;
    std::vector<std::string> sweepParameters;
    bool parallelSweeps;
    
    // Adaptive optimization
    bool enableAdaptiveOptimization;
    float adaptationRate;
    float explorationRate;
    
    // Comparative analysis
    bool enableComparativeAnalysis;
    std::vector<std::string> comparativePresets;
    bool runAllCombinations;
    
    // Hyperparameter optimization
    bool enableHyperparameterOptimization;
    std::string optimizationAlgorithm;
    std::unordered_map<std::string, float> optimizationParameters;
    
    // Resource management
    bool enableMultithreading;
    size_t threadCount;
    bool enableSIMD;
    bool enableMemoryOptimization;
    
    // Output and logging
    std::string outputDirectory;
    bool generateVisualizations;
    bool saveIntermediateResults;
    size_t checkpointInterval;
    
    // Experimental controls
    bool enableAblations;
    std::vector<std::string> ablationTargets;
    bool enableCrossValidation;
    size_t cvFolds;
    
    // Constructors
    AdvancedPhase4Config();
    explicit AdvancedPhase4Config(const std::string& presetName);
    
    // Preset management
    static AdvancedPhase4Config getPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Research preset factories
    static AdvancedPhase4Config getResearchPredictionPreset();
    static AdvancedPhase4Config getResearchMemoryPreset();
    static AdvancedPhase4Config getResearchAttentionPreset();
    
    // Performance preset factories
    static AdvancedPhase4Config getPerformanceSmallPreset();
    static AdvancedPhase4Config getPerformanceMediumPreset();
    static AdvancedPhase4Config getPerformanceLargePreset();
    
    // Research question preset factories
    static AdvancedPhase4Config getTemporalPredictionPreset();
    static AdvancedPhase4Config getWorkingMemoryPreset();
    static AdvancedPhase4Config getEpisodicRecallPreset();
    
    // Validation and optimization
    bool validate() const;
    std::string getValidationErrors() const;
    std::vector<std::string> getOptimizationRecommendations() const;
};

// Experiment: Temporal Prediction Learning
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

// Experiment: Working Memory Test
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

// Experiment: Episodic Memory Recall
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

// Experiment: Concept Formation
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

// Experiment: Attention Selection
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

// Experiment: Multi-Step Planning
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

// Experiment: Self-Action Prediction
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

// Experiment: Object Permanence
// Tests if hidden objects remain relevant to behavior
class ObjectPermanenceExperiment {
public:
    ObjectPermanenceExperiment();
    ~ObjectPermanenceExperiment();
    
    // Run object permanence test
    Phase4Results run(Brain* brain, size_t numTrials = 30);
    
private:
    std::vector<float> searchBehaviorHistory_;
};

// Experiment: Social Observation
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

// Experiment: Continual Learning
// Tests if NLM can learn new tasks without forgetting old ones
class ContinualLearningExperiment {
public:
    ContinualLearningExperiment();
    ~ContinualLearningExperiment();
    
    // Run continual learning test
    Phase4Results run(Brain* brain, size_t numTrials = 100);
    
    // Get performance across tasks
    std::vector<float> getTaskPerformance() const { return taskPerformance_; }
    
private:
    std::vector<float> taskPerformance_;
};

// Experiment: Generalization
// Tests if learned knowledge transfers to new situations
class GeneralizationExperiment {
public:
    GeneralizationExperiment();
    ~GeneralizationExperiment();
    
    // Run generalization test
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    // Get transfer performance
    float getTransferRatio() const { return transferRatio_; }
    
private:
    float transferRatio_;
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

/**
 * Advanced Phase 4 experiment with multi-objective optimization
 * Extends the basic Phase 4 experiment with:
 * - Multi-objective optimization capabilities
 * - Parameter sweep automation
 * - Adaptive optimization algorithms
 * - Comparative experiment framework
 * - Hyperparameter optimization integration
 * - Parallel execution support
 */
class AdvancedPhase4Experiment {
public:
    AdvancedPhase4Experiment(const AdvancedPhase4Config& config);
    explicit AdvancedPhase4Experiment(const std::string& presetName);
    ~AdvancedPhase4Experiment();
    
    // Core experiment execution
    void run();
    bool isRunning() const { return running_; }
    
    // Configuration management
    const AdvancedPhase4Config& getConfig() const { return config_; }
    void updateConfig(const AdvancedPhase4Config& newConfig);
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    static AdvancedPhase4Config getResearchPreset(const std::string& researchArea);
    static AdvancedPhase4Config getPerformancePreset(const std::string& scale);
    static AdvancedPhase4Config getResearchQuestionPreset(const std::string& question);
    
    // Advanced features
    void runParameterSweep();
    void runMultiObjectiveOptimization();
    void enableAdaptiveOptimization(float rate = 0.01f, float exploration = 0.1f);
    void runComparativeAnalysis(const std::vector<std::string>& presetNames);
    void enableHyperparameterOptimization(const std::string& algorithm = "CMA-ES");
    
    // Experiment control
    void setOptimizationAlgorithm(const std::string& algorithm);
    void setOptimizationParameters(const std::unordered_map<std::string, float>& params);
    void enableMultithreading(size_t threads = std::thread::hardware_concurrency());
    
    // Results collection and analysis
    const std::vector<Phase4Results>& getResults() const { return results_; }
    const std::unordered_map<std::string, std::vector<float>>& getSweepResults() const { return sweepResults_; }
    float getOptimizationScore(const std::string& objective) const;
    std::string getParetoFrontDescription() const;
    std::string getOptimizationReport() const;
    
    // Progress and status
    float getProgress() const;
    std::string getStatusReport() const;
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    static std::shared_ptr<AdvancedPhase4Experiment> importResults(const std::string& filepath);
    
    // Statistical analysis
    void computeStatistics();
    float computeConfidenceInterval(float confidence = 0.95f) const;
    bool checkConvergence() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    AdvancedPhase4Config config_;
    bool running_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::vector<Phase4Results> results_;
    std::unordered_map<std::string, std::vector<float>> sweepResults_;
    std::vector<std::vector<float>> paretoFront_;
    std::unordered_map<std::string, float> currentObjectives_;
    
    // Helper methods
    void initializeExperiment();
    void executeSingleExperiment(size_t trialIndex);
    void executeParameterSweep();
    void executeMultiObjectiveOptimization();
    void updateAdaptiveParameters();
    float evaluateObjective(const std::string& objective, const Phase4Results& results) const;
    void updateParetoFront(const std::vector<float>& objectiveValues);
    void generateComparativeReport() const;
    void generateOptimizationReport() const;
    float calculateWeightedScore(const std::vector<float>& objectives) const;
};

} // namespace nlm