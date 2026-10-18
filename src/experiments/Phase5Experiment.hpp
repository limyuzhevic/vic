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

/**
 * System profiling and performance monitoring
 */
struct SystemProfile {
    // Basic metrics
    uint64_t step;
    size_t totalNeurons;
    size_t totalSynapses;
    
    // Neural activity metrics
    size_t totalSpikes;
    size_t firingNeurons;
    size_t activeNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    
    // Memory system metrics
    size_t workingMemoryActiveTraces;
    size_t workingMemoryCapacity;
    float workingMemoryUtilization;
    size_t episodicMemoryEpisodeCount;
    size_t episodicMemoryReplayEvents;
    size_t associativeMemoryAssociationCount;
    float associativeMemoryStrength;
    
    // Neuromodulation metrics
    float dopamineLevel;
    float dopaminePlasticityFactor;
    float curiosityLevel;
    
    // Prediction system metrics
    float predictionAccuracy;
    float predictionConfidence;
    
    // Performance metrics
    float efficiency;           // Network efficiency score
    float throughput;          // Operations per second
    float energyConsumption;    // Relative energy usage
    double collectionTime;      // Time to collect profile
    
    // Resource utilization
    float cpuUsage;             // Estimated CPU usage (%)
    float memoryUsage;          // Estimated memory usage (MB)
    
    // Timestamps
    double timestamp;
    
    SystemProfile() : step(0), totalNeurons(0), totalSynapses(0),
                      totalSpikes(0), firingNeurons(0), activeNeurons(0),
                      averageFiringRate(0.0f), excitationInhibitionRatio(0.0f),
                      workingMemoryActiveTraces(0), workingMemoryCapacity(0),
                      workingMemoryUtilization(0.0f), episodicMemoryEpisodeCount(0),
                      episodicMemoryReplayEvents(0), associativeMemoryAssociationCount(0),
                      associativeMemoryStrength(0.0f), dopamineLevel(0.0f),
                      dopaminePlasticityFactor(0.0f), curiosityLevel(0.0f),
                      predictionAccuracy(0.0f), predictionConfidence(0.0f),
                      efficiency(0.0f), throughput(0.0f), energyConsumption(0.0f),
                      collectionTime(0.0), cpuUsage(0.0f), memoryUsage(0.0f),
                      timestamp(0.0) {}
};

struct ProfilingConfig {
    // Profile scope
    bool profileNeuralActivity;
    bool profileMemorySystems;
    bool profileNeuromodulation;
    bool profilePredictionSystems;
    bool profileResourceUsage;
    
    // Profile frequency
    double profileInterval;     // seconds between profiles
    bool continuousProfiling;   // true for real-time profiling
    
    // Metrics collection
    bool collectCPUUsage;
    bool collectMemoryUsage;
    bool collectEnergyConsumption;
    bool collectEfficiencyMetrics;
    bool collectThroughputMetrics;
    
    // Output options
    std::string profileOutputDir;
    bool saveProfiles;
    bool generateAnalysis;
    bool generateVisualizations;
    
    ProfilingConfig() : profileNeuralActivity(true), profileMemorySystems(true),
                       profileNeuromodulation(true), profilePredictionSystems(true),
                       profileResourceUsage(true), profileInterval(1.0),
                       continuousProfiling(false), collectCPUUsage(true),
                       collectMemoryUsage(true), collectEnergyConsumption(true),
                       collectEfficiencyMetrics(true), collectThroughputMetrics(true),
                       saveProfiles(true), generateAnalysis(true), generateVisualizations(true) {}
};

/**
 * Enhanced lifetime experiment result with performance profiling data
 */
struct EnhancedLifetimeExperimentResult {
    LifetimeExperimentResult basicResult;
    
    // Performance profiling data
    std::vector<SystemProfile> systemProfiles;
    std::vector<BenchmarkMetrics> performanceMetrics;
    
    // Scalability analysis
    std::vector<std::pair<size_t, double>> scaleVsEfficiency;
    std::vector<std::pair<size_t, double>> scaleVsThroughput;
    
    // Resource utilization trends
    std::vector<std::pair<double, float>> timeVsMemoryUsage;
    std::vector<std::pair<double, float>> timeVsCPUUsage;
    std::vector<std::pair<double, float>> timeVsEnergyConsumption;
    
    // Optimization insights
    std::vector<std::string> optimizationRecommendations;
    std::vector<std::string> performanceBottlenecks;
    std::unordered_map<std::string, float> efficiencyScores;
    
    // Statistical analysis
    struct PerformanceStatistics {
        float meanEfficiency;
        float stdDevEfficiency;
        float meanThroughput;
        float stdDevThroughput;
        float meanMemoryUsage;
        float stdDevMemoryUsage;
        float meanEnergyConsumption;
        float stdDevEnergyConsumption;
        float minEfficiency;
        float maxEfficiency;
        float pearsonCorrelation;
    } statistics;
    
    EnhancedLifetimeExperimentResult() : statistics() {}
};

