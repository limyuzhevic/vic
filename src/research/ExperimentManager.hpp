#pragma once

#include "Experiment.hpp"
#include "ResearchConfig.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>

namespace nlm {

// Experiment tracking status
enum class TrackingStatus {
    Active,
    Completed,
    Failed,
    Paused,
    Archived,
    Invalidated
};

// Experiment metadata
struct ExperimentMetadata {
    std::string experimentId;
    std::string configName;
    std::string description;
    uint64_t seed;
    SimulationStep maxSteps;
    
    // Timing information
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::chrono::duration<double> totalDuration;
    
    // Status tracking
    TrackingStatus status;
    double progress;  // 0.0 to 1.0
    
    // Results summary
    double totalReward;
    double finalFiringRate;
    size_t totalStepsRecorded;
    
    // Quality control
    bool validationPassed;
    std::vector<std::string> validationIssues;
    std::vector<std::string> qualityIssues;
    
    // Reproducibility
    std::string configHash;
    std::string randomSeedHash;
    
    ExperimentMetadata() : seed(0), maxSteps(0), progress(0.0),
                          totalReward(0.0), finalFiringRate(0.0),
                          totalStepsRecorded(0), validationPassed(true) {}
};

// Experiment validation result
struct ValidationResult {
    bool passed;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> recommendations;
    double qualityScore;  // 0.0 to 1.0
    
    ValidationResult() : passed(true), qualityScore(1.0) {}
};

// Experiment comparison result
struct ComparisonResult {
    std::string experimentId1;
    std::string experimentId2;
    double performanceDifference;
    double statisticalSignificance;
    std::string effectSize;
    std::string interpretation;
    
    ComparisonResult() : performanceDifference(0.0), statisticalSignificance(1.0) {}
};

// Experiment tracking and management system
class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager();
    
    // Experiment lifecycle management
    std::string createExperiment(const std::string& configName,
                                 const std::string& description,
                                 uint64_t seed = 0,
                                 SimulationStep maxSteps = 1000);
    
    bool startExperiment(const std::string& experimentId);
    bool pauseExperiment(const std::string& experimentId);
    bool resumeExperiment(const std::string& experimentId);
    bool completeExperiment(const std::string& experimentId);
    bool failExperiment(const std::string& experimentId, const std::string& errorMessage);
    bool archiveExperiment(const std::string& experimentId);
    bool deleteExperiment(const std::string& experimentId);
    
    // Experiment tracking
    bool recordExperimentStep(const std::string& experimentId,
                             SimulationStep step,
                             size_t neuronCount,
                             size_t synapseCount,
                             size_t firingCount,
                             float avgFiringRate);
    
    bool recordExperimentMetric(const std::string& experimentId,
                               const std::string& name,
                               double value);
    
    bool updateExperimentProgress(const std::string& experimentId, double progress);
    
    // Experiment retrieval
    const ExperimentMetadata* getExperimentMetadata(const std::string& experimentId) const;
    const Experiment* getExperimentData(const std::string& experimentId) const;
    std::shared_ptr<Experiment> getExperimentObject(const std::string& experimentId);
    
    // Query experiments
    std::vector<std::string> getExperimentIds() const;
    std::vector<std::string> getExperimentIds(TrackingStatus status) const;
    std::vector<std::string> getExperimentIds(const std::string& configName) const;
    
    // Configuration management
    bool addConfig(const ResearchConfig& config);
    bool removeConfig(const std::string& configName);
    const ResearchConfig* getConfig(const std::string& configName) const;
    std::vector<std::string> getAvailableConfigs() const;
    
    // Validation and quality control
    ValidationResult validateExperiment(const std::string& experimentId) const;
    ValidationResult validateExperimentSetup(const std::string& experimentId) const;
    
    bool addValidationIssue(const std::string& experimentId,
                           const std::string& issue,
                           const std::string& severity = "ERROR");
    
    // Parameter optimization
    std::string optimizeParameters(const std::string& experimentId,
                                   const std::map<std::string, std::string>& constraints);
    bool applyOptimizedParameters(const std::string& experimentId,
                                 const std::map<std::string, std::string>& parameters);
    
    // Reproducibility management
    bool enableReproducibility(const std::string& experimentId);
    bool ensureReproducibility(const std::string& experimentId);
    bool verifyReproducibility(const std::string& experimentId) const;
    
    std::string generateReproducibilityReport(const std::string& experimentId) const;
    
    // Results analysis
    std::string analyzeExperimentResults(const std::string& experimentId) const;
    std::string analyzeExperimentTrends(const std::string& experimentId) const;
    std::string analyzeExperimentPerformance(const std::string& experimentId) const;
    
    // Experiment comparison
    ComparisonResult compareExperiments(const std::string& experimentId1,
                                       const std::string& experimentId2) const;
    std::vector<ComparisonResult> compareExperiments(const std::vector<std::string>& experimentIds) const;
    
