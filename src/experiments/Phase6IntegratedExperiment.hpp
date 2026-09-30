#pragma once

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

/**
 * Advanced analysis data
 */
struct AdvancedAnalysis {
    // Time series data
    std::vector<std::pair<double, float>> spikeTimes;
    std::vector<std::pair<double, float>> firingRates;
    std::vector<std::pair<double, float>> noveltyLevels;
    std::vector<std::pair<double, float>> curiosityLevels;
    std::vector<std::pair<double, float>> dopamineLevels;
    
    // Spatial activity
    std::vector<std::pair<float, float>> activityPositions;
    std::vector<float> activityStrengths;
    
    // Behavioral data
    std::vector<std::pair<double, MotorCommand>> actions;
    std::vector<std::pair<double, float>> rewards;
    std::vector<std::pair<double, float>> predictionErrors;
    
    // Learning metrics
    std::vector<std::pair<double, float>> plasticityRates;
    std::vector<std::pair<double, float>> memoryConsolidation;
    std::vector<std::pair<double, float>> attentionStrengths;
    
    // Quality metrics
    float avgFiringRate;
    float avgSpikingBurstRate;
    float avgActionEntropy;
    float avgNovelty;
    float avgCuriosity;
    float avgDopamine;
    float coherenceScore;
    float burstinessScore;
    float predictabilityScore;
    
    // Neural dynamics
    float meanMembranePotential;
    float membranePotentialStd;
    float thresholdCrossingRate;
    float refractoryPeriod;
    float synapticDynamics;
    
    // Analysis metadata
    size_t totalSpikes;
    size_t totalActions;
    size_t uniqueActions;
    size_t totalReplays;
    size_t successfulCheckpoints;
    double analysisStartTime;
    double analysisEndTime;
    std::string analysisID;
    
    AdvancedAnalysis() : 
        avgFiringRate(0.0f), avgSpikingBurstRate(0.0f), avgActionEntropy(0.0f),
        avgNovelty(0.0f), avgCuriosity(0.0f), avgDopamine(0.0f),
        coherenceScore(0.0f), burstinessScore(0.0f), predictabilityScore(0.0f),
        meanMembranePotential(0.0f), membranePotentialStd(0.0f),
        thresholdCrossingRate(0.0f), refractoryPeriod(0.0f), synapticDynamics(0.0f),
        totalSpikes(0), totalActions(0), uniqueActions(0), totalReplays(0),
        successfulCheckpoints(0), analysisStartTime(0.0), analysisEndTime(0.0) {}
};

/**
 * Advanced experiment configuration
 */
struct AdvancedPhase6Config {
    // Core configuration
    Phase6Config baseConfig;
    
    // Advanced features
    bool enableRealTimeMonitoring;
    bool enablePerformanceProfiling;
    bool enableDetailedLogging;
    bool enableVisualizationExport;
    bool enableAblationStudies;
    bool enableCustomScenarios;
    
    // Analysis parameters
    std::vector<std::string> analysisMetrics;
    std::vector<std::function<bool(const AdvancedAnalysis&)>> analysisFilters;
    double analysisInterval;
    std::string analysisOutputFormat;
    
    // Simulation control
    std::function<void(double)> customEventHandler;
    std::vector<std::pair<double, std::string>> customEvents;
    bool pauseSimulation;
    double simulationSpeed;
    
    // Learning control
    bool enableMetaLearning;
    bool enableSelfTuning;
    bool enableTransferLearning;
    std::string learningStrategy;
    double learningRate;
    double explorationRate;
    
    // Memory management
    bool enableMemoryProfiling;
    size_t memorySampleSize;
    bool enableMemoryAnalysis;
    bool enableReplayAnalysis;
    
    // Advanced features
    bool enableNeuralProfiling;
    bool enableSynapticAnalysis;
    bool enableCircuitAnalysis;
    std::vector<std::string> customAnalysisTypes;
    
    AdvancedPhase6Config()
        : enableRealTimeMonitoring(true)
        , enablePerformanceProfiling(true)
        , enableDetailedLogging(true)
        , enableVisualizationExport(false)
        , enableAblationStudies(false)
        , enableCustomScenarios(false)
        , analysisInterval(0.1)
        , analysisOutputFormat("json")
        , pauseSimulation(false)
        , simulationSpeed(1.0)
        , enableMetaLearning(false)
        , enableSelfTuning(false)
        , enableTransferLearning(false)
        , learningStrategy("balanced")
        , learningRate(0.01)
        , explorationRate(0.1)
        , enableMemoryProfiling(false)
        , memorySampleSize(1000)
        , enableMemoryAnalysis(false)
        , enableReplayAnalysis(false)
        , enableNeuralProfiling(false)
        , enableSynapticAnalysis(false)
        , enableCircuitAnalysis(false) {}
};

