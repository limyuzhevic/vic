#pragma once

/**
 * Enhanced Experiment Framework for NLM Phase 6
 * 
 * This framework provides advanced experiment management capabilities:
 * - Enhanced experiment configuration with validation
 * - Detailed experiment results with comprehensive metrics
 * - Parallel experiment execution
 * - Experiment comparison and analysis
 * - Experiment versioning and history tracking
 * - Performance monitoring and optimization
 */

#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <thread>
#include <future>
#include <algorithm>
#include <numeric>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <mutex>
#include <set>
#include "src/experiments/Experiment.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"
#include "src/core/Config/Config.hpp"

namespace nlm {

/**
 * Enhanced experiment configuration with validation
 */
struct EnhancedExperimentConfig {
    // Basic configuration
    uint64_t maxSteps = 10000;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Memory system configuration
    size_t workingMemorySize = 100;
    size_t episodicMemoryCapacity = 1000;
    float memoryRetentionRate = 0.95f;
    
    // Neuromodulation configuration
    float dopamineStrength = 1.0f;
    float curiosityThreshold = 0.5f;
    float noveltySensitivity = 0.8f;
    float rewardScale = 1.0f;
    
    // Development configuration
    bool enableDevelopment = true;
    float developmentRate = 0.001f;
    size_t criticalPeriodStart = 100;
    size_t criticalPeriodEnd = 1000;
    
    // Prediction system configuration
    size_t predictionHorizon = 10;
    float predictionConfidenceThreshold = 0.7f;
    float predictionLearningRate = 0.01f;
    
    // Advanced experiment settings
    bool enableCheckpointing = true;
    std::string checkpointPath = "./checkpoints/";
    bool enableReplay = true;
    size_t replaySampleSize = 10;
    bool enableParallelExecution = true;
    size_t maxConcurrentExperiments = 4;
    
    // Performance settings
    double timeStep = 0.001;
    size_t simulationThreads = 1;
    bool enableSIMD = true;
    
    // Logging and monitoring
    std::string logFile = "experiment.log";
    bool enableProgressReporting = true;
    float progressReportInterval = 0.1f; // 10% progress
    
    // Validation and error handling
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    EnhancedExperimentConfig withOverrides(const std::string& key, const ConfigValue& value) const;
};

/**
 * Enhanced experiment results with comprehensive metrics
 */
struct EnhancedExperimentResult {
    // Basic information
    std::string experimentId;
    std::string configurationId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::chrono::duration<double> totalWallClockTime;
    
    // Performance metrics
    double totalReward;
    double averageFiringRate;
    double averageSynapticWeight;
    double weightChangeRate;
    double explorationRate;
    double exploitationRate;
    
    // Memory system metrics
    float memoryWorkingMemoryUtilization;
    float memoryEpisodicMemoryUtilization;
    size_t memoryEpisodicMemoryEpisodes;
    float memoryEpisodicMemoryRecallAccuracy;
    
    // Neuromodulation metrics
    float neuromodulationDopamineLevel;
    float neuromodulationCuriosityLevel;
    float neuromodulationNoveltyLevel;
    float neuromodulationPredictionError;
    
    // Cognitive system metrics
    float cognitionPlanningAccuracy;
    float cognitionConceptStability;
    float cognitionAttentionSelectivity;
    float cognitionSelfModelAccuracy;
    float cognitionSocialLearningAccuracy;
    
    // Development metrics
    float developmentDevelopmentalAge;
    float developmentDevelopmentalStage;
    float developmentPlasticityRate;
    float developmentSynaptogenesisRate;
    float developmentPruningRate;
    
    // Prediction system metrics
    float predictionPredictionAccuracy;
    float predictionPredictionConfidence;
    float predictionErrorMean;
    float predictionErrorStdDev;
    
