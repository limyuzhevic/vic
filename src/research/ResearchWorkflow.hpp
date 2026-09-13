#pragma once

#include "ResearchConfig.hpp"
#include "Experiment.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <future>
#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

namespace nlm {

// Research workflow execution status
enum class WorkflowStatus {
    NotStarted,
    InProgress,
    Completed,
    Failed,
    Cancelled
};

// Workflow step result
struct WorkflowStepResult {
    std::string stepName;
    WorkflowStatus status;
    std::string description;
    std::chrono::duration<double> executionTime;
    std::map<std::string, double> metrics;
    std::string errorMessage;
    
    WorkflowStepResult() : status(WorkflowStatus::NotStarted), executionTime(0.0) {}
};

// Cross-configuration comparison result
struct ComparisonResult {
    std::string config1Name;
    std::string config2Name;
    std::string comparisonType;
    std::map<std::string, double> results;
    std::map<std::string, double> differences;
    std::string statisticalSignificance;
    
    ComparisonResult() : comparisonType("pairwise") {}
};

// Visualization configuration
struct VisualizationConfig {
    std::string outputFormat;
    std::string outputPath;
    bool enable3D;
    bool enableAnimations;
    double resolution;
    std::string colorScheme;
    std::vector<std::string> metricsToVisualize;
    
    VisualizationConfig() : outputFormat("PNG"), enable3D(false), enableAnimations(false), 
                          resolution(1.0), colorScheme("default") {}
};

// Research workflow automation system
class ResearchWorkflow {
public:
    ResearchWorkflow();
    ~ResearchWorkflow();
    
    // Configuration
    void setConfigManager(std::shared_ptr<ResearchConfigManager> configManager);
    void setExperimentRunner(std::shared_ptr<ExperimentRunner> runner);
    void setParallelProcessor(std::shared_ptr<ParallelBatchProcessor> processor);
    
    // Workflow execution
    std::string createSequentialWorkflow(const std::vector<std::string>& configNames);
    std::string createCrossComparisonWorkflow(const std::vector<std::string>& configNames);
    std::string createAblationWorkflow(const std::string& baselineConfig, 
                                      const std::vector<std::string>& ablationTargets);
    
    std::string executeWorkflow(const std::string& workflowId);
    bool pauseWorkflow(const std::string& workflowId);
    bool resumeWorkflow(const std::string& workflowId);
    bool cancelWorkflow(const std::string& workflowId);
    
    // Workflow management
    const std::string& getCurrentWorkflowId() const;
    WorkflowStatus getWorkflowStatus(const std::string& workflowId) const;
    std::vector<std::string> getWorkflowIds() const;
    std::vector<std::string> getActiveWorkflowIds() const;
    
    // Step management
    std::vector<std::string> getWorkflowSteps(const std::string& workflowId) const;
    const WorkflowStepResult& getStepResult(const std::string& workflowId, 
                                           const std::string& stepName) const;
    std::map<std::string, WorkflowStepResult> getAllStepResults(const std::string& workflowId) const;
    
    // Statistical analysis
    std::string runStatisticalAnalysis(const std::string& workflowId, 
                                      const std::string& analysisType,
                                      const std::string& metricName);
    std::string generateStatisticalReport(const std::string& workflowId) const;
    std::string generateComparisonReport(const std::string& workflowId) const;
    
    // Visualization
    bool generateVisualizations(const std::string& workflowId, 
                               const VisualizationConfig& config);
    bool saveVisualization(const std::string& workflowId, 
                          const std::string& filename) const;
    
    // Results management
    std::shared_ptr<Experiment> getExperimentResult(const std::string& workflowId,
                                                   const std::string& stepName) const;
    std::vector<std::shared_ptr<Experiment>> getAllWorkflowResults(const std::string& workflowId) const;
    bool saveWorkflowResults(const std::string& workflowId, const std::string& filepath) const;
    
    // Configuration management
    bool addConfiguration(const ResearchConfig& config);
    bool removeConfiguration(const std::string& configName);
    const ResearchConfig* getConfiguration(const std::string& configName) const;
    std::vector<std::string> getConfigurationNames() const;
    