/**
 * Performance metrics
 */
struct PerformanceMetrics {
    // Basic metrics
    double totalExecutionTime;
    double cpuUsage;
    double memoryUsage;
    double diskIO;
    
    // Brain performance
    double brainInitializationTime;
    double simulationTimePerStep;
    double memoryProcessingTime;
    double neuralPropagationTime;
    
    // Agent performance
    double agentInitializationTime;
    double sensoryProcessingTime;
    double motorDecodingTime;
    double neuromodulationTime;
    
    // World performance
    double worldUpdateTime;
    double collisionDetectionTime;
    double graphicsRenderingTime;
    
    // Network performance (if applicable)
    double networkUploadTime;
    double networkDownloadTime;
    double synchronizationTime;
    
    // Memory performance
    double memoryAllocationTime;
    double memoryDeallocationTime;
    double memoryCopyTime;
    double memoryAccessTime;
    
    // Throughput metrics
    double spikesPerSecond;
    double actionsPerSecond;
    double bytesPerSecond;
    
    // Quality metrics
    double simulationStability;
    double learningEfficiency;
    double behavioralCohesion;
    double neuralCohesion;
    
    // Resource usage
    size_t peakMemoryUsage;
    size_t totalMemoryAllocated;
    size_t totalMemoryDeallocated;
    
    PerformanceMetrics()
        : totalExecutionTime(0.0)
        , cpuUsage(0.0)
        , memoryUsage(0.0)
        , diskIO(0.0)
        , brainInitializationTime(0.0)
        , simulationTimePerStep(0.0)
        , memoryProcessingTime(0.0)
        , neuralPropagationTime(0.0)
        , agentInitializationTime(0.0)
        , sensoryProcessingTime(0.0)
        , motorDecodingTime(0.0)
        , neuromodulationTime(0.0)
        , worldUpdateTime(0.0)
        , collisionDetectionTime(0.0)
        , graphicsRenderingTime(0.0)
        , networkUploadTime(0.0)
        , networkDownloadTime(0.0)
        , synchronizationTime(0.0)
        , memoryAllocationTime(0.0)
        , memoryDeallocationTime(0.0)
        , memoryCopyTime(0.0)
        , memoryAccessTime(0.0)
        , spikesPerSecond(0.0)
        , actionsPerSecond(0.0)
        , bytesPerSecond(0.0)
        , simulationStability(0.0)
        , learningEfficiency(0.0)
        , behavioralCohesion(0.0)
        , neuralCohesion(0.0)
        , peakMemoryUsage(0)
        , totalMemoryAllocated(0)
        , totalMemoryDeallocated(0) {}
};

/**
 * Advanced experiment result with detailed analysis
 */
struct AdvancedPhase6IntegrationResult {
    // Basic results
    Phase6IntegrationResult baseResult;
    
    // Advanced analysis
    AdvancedAnalysis detailedAnalysis;
    PerformanceMetrics performanceMetrics;
    
    // Advanced integration status
    bool metaLearningEnabled;
    bool selfRepairEnabled;
    bool advancedNeuromodulation;
    bool complexMemoryArchitecture;
    bool adaptivePlasticity;
    bool autonomousExploration;
    bool continuousLearning;
    bool lifeLongDevelopment;
    
    // Quality scores
    float learningProgressScore;
    float behavioralAdaptationScore;
    float neuralEfficiencyScore;
    float memoryRetentionScore;
    float predictionAccuracyScore;
    float survivalFitnessScore;
    
    // Advanced metrics
    float synapticStability;
    float memoryConsolidationQuality;
    float attentionAllocation;
    float rewardPredictionAccuracy;
    float explorationExploitationBalance;
    float developmentalTrajectoryScore;
    
    // Timing analysis
    std::vector<double> stageTimings;
    std::vector<double> learningCurve;
    std::vector<double> performanceTrend;
    
    // Pattern analysis
    std::vector<std::vector<float>> behavioralPatterns;
    std::vector<std::vector<float>> neuralPatterns;
    std::vector<std::vector<float>> environmentalResponses;
    
    // Custom analysis results
    std::unordered_map<std::string, std::string> customAnalysisResults;
    std::vector<std::string> discoveredBehaviors;
    std::vector<std::string> learnedStrategies;
    
    // Advanced metadata
    std::string experimentDescription;
    std::string hypothesisTested;
    std::string methodology;
    std::string conclusions;
    
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string experimentID;
    
