#pragma once

/**
 * Phase5IntegratedExperiment - Comprehensive lifetime learning experiment
 * 
 * This experiment demonstrates the full capabilities of Phase 5:
 * - Large-scale neural simulation
 * - Event-driven processing
 * - Multithreading and SIMD
 * - Lifelong learning
 * - Memory consolidation
 * - Continual adaptation
 * - Emergent specialization
 * 
 * Experiment phases:
 * 
 * Phase 1: Early Development (0-10k steps)
 *   - Initial synaptogenesis
 *   - Sensory exploration
 *   - Basic reflexes
 * 
 * Phase 2: Sensory Development (10k-50k steps)
 *   - Vision/contact object detection
 *   - Simple object interaction
 *   - Energy management
 * 
 * Phase 3: Motor Development (50k-100k steps)
 *   - Movement coordination
 *   - Spatial navigation
 *   - Object manipulation
 * 
 * Phase 4: Associative Development (100k-200k steps)
 *   - Action-consequence learning
 *   - Reward prediction
 *   - Curiosity-driven exploration
 * 
 * Phase 5: Memory Consolidation (200k-300k steps)
 *   - Episodic memory formation
 *   - Spatial representation
 *   - Concept formation
 * 
 * Phase 6: Social Development (300k-400k steps)
 *   - Other agent observation
 *   - Imitation learning
 *   - Simple communication
 * 
 * Phase 7: Continual Learning (400k+ steps)
 *   - New environments
 *   - Task switching
 *   - Lifelong adaptation
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include "../experiments/AblationSystem.hpp"
#include "../experiments/ScalingBenchmark.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Lifetime experiment configuration
 */
struct LifetimeExperimentConfig {
    // Scale settings
    size_t initialNeurons;
    size_t maxNeurons;
    float connectionProbability;
    
    // Development settings
    uint64_t synapseFormationSteps;
    uint64_t sensoryDevelopmentSteps;
    uint64_t motorDevelopmentSteps;
    uint64_t associativeDevelopmentSteps;
    uint64_t memoryConsolidationSteps;
    uint64_t socialDevelopmentSteps;
    
    // Experiment settings
    size_t trialsPerPhase;
    uint64_t stepsPerTrial;
    
    // Performance settings
    bool enableMultithreading;
    bool enableSIMD;
    bool enableSparseConnectivity;
    bool enableEventDriven;
    size_t threadCount;
    
    // Output settings
    std::string outputDir;
    bool saveCheckpoints;
    bool generateVisualizations;
    bool runAblations;
    
    LifetimeExperimentConfig();
};

/**
 * Results from a lifetime experiment phase
 */
struct LifetimePhaseResult {
    std::string phaseName;
    uint64_t startStep;
    uint64_t endStep;
    double duration;
    
    // Performance metrics
    BenchmarkMetrics performance;
    
    // Learning metrics
    float totalReward;
    float avgRewardPerStep;
    float rewardVariance;
    
    // Neural metrics
    float avgFiringRate;
    float synapticWeightMean;
    float excitatoryRatio;
    float connectionDensity;
    
    // Behavioral metrics
    float explorationRate;
    float goalDirectedness;
    float adaptationSpeed;
    
    // Memory metrics
    float memoryRetention;
    float episodicRecall;
    
    // Prediction metrics
    float predictionAccuracy;
    float predictionError;
    
    // Development metrics
    std::unordered_map<std::string, float> developmentalProgress;
};

/**
 * Complete lifetime experiment result
 */
struct LifetimeExperimentResult {
    LifetimeExperimentConfig config;
    
    std::vector<LifetimePhaseResult> phaseResults;
    
    // Overall metrics
    float totalReward;
    float avgRewardPerStep;
    float overallLearningEfficiency;
    
    // Final capabilities
    float finalPerformance;
    float memoryCapacity;
    float predictionAccuracy;
    float generalizationAbility;
    
    // Scaling data
    std::vector<std::pair<size_t, double>> scaleVsPerformance;
    
    // Comparison with baselines
    std::unordered_map<std::string, float> capabilityScores;
    
    // Failures and issues
    std::vector<std::string> failures;
    std::vector<std::string> warnings;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    LifetimeExperimentResult();
    
    std::string toJSON() const;
    std::string summary() const;
};

/**
 * Continual learning task definition
 */
struct ContinualTask {
    std::string name;
    std::string description;
    std::function<float(class Brain*)> evaluateFunction;
    uint64_t stepsBetweenTasks;
};

/**
 * Phase 5 integrated experiment runner
 */
class Phase5IntegratedExperiment {
public:
    Phase5IntegratedExperiment();
    ~Phase5IntegratedExperiment();
    