    // Integration metrics
    bool integrationMemoryWorkingMemory;
    bool integrationMemoryEpisodicMemory;
    bool integrationNeuromodulation;
    bool integrationPrediction;
    bool integrationDevelopment;
    bool integrationCheckpointing;
    bool integrationReplay;
    
    // Experiment status
    bool success;
    std::vector<std::string> errorMessages;
    std::vector<std::string> warnings;
    
    // Detailed timeseries data (sampled)
    std::vector<double> rewardHistory;
    std::vector<double> firingRateHistory;
    std::vector<double> synapticWeightHistory;
    std::vector<double> noveltyHistory;
    std::vector<double> curiosityHistory;
    
    // Statistical analysis
    double rewardMean;
    double rewardStdDev;
    double rewardSkew;
    double rewardKurtosis;
    
    // Behavioral analysis
    std::map<std::string, double> actionStatistics;
    std::map<std::string, double> movementStatistics;
    std::map<std::string, double> interactionStatistics;
    
    // Comparative analysis
    std::vector<std::string> comparableExperiments;
    std::map<std::string, double> comparativeMetrics;
    
    // Additional metadata
    std::unordered_map<std::string, std::string> metadata;
    
    // Helper methods
    double getMetric(const std::string& name) const;
    void setMetric(const std::string& name, double value);
    bool hasMetric(const std::string& name) const;
    std::vector<std::string> getAllMetricNames() const;
    
    // Statistical calculations
    double calculateMean(const std::vector<double>& data) const;
    double calculateStdDev(const std::vector<double>& data, double mean) const;
    double calculateSkew(const std::vector<double>& data, double mean, double stdDev) const;
    double calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) const;
    
    // Time formatting
    std::string formatTime(std::chrono::system_clock::time_point time) const;
    double formatTimeDiff(std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point end) const;
};

/**
 * Enhanced experiment runner with advanced features
 */