/**
 * Performance analysis and optimization recommendations
 */
struct PerformanceAnalysis {
    std::string title;
    
    // Performance bottlenecks
    struct Bottleneck {
        std::string name;
        std::string description;
        float severity;        // 0.0 to 1.0
        float impact;          // 0.0 to 1.0
        std::string recommendation;
        float estimatedGain;   // Expected improvement if fixed
    };
    std::vector<Bottleneck> bottlenecks;
    
    // Performance trends
    struct Trend {
        std::string metric;
        std::vector<std::pair<double, float>> dataPoints;
        float trendSlope;
        float rSquared;
        std::string description;
    };
    std::vector<Trend> trends;
    
    // Optimization opportunities
    struct Optimization {
        std::string name;
        std::string technique;
        float potentialImprovement;
        float implementationCost;
        float roi;            // Return on investment
        std::string description;
    };
    std::vector<Optimization> optimizations;
    
    // Performance projections
    struct Projection {
        std::string scenario;
        float projectedEfficiency;
        float projectedThroughput;
        float projectedMemoryUsage;
        float timeToImplement;
    };
    std::vector<Projection> projections;
    
    // Overall assessment
    float overallScore;
    std::string assessment;
    std::vector<std::string> keyFindings;
};

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

LifetimeExperimentResult run(
        const LifetimeExperimentConfig& config,
        std::function<std::shared_ptr<class Brain>(const LifetimeExperimentConfig& config)> createBrain,
        std::function<bool(class Brain* brain, uint64_t steps)> runSimulation
    );
    
    /**
     * Run the enhanced lifetime experiment with comprehensive performance profiling
     */
    EnhancedLifetimeExperimentResult runWithProfiling(
        const LifetimeExperimentConfig& config,
        std::function<std::shared_ptr<class Brain>(const LifetimeExperimentConfig& config)> createBrain,
        std::function<bool(class Brain* brain, uint64_t steps)> runSimulation,
        const ProfilingConfig& profilingConfig = ProfilingConfig()
    );
    
    /**
     * Generate performance analysis and optimization recommendations
     */
    PerformanceAnalysis generatePerformanceAnalysis(
        const EnhancedLifetimeExperimentResult& result
    );
    
    /**
     * Profile system performance at a specific point
     */
    bool profileSystemPerformance(
        class Brain* brain,
        SystemProfile& profile,
        const ProfilingConfig& config = ProfilingConfig()
    );
    
    /**
     * Calculate network efficiency score
     */
    float calculateNetworkEfficiency(class Brain* brain);
    
    /**
     * Calculate network throughput
     */
    float calculateNetworkThroughput(const SystemProfile& profile);
    
    /**
     * Estimate CPU usage
     */
    float estimateCPUUsage(const SystemProfile& profile);
    
    /**
     * Estimate memory usage
     */
    float estimateMemoryUsage(const SystemProfile& profile);
    
    /**
     * Estimate energy consumption
     */
    float estimateEnergyConsumption(const SystemProfile& profile);
    
    /**
     * Save system profile to file
     */
    bool saveSystemProfile(
        const SystemProfile& profile,
        const std::string& filepath,
        const ProfilingConfig& config
    );
    
    /**
     * Load system profile from file
     */
    bool loadSystemProfile(
        SystemProfile& profile,
        const std::string& filepath
    );
    
    /**
     * Compare performance profiles
     */
    std::vector<SystemProfile> compareProfiles(
        const std::vector<SystemProfile>& profiles,
        const std::string& comparisonType = "efficiency"
    );
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport(
        const EnhancedLifetimeExperimentResult& result,
        const PerformanceAnalysis& analysis
    );
    
    /**
     * Export performance data to JSON
     */
    std::string exportPerformanceToJSON(
        const EnhancedLifetimeExperimentResult& result,
        const PerformanceAnalysis& analysis
    );
    
    /**
     * Import performance data from JSON
     */
    bool importPerformanceFromJSON(
        EnhancedLifetimeExperimentResult& result,
        PerformanceAnalysis& analysis,
        const std::string& filepath
    );
    
    /**
     * Create comprehensive benchmarking report
     */
    std::string createBenchmarkingReport(
        const EnhancedLifetimeExperimentResult& result,
        const std::vector<BenchmarkMetrics>& benchmarkResults,
        const PerformanceAnalysis& analysis
    );
    
    /**
     * Update experiment configuration with optimization recommendations
     */
    LifetimeExperimentConfig applyOptimizationRecommendations(
        const LifetimeExperimentConfig& originalConfig,
        const PerformanceAnalysis& analysis
    );
    
    /**
     * Generate visualization data for performance metrics
     */
    struct VisualizationData {
        std::vector<double> timePoints;
        std::vector<float> efficiencyValues;
        std::vector<float> throughputValues;
        std::vector<float> memoryUsageValues;
        std::vector<float> energyConsumptionValues;
        std::vector<float> firingRateValues;
    };
    
    VisualizationData generateVisualizationData(
        const EnhancedLifetimeExperimentResult& result
    );

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