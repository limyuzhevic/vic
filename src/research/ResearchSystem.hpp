#pragma once

#include "ResearchConfigManager.hpp"
#include "ParallelBatchProcessor.hpp"
#include "ResearchWorkflow.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Main research system that integrates all components
class ResearchSystem {
public:
    ResearchSystem();
    ~ResearchSystem();
    
    // System initialization and configuration
    bool initialize(const std::string& configFile = "");
    void shutdown();
    
    // Component access
    std::shared_ptr<ResearchConfigManager> getConfigManager() const;
    std::shared_ptr<ExperimentRunner> getExperimentRunner() const;
    std::shared_ptr<ParallelBatchProcessor> getBatchProcessor() const;
    std::shared_ptr<ResearchWorkflow> getWorkflowManager() const;
    
    // Core research operations
    bool runExperiment(const std::string& configName, const std::string& experimentName);
    bool runBatchExperiment(const std::vector<std::string>& configNames, 
                           const std::string& experimentName);
    bool runWorkflow(const std::string& workflowId);
    
    // Configuration management
    bool addResearchConfig(const std::string& configName, const ResearchConfig& config);
    bool loadResearchConfig(const std::string& configName, const std::string& filepath);
    bool saveResearchConfig(const std::string& configName, const std::string& filepath) const;
    bool removeResearchConfig(const std::string& configName);
    const ResearchConfig* getResearchConfig(const std::string& configName) const;
    std::vector<std::string> getAvailableConfigs() const;
    
    // Configuration presets
    bool loadPresetConfigs();
    ResearchConfig createCustomConfig(const std::string& name, 
                                     const std::string& description,
                                     const std::map<std::string, std::string>& parameters);
    
    // Experiment management
    bool createExperiment(const std::string& name, const std::string& configName);
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    std::vector<std::shared_ptr<Experiment>> getExperiments(const std::string& configName = "") const;
    bool saveExperimentResults(const std::string& experimentName, const std::string& filepath) const;
    
    // Batch processing
    std::string submitBatchJob(const std::string& configName, uint64_t seed, 
                              SimulationStep maxSteps, BatchPriority priority = BatchPriority::Normal);
    bool cancelBatchJob(const std::string& jobId);
    bool pauseBatchJob(const std::string& jobId);
    bool resumeBatchJob(const std::string& jobId);
    const BatchJob* getBatchJob(const std::string& jobId) const;
    BatchStats getBatchStats() const;
    std::string getBatchProgressReport() const;
    
    // Workflow management
    std::string createSequentialWorkflow(const std::vector<std::string>& configNames);
    std::string createCrossComparisonWorkflow(const std::vector<std::string>& configNames);
    std::string createAblationWorkflow(const std::string& baselineConfig,
                                       const std::vector<std::string>& ablationTargets);
    bool executeWorkflow(const std::string& workflowId);
    bool pauseWorkflow(const std::string& workflowId);
    bool resumeWorkflow(const std::string& workflowId);
    bool cancelWorkflow(const std::string& workflowId);
    std::vector<std::string> getActiveWorkflowIds() const;
    std::string getWorkflowProgressReport(const std::string& workflowId) const;
    std::string getWorkflowDetailedReport(const std::string& workflowId) const;
    
    // Statistical analysis and reporting
    std::string runStatisticalAnalysis(const std::string& workflowId,
                                      const std::string& analysisType,
                                      const std::string& metricName);
    std::string generateCompleteResearchReport(const std::string& workflowId) const;
    
    // Visualization
    bool generateVisualizations(const std::string& workflowId,
                               const VisualizationConfig& config);
    bool saveVisualization(const std::string& workflowId, const std::string& filename) const;
    
    // Monitoring and status
    bool isRunning() const;
    bool isPaused() const;
    double getSystemProgress() const;
    std::chrono::duration<double> getEstimatedTimeRemaining() const;
    std::string getSystemStatusReport() const;
    
    // Results aggregation and analysis
    std::vector<std::shared_ptr<Experiment>> getAllExperimentResults() const;
    std::vector<std::shared_ptr<Experiment>> getSuccessfulExperiments() const;
    std::vector<std::shared_ptr<Experiment>> getFailedExperiments() const;
    
    // Configuration validation and reporting
    std::vector<std::string> validateAllConfigs() const;
    std::string generateSystemConfigurationReport() const;
    std::string generateExperimentSummaryReport() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    void setPerformanceThresholds(double maxCpuUsage, double maxMemoryUsage);
    std::string getPerformanceReport() const;
    
    // Result export
    bool exportResults(const std::string& filepath, const std::string& format = "JSON") const;
    bool exportResultsToCSV(const std::string& filepath) const;
    bool exportResultsToJSON(const std::string& filepath) const;
    bool exportResultsToBinary(const std::string& filepath) const;
    
    // Error handling and recovery
    bool handleSystemError(const std::string& errorMessage);
    bool attemptRecovery(const std::string& workflowId);
    std::string getLastError() const;
    
    // Configuration validation utilities
    static bool isValidConfigName(const std::string& name);
    static bool isValidExperimentName(const std::string& name);
    static bool isValidWorkflowId(const std::string& id);
    static std::string normalizeConfigName(const std::string& name);
    static std::string normalizeExperimentName(const std::string& name);
    static std::string normalizeWorkflowId(const std::string& id);
    
    // Utility functions
    static std::string generateExperimentId(const std::string& prefix = "exp");
    static std::string generateWorkflowId(const std::string& prefix = "workflow");
    static double calculateSimilarity(const ResearchConfig& config1, const ResearchConfig& config2);
    
    // Integration with existing NLM components
    void integrateWithExperimentRunner(std::shared_ptr<ExperimentRunner> runner);
    void integrateWithBrain(std::shared_ptr<Brain> brain);
    void integrateWithEnvironment(std::shared_ptr<Environment> environment);
    
    // Research lifecycle management
    void startSystem();
    void stopSystem();
    void pauseSystem();
    void resumeSystem();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialization helper
    void initializeComponents();
    
    // Configuration file helpers
    bool loadConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const std::string& filepath) const;
    
    // Result aggregation
    void aggregateResults();
    void analyzeResults();
    
    // Error handling
    void logError(const std::string& errorMessage);
    void handleCriticalError(const std::string& errorMessage);
};

// Research system factory functions
namespace research {
    // Create a research system with default configuration
    std::shared_ptr<ResearchSystem> createResearchSystem();
    
    // Create a research system with custom configuration
    std::shared_ptr<ResearchSystem> createResearchSystem(const std::string& configFile);
    
    // Create a research system with all components enabled
    std::shared_ptr<ResearchSystem> createFullResearchSystem();
    
    // Create a minimal research system (no parallel processing)
    std::shared_ptr<ResearchSystem> createMinimalResearchSystem();
}

} // namespace nlm