class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner();
    ~EnhancedExperimentRunner();
    
    // Configuration management
    void setDefaultConfig(const EnhancedExperimentConfig& config);
    EnhancedExperimentConfig getConfig() const;
    void updateConfig(const std::string& key, const ConfigValue& value);
    EnhancedExperimentConfig createConfigFromJson(const std::string& filepath) const;
    bool saveConfigToJson(const std::string& filepath) const;
    
    // Experiment execution
    EnhancedExperimentResult runExperiment(const EnhancedExperimentConfig& config);
    EnhancedExperimentResult runExperiment(const std::string& configId);
    std::vector<EnhancedExperimentResult> runExperiments(const std::vector<EnhancedExperimentConfig>& configs);
    EnhancedExperimentResult runEnhancedExperiment(const EnhancedExperimentConfig& config);
    
    // Parallel execution
    EnhancedExperimentResult runExperimentParallel(const EnhancedExperimentConfig& config, int workerThreads);
    std::vector<EnhancedExperimentResult> runExperimentsParallel(const std::vector<EnhancedExperimentConfig>& configs);
    
    // Experiment management
    std::string saveExperimentResult(const EnhancedExperimentResult& result);
    EnhancedExperimentResult loadExperimentResult(const std::string& experimentId);
    bool deleteExperimentResult(const std::string& experimentId);
    std::vector<std::string> getExperimentIds() const;
    std::vector<EnhancedExperimentResult> getAllExperimentResults() const;
    
    // Experiment comparison
    EnhancedExperimentResult compareExperiments(const std::vector<std::string>& experimentIds);
    double compareExperiments(const std::string& experimentId1, const std::string& experimentId2, const std::string& metricName);
    bool areExperimentsComparable(const std::string& experimentId1, const std::string& experimentId2) const;
    
    // Analysis and reporting
    std::string generateExperimentReport(const EnhancedExperimentResult& result) const;
    std::string generateComparativeReport(const std::vector<std::string>& experimentIds) const;
    std::string generateStatisticalReport(const EnhancedExperimentResult& result) const;
    std::string generatePerformanceReport(const EnhancedExperimentResult& result) const;
    
    // Data export and import
    bool exportResultsToCSV(const std::vector<EnhancedExperimentResult>& results, const std::string& filepath) const;
    bool exportResultsToJSON(const std::vector<EnhancedExperimentResult>& results, const std::string& filepath) const;
    std::vector<EnhancedExperimentResult> importResultsFromCSV(const std::string& filepath);
    std::vector<EnhancedExperimentResult> importResultsFromJSON(const std::string& filepath);
    
    // Experiment history and versioning
    void saveExperimentVersion(const std::string& experimentId, const std::string& versionName, const EnhancedExperimentConfig& config);
    std::vector<std::string> getExperimentVersions(const std::string& experimentId) const;
    EnhancedExperimentConfig getExperimentVersion(const std::string& experimentId, const std::string& versionName) const;
    
    // Performance monitoring
    double getAverageExperimentTime() const;
    double getMedianExperimentTime() const;
    size_t getExperimentCount() const;
    std::chrono::duration<double> getTotalExecutionTime() const;
    
    // Configuration management
    void saveConfiguration(const std::string& configId, const EnhancedExperimentConfig& config);
    EnhancedExperimentConfig loadConfiguration(const std::string& configId) const;
    bool deleteConfiguration(const std::string& configId);
    std::vector<std::string> getConfigurationIds() const;
    
    // Experiment analysis
    EnhancedExperimentResult analyzeExperiment(const EnhancedExperimentResult& result);
    std::vector<std::string> identifyKeyInsights(const EnhancedExperimentResult& result) const;
    std::map<std::string, double> extractPatternFromExperiment(const EnhancedExperimentResult& result) const;
    
    // Experiment validation
    bool validateExperimentConfig(const EnhancedExperimentConfig& config) const;
    std::vector<std::string> validateExperimentResult(const EnhancedExperimentResult& result) const;
    
    // Utility functions
    static std::string generateExperimentId();
    static std::string generateConfigurationId();
    static std::string timestampToString(const std::chrono::system_clock::time_point& time);
    static std::chrono::system_clock::time_point stringToTimestamp(const std::string& timestamp);
    
    // Progress monitoring
    void setProgressCallback(std::function<void(double)> callback);
    void setExperimentStartCallback(std::function<void(const std::string&)> callback);
    void setExperimentEndCallback(std::function<void(const EnhancedExperimentResult&)> callback);
    
    // Configuration templates
    static EnhancedExperimentConfig getTemplate(const std::string& templateName);
    static std::vector<std::string> getAvailableTemplates();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    EnhancedExperimentResult convertToEnhancedResult(const ExperimentResult& oldResult) const;
    ExperimentResult convertToOldResult(const EnhancedExperimentResult& newResult) const;
    EnhancedExperimentConfig convertToEnhancedConfig(const Phase6Config& oldConfig) const;
    Phase6Config convertToOldConfig(const EnhancedExperimentConfig& newConfig) const;
    
    // File management
    std::string getExperimentFilePath(const std::string& experimentId) const;
    std::string getConfigFilePath(const std::string& configId) const;
    bool createDirectoriesIfNeeded(const std::string& filepath) const;
    
    // Data parsing
    EnhancedExperimentConfig parseConfigFromJson(const std::string& jsonStr) const;
    EnhancedExperimentResult parseResultFromJson(const std::string& jsonStr) const;
    std::string serializeConfigToJson(const EnhancedExperimentConfig& config) const;
    std::string serializeResultToJson(const EnhancedExperimentResult& result) const;
    
    // Experiment execution
    EnhancedExperimentResult executeSingleExperiment(const EnhancedExperimentConfig& config);
    EnhancedExperimentResult executeEnhancedSingleExperiment(const EnhancedExperimentConfig& config);
    
    // Experiment conversion
    EnhancedExperimentResult convertLegacyExperiment(const std::string& experimentId);
    EnhancedExperimentResult convertLegacyResult(const ExperimentResult& oldResult) const;
};

