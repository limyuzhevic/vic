#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test with advanced features
 * 
 * This enhanced experiment demonstrates that the integrated brain systems work together
 * with comprehensive testing and analysis capabilities for power users:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * Advanced experiment types:
 * - Integration verification with detailed reports
 * - Performance benchmarking with scaling tests
 * - Ablation studies for system analysis
 * - Continual learning with task switching
 * - Stress testing with extreme conditions
 * 
 * The experiment runs comprehensive lifetime simulations and verifies
 * that all systems interact properly under various conditions.
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
#include <chrono>
#include <variant>
#include <filesystem>

namespace nlm {

/**
 * Phase 6 integration test result
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
};

/**
 * Configuration for Phase 6 integration experiment
 */
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    // Performance optimization settings
    bool enableSIMD;
    bool enableParallel;
    bool enableMemoryPools;
    
    // Phase 6 specific settings
    float predictionConfidence;
    float attentionStrength;
    float workingMemoryCapacity;
    float episodicMemoryMaxEpisodes;
    float developmentStageSpeed;
    float neuromodulationGain;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , enableSIMD(true)
        , enableParallel(false)
        , enableMemoryPools(true)
        , predictionConfidence(0.5f)
        , attentionStrength(1.0f)
        , workingMemoryCapacity(1000.0f)
        , episodicMemoryMaxEpisodes(1000.0f)
        , developmentStageSpeed(1.0f)
        , neuromodulationGain(1.0f) {}
};

/**
 * Advanced experiment configuration
 */
struct AdvancedExperimentConfig {
    // Experiment type
    enum class Type {
        STANDARD_INTEGRATION,
        DETAILED_INTEGRATION_VERIFICATION,
        PERFORMANCE_BENCHMARKING,
        ABLATION_STUDY,
        CONTINUAL_LEARNING,
        STRESS_TESTING,
        MULTI_RUN_STATISTICAL
    };
    
    // Basic settings
    Type experimentType;
    std::string experimentName;
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Advanced settings
    bool enableDetailedReports;
    bool enableStatisticalAnalysis;
    bool enableVisualizations;
    bool enableMultiSeedExperiments;
    size_t numSeeds;
    
    // Configuration for specific experiment types
    struct IntegrationVerificationConfig {
        bool testWorkingMemory;
        bool testEpisodicMemory;
        bool testNeuromodulation;
        bool testPrediction;
        bool testDevelopment;
        bool testCheckpointing;
        bool testReplay;
        size_t verificationSteps;
    } integrationVerification;
    
    struct PerformanceBenchmarkConfig {
        std::vector<size_t> neuronScales;
        std::vector<uint64_t> seeds;
        uint64_t stepsPerScale;
        uint64_t warmupSteps;
        bool runUntilStable;
        double stabilityThreshold;
        size_t maxStabilityChecks;
        bool enableMultithreading;
        bool enableSIMD;
        bool enableSparseConnectivity;
        bool enableEventDriven;
        size_t threadCount;
    } performanceBenchmark;
    
    struct AblationStudyConfig {
        std::string targetTypes;
        size_t stepsPerTrial;
        size_t trialsPerCondition;
        uint64_t randomSeed;
        bool enableStatisticalComparison;
        bool enableDetailedMetrics;
    } ablationStudy;
    
    struct ContinualLearningConfig {
        size_t numCycles;
        size_t tasksPerCycle;
        uint64_t stepsPerTask;
        bool enableTaskSwitching;
        bool enableTransferLearning;
        bool enableMetaLearning;
    } continualLearning;
    
    struct StressTestConfig {
        bool extremeConditions;
        bool memoryPressure;
        bool cpuOverload;
        bool connectionExtremes;
        bool timePressure;
        float memoryLimitGB;
        float cpuLimitPercent;
        uint64_t connectionProbabilityRange;
    } stressTest;
    
    struct MultiRunConfig {
        size_t numRuns;
        uint64_t runDelayMs;
        std::string outputPrefix;
        bool parallelExecution;
        size_t maxParallelRuns;
        bool automaticAggregation;
    } multiRun;
    
