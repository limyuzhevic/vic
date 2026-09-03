#pragma once

/**
 * AblationSystem - Framework for systematic mechanism ablation
 * 
 * Enables researchers to disable specific neural mechanisms and measure
 * their contribution to overall behavior and learning.
 * 
 * Ablatable mechanisms:
 * - STDP plasticity
 * - Hebbian learning
 * - Neuromodulation (dopamine, novelty, curiosity)
 * - Reward modulation
 * - Structural plasticity (synaptogenesis, pruning)
 * - Memory systems (working, episodic)
 * - Prediction system
 * - Attention
 * - Specific neuron types
 * - Specific brain regions
 * 
 * Features:
 * - Atomic mechanism enable/disable
 * - Combined ablation support
 * - Statistical comparison
 * - Automated experiment runner
 */

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>

namespace nlm {

/**
 * Identifies a neural mechanism that can be ablated
 */
enum class AblationTarget : uint32_t {
    // Plasticity mechanisms
    STDP = 0,
    Hebbian = 1,
    RewardModulation = 2,
    EligibilityTraces = 3,
    
    // Structural plasticity
    Synaptogenesis = 4,
    Pruning = 5,
    StructuralPlasticity = 6,
    
    // Neuromodulation
    Dopamine = 7,
    Novelty = 8,
    Curiosity = 9,
    Neuromodulation = 10,
    
    // Memory systems
    WorkingMemory = 11,
    EpisodicMemory = 12,
    SemanticMemory = 13,
    ProceduralMemory = 14,
    Memory = 15,
    
    // Cognitive mechanisms
    Prediction = 16,
    Attention = 17,
    Planning = 18,
    SelfModel = 19,
    
    // Sensory systems
    Vision = 20,
    Audio = 21,
    Touch = 22,
    Proprioception = 23,
    InternalSensing = 24,
    
    // Neuron types
    ExcitatoryNeurons = 25,
    InhibitoryNeurons = 26,
    SensoryNeurons = 27,
    MotorNeurons = 28,
    
    // Brain regions
    Region1 = 100,
    Region2 = 101,
    Region3 = 102,
    // ... up to Region 20
    
    // Meta targets (combinations)
    AllPlasticity = 200,
    AllNeuromodulation = 201,
    AllMemory = 202,
    AllSensation = 203,
    AllCognition = 204,
    
    // Control
    None = 255
};

/**
 * Represents an ablation configuration
 */
struct AblationConfig {
    std::unordered_set<AblationTarget> disabledTargets;
    std::unordered_map<AblationTarget, std::string> customDescriptions;
    
    AblationConfig() {}
    
    void disable(AblationTarget target) {
        disabledTargets.insert(target);
    }
    
    void enable(AblationTarget target) {
        disabledTargets.erase(target);
    }
    
    bool isDisabled(AblationTarget target) const {
        return disabledTargets.count(target) > 0;
    }
    
    void disableAll() {
        disabledTargets.insert(AblationTarget::AllPlasticity);
        disabledTargets.insert(AblationTarget::AllNeuromodulation);
        disabledTargets.insert(AblationTarget::AllMemory);
        disabledTargets.insert(AblationTarget::AllCognition);
    }
    
    void enableAll() {
        disabledTargets.clear();
    }
};

/**
 * Result of an ablation experiment
 */
struct AblationResult {
    std::string name;
    AblationConfig config;
    
    // Performance metrics
    float totalReward;
    float avgRewardPerStep;
    float learningEfficiency;
    float finalPerformance;
    
    // Neural activity metrics
    float avgFiringRate;
    float spikeCount;
    float synapticWeightSum;
    float excitatoryInhibitoryRatio;
    
    // Memory metrics (if applicable)
    float memoryRetention;
    float memoryRecall;
    
    // Prediction metrics (if applicable)
    float predictionAccuracy;
    float predictionError;
    
    // Stability metrics
    float activityStability;
    float weightStability;
    
    // Behavioral metrics
    float explorationRate;
    float goalDirectedness;
    float adaptationSpeed;
    
    // Statistical comparisons
    float performanceVsFull;
    float statisticalSignificance;
    bool isSignificant;
    
    // Metadata
    uint64_t stepsRun;
    double computationTime;
    size_t memoryUsageMB;
    
    AblationResult();
    
    std::string toCSV() const;
    std::string toJSON() const;
};

/**
 * Baseline comparison result
 */
struct AblationComparison {
    std::string fullSystemName;
    std::string ablatedSystemName;
    
    // Ratio metrics (ablated / full)
    float rewardRatio;
    float firingRateRatio;
    float stabilityRatio;
    float memoryRatio;
    float predictionRatio;
    
    // Difference metrics (full - ablated)
    float rewardDifference;
    float firingRateDifference;
    