/**
 * Experiment configuration manager
 */
class ExperimentConfigManager {
public:
    ExperimentConfigManager();
    ~ExperimentConfigManager();
    
    // Configuration management
    void addConfiguration(const std::string& configId, const EnhancedExperimentConfig& config);
    EnhancedExperimentConfig getConfiguration(const std::string& configId) const;
    bool updateConfiguration(const std::string& configId, const EnhancedExperimentConfig& config);
    bool deleteConfiguration(const std::string& configId);
    std::vector<std::string> getAllConfigurationIds() const;
    
    // Configuration templates
    void addTemplate(const std::string& templateName, const EnhancedExperimentConfig& config);
    EnhancedExperimentConfig getTemplate(const std::string& templateName) const;
    bool deleteTemplate(const std::string& templateName);
    std::vector<std::string> getAllTemplateNames() const;
    
    // Configuration comparison
    bool areConfigurationsCompatible(const std::string& configId1, const std::string& configId2) const;
    EnhancedExperimentConfig mergeConfigurations(const std::string& configId1, const std::string& configId2) const;
    
    // Configuration validation
    std::vector<std::string> validateConfiguration(const EnhancedExperimentConfig& config) const;
    bool isConfigurationValid(const std::string& configId) const;
    
    // Configuration export/import
    bool exportConfiguration(const std::string& configId, const std::string& filepath) const;
    bool importConfiguration(const std::string& configId, const std::string& filepath);
    bool exportAllConfigurations(const std::string& directory) const;
    bool importAllConfigurations(const std::string& directory);
    