    AdvancedPhase6IntegrationResult()
        : metaLearningEnabled(false)
        , selfRepairEnabled(false)
        , advancedNeuromodulation(false)
        , complexMemoryArchitecture(false)
        , adaptivePlasticity(false)
        , autonomousExploration(false)
        , continuousLearning(false)
        , lifeLongDevelopment(false)
        , learningProgressScore(0.0f)
        , behavioralAdaptationScore(0.0f)
        , neuralEfficiencyScore(0.0f)
        , memoryRetentionScore(0.0f)
        , predictionAccuracyScore(0.0f)
        , survivalFitnessScore(0.0f)
        , synapticStability(0.0f)
        , memoryConsolidationQuality(0.0f)
        , attentionAllocation(0.0f)
        , rewardPredictionAccuracy(0.0f)
        , explorationExploitationBalance(0.0f)
        , developmentalTrajectoryScore(0.0f) {}
};

/**
 * Enhanced Phase 6 integrated experiment with advanced features
 */
class EnhancedPhase6IntegratedExperiment {
public:
    EnhancedPhase6IntegratedExperiment();
    ~EnhancedPhase6IntegratedExperiment();
    
    /**
     * Run advanced integration test with detailed analysis
     */
    AdvancedPhase6IntegrationResult runAdvanced(const AdvancedPhase6Config& config);
    
    /**
     * Run custom experiment scenario
     */
    AdvancedPhase6IntegrationResult runCustomScenario(const std::string& scenarioName,
                                                      const AdvancedPhase6Config& config);
    
    /**
     * Run ablative study
     */
    AdvancedPhase6IntegrationResult runAblationStudy(const std::string& ablationTarget,
                                                     const AdvancedPhase6Config& config);
    
    /**
     * Run performance benchmark
     */
    PerformanceMetrics benchmarkPerformance(const AdvancedPhase6Config& config);
    
    /**
     * Export analysis results
     */
    bool exportAnalysis(const std::string& filepath, const AdvancedAnalysis& analysis);
    bool exportResults(const std::string& filepath, const AdvancedPhase6IntegrationResult& result);
    
    /**
     * Get real-time monitoring data
     */
    std::vector<AdvancedAnalysis> getRealTimeData() const;
    PerformanceMetrics getCurrentPerformanceMetrics() const;
    
    /**
     * Control experiment execution
     */
    void pauseSimulation(bool pause);
    void setSimulationSpeed(double speed);
    void addCustomEvent(double timestep, const std::string& eventType, const std::string& eventData);
    void setEventHandler(const std::function<void(double)>& handler);
    
    /**
     * Advanced analysis control
     */
    void addAnalysisMetric(const std::string& metric);
    void removeAnalysisMetric(const std::string& metric);
    void addAnalysisFilter(const std::function<bool(const AdvancedAnalysis&)>& filter);
    void setAnalysisInterval(double interval);
    void enableRealTimeAnalysis(bool enable);
    
    /**
     * Memory management
     */
    void enableMemoryProfiling(bool enable);
    void setMemorySampleSize(size_t samples);
    std::vector<double> getMemoryUsageHistory() const;
    bool analyzeMemoryLeaks() const;
    
    /**
     * Neural network analysis
     */
    void enableNeuralProfiling(bool enable);
    void enableSynapticAnalysis(bool enable);
    void enableCircuitAnalysis(bool enable);
    std::unordered_map<std::string, float> analyzeNeuralTopology() const;
    std::vector<std::pair<float, float>> getActivityHeatmap() const;
    
    /**
     * Learning analysis
     */
    void enableMetaLearning(bool enable);
    void enableSelfTuning(bool enable);
    void enableTransferLearning(bool enable);
    std::vector<float> getLearningProgressCurve() const;
    std::vector<float> getAdaptationCurve() const;
    float calculateLearningEfficiency() const;
    
    /**
     * Results comparison and validation
     */
    bool compareResults(const AdvancedPhase6IntegrationResult& baseline) const;
    std::vector<std::string> validateResults(const AdvancedPhase6IntegrationResult& result) const;
    float calculateSimilarityScore(const AdvancedPhase6IntegrationResult& other) const;
    
    /**
     * Export/Import utilities
     */
    bool saveExperimentState(const std::string& filepath) const;
    bool loadExperimentState(const std::string& filepath);
    std::string generateExperimentReport(const AdvancedPhase6IntegrationResult& result) const;
    
private:
    // Core experiment
    Phase6IntegratedExperiment coreExperiment;
    
    // Advanced features
    std::atomic<bool> simulationPaused;
    double simulationSpeed;
    std::chrono::system_clock::time_point experimentStartTime;
    std::chrono::system_clock::time_point experimentEndTime;
    
    // Real-time monitoring
    std::vector<AdvancedAnalysis> realTimeData;
    std::mutex dataMutex;
    bool realTimeAnalysisEnabled;
    double analysisInterval;
    
    // Performance metrics
    PerformanceMetrics currentPerformanceMetrics;
    std::chrono::steady_clock::time_point performanceStartTime;
    