    // Results aggregation
    std::string aggregateResults(const std::vector<std::string>& experimentIds) const;
    std::string aggregateResultsByConfig(const std::string& configName) const;
    std::string aggregateResultsByMetric(const std::string& metricName) const;
    
    // Progress tracking
    double getExperimentProgress(const std::string& experimentId) const;
    std::chrono::duration<double> getEstimatedTimeRemaining(const std::string& experimentId) const;
    
    // Reporting
    std::string generateExperimentReport(const std::string& experimentId) const;
    std::string generateBatchReport(const std::vector<std::string>& experimentIds) const;
    std::string generateComprehensiveReport() const;
    
    // Export and import
    bool exportExperiment(const std::string& experimentId, const std::string& filepath) const;
    bool importExperiment(const std::string& filepath, const std::string& configName);
    
    // Configuration validation and reporting
    std::vector<std::string> validateAllConfigs() const;
    std::string generateConfigValidationReport() const;
    std::string generateQualityReport() const;
    
    // Monitoring and status
    bool isExperimentRunning(const std::string& experimentId) const;
    bool isSystemRunning() const;
    size_t getRunningExperimentsCount() const;
    std::string getSystemStatusReport() const;
    
    // System management
    void stopAllExperiments();
    void pauseAllExperiments();
    void resumeAllExperiments();
    void cleanupCompletedExperiments();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    void setPerformanceThresholds(double maxCpuUsage, double maxMemoryUsage);
    std::string getPerformanceReport() const;
    
    // Error handling and recovery
    bool handleExperimentError(const std::string& experimentId, const std::string& errorMessage);
    bool attemptExperimentRecovery(const std::string& experimentId);
    std::string getLastError() const;
    
    // Utility functions
    static std::string generateExperimentId(const std::string& prefix = "exp");
    static std::string generateConfigName(const std::string& prefix = "config");
    static bool isValidExperimentId(const std::string& id);
    static bool isValidConfigName(const std::string& name);
    static double calculateExperimentQualityScore(const ExperimentMetadata& metadata) const;
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    static std::string calculateHash(const std::string& data);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialization helper
    void initializeDefaults();
    
    // Internal helper functions
    void updateExperimentMetadata(const std::string& experimentId);
    bool validateExperimentSetup(const ExperimentMetadata& metadata) const;
    std::string calculateConfigHash(const ResearchConfig& config) const;
    std::string calculateSeedHash(uint64_t seed) const;
    
    // Error handling
    void logError(const std::string& errorMessage);
    void handleCriticalError(const std::string& errorMessage);
};

// Results aggregation and analysis utilities
namespace results_utils {
    // Statistical analysis utilities
    struct StatisticalSummary {
        std::string metricName;
        double mean;
        double stddev;
        double median;
        double min;
        double max;
        double variance;
        double coefficientOfVariation;
        
        StatisticalSummary() : mean(0.0), stddev(0.0), median(0.0), 
                              min(0.0), max(0.0), variance(0.0), 
                              coefficientOfVariation(0.0) {}
    };
    
    // Data analysis functions
    StatisticalSummary calculateStatistics(const std::vector<double>& data);
    double calculateMean(const std::vector<double>& data);
    double calculateStdDev(const std::vector<double>& data, double mean = -1.0);
    double calculateMedian(const std::vector<double>& data);
    double calculatePercentile(const std::vector<double>& data, double percentile);
    
    // Performance analysis
    std::string analyzeLearningCurve(const std::vector<std::pair<SimulationStep, double>>& data);
    std::string analyzePerformanceProfile(const std::vector<std::pair<SimulationStep, double>>& data);
    std::string analyzeStability(const std::vector<double>& data, double threshold = 0.01);
    
    // Experiment comparison
    double calculatePerformanceDifference(const std::vector<double>& data1, 
                                         const std::vector<double>& data2);
    double calculateEffectSize(const std::vector<double>& data1,
                              const std::vector<double>& data2);
    std::string interpretEffectSize(double effectSize);
    
    // Quality assessment
    double calculateReproducibilityScore(const std::string& configHash,
                                        const std::string& seedHash) const;
    double calculateQualityScore(const ExperimentMetadata& metadata) const;
    
    // Report generation
    std::string formatStatisticalSummary(const StatisticalSummary& summary);
    std::string formatExperimentMetadata(const ExperimentMetadata& metadata);
    std::string formatValidationResult(const ValidationResult& result);
    
    // Data export
    std::string exportToCSV(const std::vector<ExperimentMetadata>& metadata);
    std::string exportToJSON(const std::vector<ExperimentMetadata>& metadata);
    std::string exportToBinary(const std::vector<ExperimentMetadata>& metadata);
}

} // namespace nlm