    // Configuration history
    void saveConfigurationVersion(const std::string& configId, const std::string& versionName, const EnhancedExperimentConfig& config);
    std::vector<std::string> getConfigurationVersions(const std::string& configId) const;
    EnhancedExperimentConfig getConfigurationVersion(const std::string& configId, const std::string& versionName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Experiment results analyzer
 */
class ExperimentResultsAnalyzer {
public:
    ExperimentResultsAnalyzer();
    ~ExperimentResultsAnalyzer();
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& data) const;
    double calculateStdDev(const std::vector<double>& data, double mean) const;
    double calculateSkew(const std::vector<double>& data, double mean, double stdDev) const;
    double calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculateMode(const std::vector<double>& data) const;
    
    // Data analysis
    std::vector<double> normalizeData(const std::vector<double>& data, double mean, double stdDev) const;
    std::vector<double> standardizeData(const std::vector<double>& data) const;
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    std::map<std::string, double> calculateAllStatistics(const std::vector<double>& data) const;
    
    // Pattern detection
    std::vector<std::string> detectTrends(const std::vector<double>& data) const;
    std::vector<std::string> detectPeaks(const std::vector<double>& data) const;
    std::vector<std::string> detectValleys(const std::vector<double>& data) const;
    std::vector<std::string> detectAnomalies(const std::vector<double>& data, double threshold) const;
    
    // Experiment comparison
    EnhancedExperimentResult compareExperiments(const std::vector<EnhancedExperimentResult>& results) const;
    std::map<std::string, double> compareMetrics(const std::vector<EnhancedExperimentResult>& results, const std::string& metricName) const;
    bool areResultsComparable(const EnhancedExperimentResult& result1, const EnhancedExperimentResult& result2) const;
    
    // Performance analysis
    double calculatePerformanceScore(const EnhancedExperimentResult& result) const;
    std::vector<std::string> identifyPerformanceIssues(const EnhancedExperimentResult& result) const;
    double calculateEfficiency(const EnhancedExperimentResult& result) const;
    double calculateEffectiveness(const EnhancedExperimentResult& result) const;
    
    // Behavioral analysis
    std::map<std::string, double> analyzeBehavior(const EnhancedExperimentResult& result) const;
    double calculateExplorationExploitationBalance(const EnhancedExperimentResult& result) const;
    std::vector<std::string> identifyBehavioralPatterns(const EnhancedExperimentResult& result) const;
    
    // Memory analysis
    double calculateMemoryEfficiency(const EnhancedExperimentResult& result) const;
    double calculateMemoryUtilization(const EnhancedExperimentResult& result) const;
    std::vector<std::string> analyzeMemoryUsage(const EnhancedExperimentResult& result) const;
    
    // Integration analysis
    double calculateIntegrationScore(const EnhancedExperimentResult& result) const;
    std::vector<std::string> analyzeIntegration(const EnhancedExperimentResult& result) const;
    
    // Time series analysis
    std::vector<std::string> detectTimeSeriesAnomalies(const std::vector<double>& timeSeries) const;
    double calculateTimeSeriesTrend(const std::vector<double>& timeSeries) const;
    std::vector<std::pair<double, double>> detectTimeSeriesCycles(const std::vector<double>& timeSeries) const;
    
    // Export analysis results
    std::string exportAnalysisResults(const std::vector<EnhancedExperimentResult>& results) const;
    bool saveAnalysisResults(const std::vector<EnhancedExperimentResult>& results, const std::string& filepath) const;
    std::vector<EnhancedExperimentResult> loadAnalysisResults(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Enhanced experiment configuration templates
 */
class EnhancedExperimentTemplates {
public:
    static EnhancedExperimentConfig createSmallScaleExperiment() {
        EnhancedExperimentConfig config;
        config.maxSteps = 1000;
        config.neuronCount = 500;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableDevelopment = false;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableParallelExecution = false;
        config.simulationThreads = 1;
        return config;
    }
    
    static EnhancedExperimentConfig createMediumScaleExperiment() {
        EnhancedExperimentConfig config;
        config.maxSteps = 10000;
        config.neuronCount = 2000;
        config.regionCount = 2;
        config.connectionProbability = 0.15f;
        config.enableDevelopment = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableParallelExecution = true;
        config.simulationThreads = 2;
        return config;
    }
    
    static EnhancedExperimentConfig createLargeScaleExperiment() {
        EnhancedExperimentConfig config;
        config.maxSteps = 50000;
        config.neuronCount = 10000;
        config.regionCount = 4;
        config.connectionProbability = 0.2f;
        config.enableDevelopment = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableParallelExecution = true;
        config.simulationThreads = 4;
        return config;
    }
    
    static EnhancedExperimentConfig createDebugExperiment() {
        EnhancedExperimentConfig config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.regionCount = 1;
        config.connectionProbability = 0.05f;
        config.enableDevelopment = false;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableParallelExecution = false;
        config.simulationThreads = 1;
        config.enableProgressReporting = true;
        config.progressReportInterval = 0.5f;
        return config;
    }
    
    static EnhancedExperimentConfig createProductionExperiment() {
        EnhancedExperimentConfig config;
        config.maxSteps = 100000;
        config.neuronCount = 50000;
        config.regionCount = 8;
        config.connectionProbability = 0.25f;
        config.enableDevelopment = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableParallelExecution = true;
        config.simulationThreads = 8;
        config.enableProgressReporting = true;
        config.progressReportInterval = 0.1f;
        config.enableSIMD = true;
        return config;
    }
    
    static EnhancedExperimentConfig createResearchExperiment() {
        EnhancedExperimentConfig config;
        config.maxSteps = 200000;
        config.neuronCount = 100000;
        config.regionCount = 16;
        config.connectionProbability = 0.3f;
        config.enableDevelopment = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableParallelExecution = true;
        config.simulationThreads = 16;
        config.enableProgressReporting = true;
        config.progressReportInterval = 0.05f;
        config.enableSIMD = true;
        return config;
    }
};

} // namespace nlm