    AdvancedExperimentConfig()
        : experimentType(Type::DETAILED_INTEGRATION_VERIFICATION)
        , experimentName("phase6_advanced")
        , maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableDetailedReports(true)
        , enableStatisticalAnalysis(true)
        , enableVisualizations(true)
        , enableMultiSeedExperiments(false)
        , numSeeds(1)
        , integrationVerification({true, true, true, true, true, true, true, 1000})
        , performanceBenchmark({
            std::vector<size_t>{1000, 5000, 10000, 50000, 100000},
            std::vector<uint64_t>{42, 43, 44, 45, 46},
            1000, 100, false, 0.05, 100,
            true, true, true, true, 0
        })
        , ablationStudy({"all", 10000, 5, 42, true, true})
        , continualLearning({3, 5, 10000, true, true, false})
        , stressTest({false, false, false, false, false, 8.0, 100.0, 1})
        , multiRun({1, 0, "experiment", false, 4, true}) {}
};

/**
 * Detailed integration verification result
 */
struct DetailedIntegrationResult {
    // System integration details
    struct SystemIntegration {
        bool workingMemoryIntegrated;
        bool episodicMemoryIntegrated;
        bool neuromodulationIntegrated;
        bool predictionIntegrated;
        bool developmentIntegrated;
        bool checkpointingWorks;
        bool replayWorks;
        
        std::string workingMemoryDetails;
        std::string episodicMemoryDetails;
        std::string neuromodulationDetails;
        std::string predictionDetails;
        std::string developmentDetails;
        std::string checkpointingDetails;
        std::string replayDetails;
    } integration;
    
    // Performance metrics
    struct PerformanceMetrics {
        double wallClockTime;
        double simulatedTime;
        double stepsPerSecond;
        float totalReward;
        float avgFiringRate;
        float avgSynapticWeight;
        float memoryEfficiency;
        float noveltyCapture;
        float curiosityUtilization;
        float neuromodulationEffectiveness;
        float predictionAccuracy;
        float developmentProgress;
    } performance;
    
    // Comparison metrics
    struct ComparisonMetrics {
        float vsBaselineReward;
        float vsBaselineFiringRate;
        float vsBaselineMemory;
        float vsBaselineNovelty;
        float vsBaselineCuriosity;
        float vsBaselinePrediction;
        float vsBaselineDevelopment;
        float vsBaselineCheckpointing;
        float vsBaselineReplay;
    } comparison;
    
    // Confidence intervals
    struct ConfidenceIntervals {
        double confidenceLevel;
        double rewardCI;
        double firingRateCI;
        double memoryCI;
    } confidence;
    
    // Recommendations
    std::vector<std::string> recommendations;
    std::vector<std::string> warnings;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    
    std::string toJSON() const;
    std::string toCSV() const;
    std::string summary() const;
};

/**
 * Performance benchmarking result
 */
struct PerformanceBenchmarkResult {
    std::vector<BenchmarkRun> runs;
    ScalingAnalysis analysis;
    
    float overallEfficiency;
    float scalabilityScore;
    float performanceScore;
    float memoryEfficiency;
    float hardwareUtilization;
    
    std::string bottleneck;
    double bottleneckSeverity;
    
    std::vector<std::string> recommendations;
    
    std::string toJSON() const;
    std::string summary() const;
};

/**
 * Ablation study result
 */
struct AblationStudyResult {
    std::vector<AblationResult> results;
    AblationComparison comparison;
    
    float overallImpact;
    float criticalMechanismScore;
    float redundancyScore;
    
    std::string toJSON() const;
    std::string summary() const;
};

/**
 * Continual learning result
 */
struct ContinualLearningResult {
    float finalPerformance;
    float taskSwitchEfficiency;
    float transferLearningEffect;
    float metaLearningProgress;
    float knowledgeRetention;
    
    std::vector<std::string> learnedTasks;
    std::vector<float> taskPerformances;
    std::vector<double> taskSwitchingTimes;
    
    std::string toJSON() const;
    std::string summary() const;
};

/**
 * Stress testing result
 */
struct StressTestResult {
    float systemStability;
    float failureRate;
    float recoveryTime;
    float performanceUnderStress;
    