    /**
     * Run the complete lifetime experiment
     */
    LifetimeExperimentResult run(
        const LifetimeExperimentConfig& config,
        std::function<std::shared_ptr<class Brain>(const LifetimeExperimentConfig& config)> createBrain,
        std::function<bool(class Brain* brain, uint64_t steps)> runSimulation
    );
    
    /**
     * Run a single phase
     */
    LifetimePhaseResult runPhase(
        class Brain* brain,
        const std::string& phaseName,
        uint64_t startStep,
        uint64_t endStep,
        const std::function<void(uint64_t step)>& progressCallback
    );
    
    /**
     * Run continual learning experiment with task switching
     */
    LifetimeExperimentResult runContinualLearning(
        std::shared_ptr<class Brain> brain,
        const std::vector<ContinualTask>& tasks,
        size_t cycles = 3
    );
    
    /**
     * Run damage and recovery experiment
     */
    LifetimeExperimentResult runDamageRecovery(
        std::shared_ptr<class Brain> brain,
        uint64_t damageStep,
        float damageFraction,
        const std::string& damageRegion
    );
    
    /**
     * Run scaling experiment
     */
    ScalingAnalysis runScalingExperiment(
        const std::vector<ScaleLevel>& scales,
        uint64_t stepsPerScale
    );
    
    /**
     * Run ablation comparison
     */
    std::vector<AblationResult> runAblationComparison(
        const std::string& task,
        const AblationExperiment& experiment
    );

private:
    LifetimeExperimentResult initializeResult(const LifetimeExperimentConfig& config);
    
    void savePhaseCheckpoint(
        class Brain* brain,
        const std::string& phase,
        uint64_t step
    );
    
    LifetimePhaseResult collectPhaseResults(
        class Brain* brain,
        const std::string& phaseName,
        uint64_t startStep,
        uint64_t endStep
    );
    
    void generateVisualizations(
        const LifetimeExperimentResult& result,
        const std::string& outputDir
    );
    
    std::string generateReport(const LifetimeExperimentResult& result);
};

/**
 * Multi-seed experiment runner for statistical significance
 */
class MultiSeedExperimentRunner {
public:
    MultiSeedExperimentRunner();
    ~MultiSeedExperimentRunner();
    
    /**
     * Run experiment with multiple seeds
     */
    std::vector<LifetimeExperimentResult> runWithMultipleSeeds(
        size_t numSeeds,
        const LifetimeExperimentConfig& baseConfig,
        std::function<std::shared_ptr<class Brain>(const LifetimeExperimentConfig& config, uint64_t seed)> createBrain,
        std::function<bool(class Brain* brain, uint64_t steps)> runSimulation
    );
    
    /**
     * Compute statistics across seeds
     */
    struct SeedStatistics {
        float meanReward;
        float stdDevReward;
        float meanPerformance;
        float stdDevPerformance;
        float minPerformance;
        float maxPerformance;
        float coefficientOfVariation;
        size_t successCount;
        size_t failureCount;
    };
    
    SeedStatistics computeStatistics(const std::vector<LifetimeExperimentResult>& results);
    
    /**
     * Generate multi-seed report
     */
    std::string generateReport(
        const std::vector<LifetimeExperimentResult>& results,
        const SeedStatistics& stats
    );

private:
    uint64_t baseSeed_;
    std::mt19937_64 rng_;
};

// Inline implementations

inline LifetimeExperimentConfig::LifetimeExperimentConfig()
    : initialNeurons(10000)
    , maxNeurons(100000)
    , connectionProbability(0.1f)
    , synapseFormationSteps(10000)
    , sensoryDevelopmentSteps(40000)
    , motorDevelopmentSteps(50000)
    , associativeDevelopmentSteps(100000)
    , memoryConsolidationSteps(100000)
    , socialDevelopmentSteps(100000)
    , trialsPerPhase(5)
    , stepsPerTrial(10000)
    , enableMultithreading(true)
    , enableSIMD(true)
    , enableSparseConnectivity(true)
    , enableEventDriven(true)
    , threadCount(0)  // auto
    , saveCheckpoints(true)
    , generateVisualizations(true)
    , runAblations(true) {}

inline LifetimeExperimentResult::LifetimeExperimentResult()
    : totalReward(0.0f)
    , avgRewardPerStep(0.0f)
    , overallLearningEfficiency(0.0f)
    , finalPerformance(0.0f)
    , memoryCapacity(0.0f)
    , predictionAccuracy(0.0f)
    , generalizationAbility(0.0f)
    , startTime(0)
    , endTime(0)
    , totalWallClockTime(0.0) {}

} // namespace nlm