    // Analysis
    std::vector<std::string> activeAnalysisMetrics;
    std::vector<std::function<bool(const AdvancedAnalysis&)>> analysisFilters;
    std::vector<std::pair<double, std::string>> customEvents;
    std::function<void(double)> eventHandler;
    
    // Memory analysis
    bool memoryProfilingEnabled;
    size_t memorySampleSize;
    std::vector<double> memoryUsageHistory;
    
    // Neural analysis
    bool neuralProfilingEnabled;
    bool synapticAnalysisEnabled;
    bool circuitAnalysisEnabled;
    
    // Learning analysis
    bool metaLearningEnabled;
    bool selfTuningEnabled;
    bool transferLearningEnabled;
    std::vector<float> learningProgressCurve;
    std::vector<float> adaptationCurve;
    
    // Custom scenarios and ablations
    std::unordered_map<std::string, std::function<AdvancedPhase6IntegrationResult(const AdvancedPhase6Config&)>> customScenarios;
    std::unordered_map<std::string, std::function<AdvancedPhase6IntegrationResult(const AdvancedPhase6Config&)>> ablations;
    
    // Export/Import
    std::unordered_map<std::string, AdvancedAnalysis> analysisCache;
    std::unordered_map<std::string, AdvancedPhase6IntegrationResult> resultCache;
    
    // Helper methods
    AdvancedAnalysis collectAdvancedAnalysis(const Phase6IntegrationResult& baseResult,
                                             const PerformanceMetrics& perfMetrics) const;
    PerformanceMetrics measurePerformance() const;
    void updateRealTimeData(const AdvancedAnalysis& analysis);
    bool applyCustomEvents(double currentTime, std::shared_ptr<Brain> brain,
                          std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) const;
    std::string generateAnalysisReport(const AdvancedAnalysis& analysis) const;
    std::string generateResultsReport(const AdvancedPhase6IntegrationResult& result) const;
    std::string generateComparisonReport(const AdvancedPhase6IntegrationResult& result,
                                         const AdvancedPhase6IntegrationResult& baseline) const;
    
    // Helper functions
    static AdvancedAnalysis createEmptyAnalysis();
    static PerformanceMetrics createEmptyPerformanceMetrics();
    static std::string generateAnalysisID();
    static std::string generateExperimentID();
};

/**
 * Advanced experiment utilities
 */
namespace AdvancedExperiment {
    
    /**
     * Create advanced configuration with sensible defaults
     */
    AdvancedPhase6Config createAdvancedConfig(const std::string& mode = "balanced");
    
    /**
     * Create custom scenario configuration
     */
    AdvancedPhase6Config createCustomScenario(const std::string& scenarioName);
    
    /**
     * Create ablative study configuration
     */
    AdvancedPhase6Config createAblationConfig(const std::string& target);
    
    /**
     * Export experiment data
     */
    bool exportAnalysisData(const std::string& filepath, const AdvancedAnalysis& analysis);
    bool exportResultsData(const std::string& filepath, const AdvancedPhase6IntegrationResult& result);
    bool exportComparisonData(const std::string& filepath, 
                             const AdvancedPhase6IntegrationResult& result,
                             const AdvancedPhase6IntegrationResult& baseline);
    
    /**
     * Import experiment data
     */
    bool importAnalysisData(AdvancedAnalysis& analysis, const std::string& filepath);
    bool importResultsData(AdvancedPhase6IntegrationResult& result, const std::string& filepath);
    
    /**
     * Analysis utilities
     */
    std::string analyzeSpikeTrain(const std::vector<double>& spikeTimes);
    std::string analyzeBehavior(const std::vector<MotorCommand>& actions);
    std::string analyzeLearning(const std::vector<float>& rewards);
    std::string analyzeMemory(const std::vector<float>& memoryScores);
    
    /**
     * Export formats
     */
    std::string toJSON(const AdvancedAnalysis& analysis);
    std::string toCSV(const AdvancedAnalysis& analysis);
    std::string toPDF(const AdvancedAnalysis& analysis);
    std::string toHTML(const AdvancedAnalysis& analysis);
    
    /**
     * Configuration presets
     */
    AdvancedPhase6Config getQuickTestConfig();
    AdvancedPhase6Config getProductionConfig();
    AdvancedPhase6Config getResearchConfig();
    AdvancedPhase6Config getBenchmarkConfig();
    
    /**
     * Performance benchmarks
     */
    bool runPerformanceBenchmark(const AdvancedPhase6Config& config);
    bool runMemoryBenchmark(const AdvancedPhase6Config& config);
    bool runScalabilityBenchmark(const AdvancedPhase6Config& config);
    
} // namespace AdvancedExperiment

} // namespace nlm