    struct StressMetrics {
        float memoryUtilization;
        float cpuUtilization;
        float connectionStability;
        float temporalStability;
    } metrics;
    
    std::vector<std::string> stressPoints;
    std::vector<std::string> failureModes;
    
    std::string toJSON() const;
    std::string summary() const;
};

/**
 * Multi-run statistical analysis result
 */
struct MultiRunResult {
    std::vector<Phase6IntegrationResult> runs;
    
    float meanReward;
    float rewardStdDev;
    float rewardCV;
    float minReward;
    float maxReward;
    
    float meanPerformance;
    float performanceStdDev;
    
    float successRate;
    float failureRate;
    
    std::string toJSON() const;
    std::string summary() const;
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     */
    bool testReplay();
    
    // ===== ADVANCED EXPERIMENT TYPES =====
    
    /**
     * Run detailed integration verification with comprehensive reports
     */
    DetailedIntegrationResult runDetailedIntegrationVerification(const AdvancedExperimentConfig& config);
    
    /**
     * Run performance benchmarking with scaling tests
     */
    PerformanceBenchmarkResult runPerformanceBenchmarking(const AdvancedExperimentConfig& config);
    
    /**
     * Run ablation study for system analysis
     */
    AblationStudyResult runAblationStudy(const AdvancedExperimentConfig& config);
    
    /**
     * Run continual learning experiment with task switching
     */
    ContinualLearningResult runContinualLearning(const AdvancedExperimentConfig& config);
    
    /**
     * Run stress testing with extreme conditions
     */
    StressTestResult runStressTesting(const AdvancedExperimentConfig& config);
    
    /**
     * Run multi-run experiment for statistical significance
     */
    MultiRunResult runMultiRun(const AdvancedExperimentConfig& config);
    
    /**
     * Run all advanced experiment types
     */
    std::unordered_map<std::string, std::variant<DetailedIntegrationResult, PerformanceBenchmarkResult, AblationStudyResult, ContinualLearningResult, StressTestResult, MultiRunResult>> runAllAdvancedExperiments(const AdvancedExperimentConfig& config);
    
    /**
     * Save experiment results to files
     */
    bool saveResults(const std::string& outputDir, const std::string& experimentName);
    
    /**
     * Load experiment results from files
     */
    bool loadResults(const std::string& inputDir, const std::string& experimentName);
    
    /**
     * Generate comprehensive report
     */
    std::string generateReport(const std::unordered_map<std::string, std::variant<DetailedIntegrationResult, PerformanceBenchmarkResult, AblationStudyResult, ContinualLearningResult, StressTestResult, MultiRunResult>>& results) const;
    
    /**
     * Set advanced configuration
     */
    void setAdvancedConfig(const AdvancedExperimentConfig& config) { advancedConfig_ = config; }
    
    /**
     * Get current advanced configuration
     */
    const AdvancedExperimentConfig& getAdvancedConfig() const { return advancedConfig_; }
    
private:
    AdvancedExperimentConfig advancedConfig_;
    
    DetailedIntegrationResult runDetailedIntegrationVerificationImpl(const AdvancedExperimentConfig& config);
    PerformanceBenchmarkResult runPerformanceBenchmarkingImpl(const AdvancedExperimentConfig& config);
    AblationStudyResult runAblationStudyImpl(const AdvancedExperimentConfig& config);
    ContinualLearningResult runContinualLearningImpl(const AdvancedExperimentConfig& config);
    StressTestResult runStressTestingImpl(const AdvancedExperimentConfig& config);
    MultiRunResult runMultiRunImpl(const AdvancedExperimentConfig& config);
    
    std::string generateDetailedReport(const DetailedIntegrationResult& result) const;
    std::string generateBenchmarkReport(const PerformanceBenchmarkResult& result) const;
    std::string generateAblationReport(const AblationStudyResult& result) const;
    std::string generateContinualLearningReport(const ContinualLearningResult& result) const;
    std::string generateStressTestReport(const StressTestResult& result) const;
    std::string generateMultiRunReport(const MultiRunResult& result) const;
    
    bool saveToFile(const std::string& filepath, const std::string& content) const;
    std::string loadFromFile(const std::string& filepath) const;
};

} // namespace nlm