    // Progress tracking
    double getWorkflowProgress(const std::string& workflowId) const;
    std::chrono::duration<double> getEstimatedTimeRemaining(const std::string& workflowId) const;
    std::string getCurrentWorkflowProgressReport(const std::string& workflowId) const;
    
    // Monitoring and reporting
    std::string generateProgressReport(const std::string& workflowId) const;
    std::string generateDetailedReport(const std::string& workflowId) const;
    std::string generateCompleteReport(const std::string& workflowId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Workflow thread function
    void workflowThread(const std::string& workflowId);
    
    // Step execution
    bool executeStep(const std::string& workflowId, const std::string& stepName);
    void updateStepProgress(const std::string& workflowId, const std::string& stepName, 
                           double progress);
    
    // Statistical analysis functions
    std::string analyzePredictionError(const std::string& workflowId, 
                                      const std::string& metricName) const;
    std::string analyzeLearningCurve(const std::string& workflowId,
                                     const std::string& metricName) const;
    std::string analyzeRetention(const std::string& workflowId,
                                 const std::string& metricName) const;
    std::string analyzePerformance(const std::string& workflowId,
                                   const std::string& metricName) const;
    
    // Visualization functions
    bool createLearningCurvePlot(const std::string& workflowId,
                                const VisualizationConfig& config) const;
    bool createComparisonPlot(const std::string& workflowId,
                             const VisualizationConfig& config) const;
    bool createMetricsDistributionPlot(const std::string& workflowId,
                                      const VisualizationConfig& config) const;
    
    // Helper functions
    std::string generateStepId(const std::string& workflowId, const std::string& stepName);
    size_t getCompletedSteps(const std::string& workflowId) const;
    double calculateWorkflowProgress(const std::string& workflowId) const;
    
    // Thread synchronization
    void notifyWorkflowThreads();
    void waitForWorkflowThreads(std::chrono::milliseconds timeout = std::chrono::milliseconds(100));
};

// Statistical analysis utilities
namespace workflow_utils {
    // Statistical test functions
    struct StatisticalResult {
        std::string testType;
        double statistic;
        double pValue;
        std::string interpretation;
        std::string recommendation;
        
        StatisticalResult() : statistic(0.0), pValue(1.0) {}
    };
    
    // Hypothesis testing
    StatisticalResult performTTest(const std::vector<double>& data1, 
                                  const std::vector<double>& data2,
                                  const std::string& testName);
    
    StatisticalResult performANOVA(const std::vector<std::vector<double>>& datasets,
                                   const std::string& testName);
    
    StatisticalResult performRegression(const std::vector<double>& x,
                                        const std::vector<double>& y,
                                        const std::string& testName);
    
    // Effect size calculation
    double calculateEffectSize(double mean1, double mean2, double stddev1, double stddev2);
    
    // Data analysis utilities
    double calculateMean(const std::vector<double>& data);
    double calculateStdDev(const std::vector<double>& data, double mean = -1.0);
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    double calculateCoefficientOfVariation(const std::vector<double>& data, double mean = -1.0);
    
    // Data formatting and reporting
    std::string formatStatisticalResult(const StatisticalResult& result);
    std::string formatConfidenceInterval(double mean, double stddev, double n, double confidence = 0.95);
    
    // Workflow analysis
    std::string identifyLearningPhase(const std::vector<double>& metricValues,
                                       double stabilityThreshold = 0.01);
    std::string detectPlateau(const std::vector<double>& metricValues,
                              double plateauThreshold = 0.001,
                              int plateauLength = 50);
    
    // Visualization data preparation
    std::string generatePlotData(const std::vector<std::pair<double, double>>& points,
                                const std::string& format);
    std::string generateHistogramData(const std::vector<double>& data,
                                      const std::vector<double>& bins);
    std::string generateTimeSeriesData(const std::vector<double>& timeSeries,
                                       const std::vector<double>& values);
}

} // namespace nlm