    // Interpretation
    std::string interpretation;
    std::vector<std::string> keyFindings;
};

/**
 * Ablation experiment definition
 */
struct AblationExperiment {
    std::string name;
    std::string description;
    AblationConfig baseline;           // Full system (no ablation)
    std::vector<AblationConfig> ablations;  // Each ablation variant
    uint64_t stepsPerTrial;
    size_t trialsPerCondition;
    uint64_t randomSeed;
    
    AblationExperiment();
};

/**
 * Ablation system interface
 */
class AblationSystem {
public:
    AblationSystem();
    ~AblationSystem();
    
    /**
     * Register the current brain state as the full (non-ablated) baseline
     */
    void setBaseline(const std::string& name);
    
    /**
     * Apply an ablation configuration to the system
     */
    bool applyAblation(const AblationConfig& config);
    
    /**
     * Reset to full (non-ablated) state
     */
    bool resetToFull();
    
    /**
     * Check if a mechanism is currently disabled
     */
    bool isMechanismDisabled(AblationTarget target) const;
    
    /**
     * Get current ablation configuration
     */
    const AblationConfig& getCurrentConfig() const { return currentConfig_; }
    
    /**
     * Get target name
     */
    static std::string getTargetName(AblationTarget target);
    
    /**
     * Get target description
     */
    static std::string getTargetDescription(AblationTarget target);
    
    /**
     * Get all available ablation targets
     */
    static std::vector<AblationTarget> getAllTargets();
    
    /**
     * Create a standard ablation experiment
     */
    static AblationExperiment createStandardExperiment();
    
    /**
     * Create a plasticty ablation experiment
     */
    static AblationExperiment createPlasticityExperiment();
    
    /**
     * Create a neuromodulation ablation experiment
     */
    static AblationExperiment createNeuromodulationExperiment();
    
    /**
     * Create a memory ablation experiment
     */
    static AblationExperiment createMemoryExperiment();

private:
    AblationConfig currentConfig_;
    std::string baselineName_;
    std::unordered_map<AblationTarget, std::function<bool(bool)>> mechanismHandlers_;
};

/**
 * Automated ablation experiment runner
 */
class AblationExperimentRunner {
public:
    AblationExperimentRunner();
    ~AblationExperimentRunner();
    
    /**
     * Run a complete ablation experiment
     * @param experiment Experiment definition
     * @param runFunction Function to run a single trial: bool(steps, config)
     * @return Vector of results for each condition
     */
    std::vector<AblationResult> runExperiment(
        const AblationExperiment& experiment,
        std::function<bool(uint64_t steps, const AblationConfig& config)> runFunction
    );
    
    /**
     * Run multiple seeds for statistical significance
     */
    std::vector<AblationResult> runWithMultipleSeeds(
        const AblationExperiment& experiment,
        size_t numSeeds,
        std::function<bool(uint64_t steps, const AblationConfig& config, uint64_t seed)> runFunction
    );
    
    /**
     * Compare two conditions
     */
    AblationComparison compare(const AblationResult& full, const AblationResult& ablated);
    
    /**
     * Generate summary report
     */
    std::string generateReport(const std::vector<AblationResult>& results);
    
    /**
     * Export results to CSV
     */
    bool exportToCSV(const std::string& filepath, const std::vector<AblationResult>& results);
    
    /**
     * Export results to JSON
     */
    bool exportToJSON(const std::string& filepath, const std::vector<AblationResult>& results);

private:
    std::string generateConditionName(const AblationConfig& config);
};

// Inline implementations

inline AblationResult::AblationResult()
    : totalReward(0.0f)
    , avgRewardPerStep(0.0f)
    , learningEfficiency(0.0f)
    , finalPerformance(0.0f)
    , avgFiringRate(0.0f)
    , spikeCount(0.0f)
    , synapticWeightSum(0.0f)
    , excitatoryInhibitoryRatio(0.0f)
    , memoryRetention(0.0f)
    , memoryRecall(0.0f)
    , predictionAccuracy(0.0f)
    , predictionError(0.0f)
    , activityStability(0.0f)
    , weightStability(0.0f)
    , explorationRate(0.0f)
    , goalDirectedness(0.0f)
    , adaptationSpeed(0.0f)
    , performanceVsFull(0.0f)
    , statisticalSignificance(0.0f)
    , isSignificant(false)
    , stepsRun(0)
    , computationTime(0.0)
    , memoryUsageMB(0) {}

inline std::string AblationResult::toCSV() const {
    return std::to_string(totalReward) + "," +
           std::to_string(avgRewardPerStep) + "," +
           std::to_string(learningEfficiency) + "," +
           std::to_string(finalPerformance) + "," +
           std::to_string(avgFiringRate) + "," +
           std::to_string(spikeCount) + "," +
           std::to_string(excitatoryInhibitoryRatio) + "," +
           std::to_string(predictionAccuracy) + "," +
           std::to_string(predictionError) + "," +
           std::to_string(stepsRun);
}

inline AblationExperiment::AblationExperiment()
    : stepsPerTrial(10000)
    , trialsPerCondition(5)
    , randomSeed(42) {}

} // namespace nlm