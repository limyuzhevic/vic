#pragma once

/**
 * Phase 6 Diagnostic System
 * 
 * Advanced diagnostic and analysis system for NLM Phase 6 integration
 * Provides comprehensive system analysis, validation, and debugging capabilities
 */

#include "../experiments/Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>

namespace nlm {

/**
 * Diagnostic result structure containing all analysis data
 */
struct Phase6DiagnosticResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool memoryAssociativeMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool cognitionIntegrated;
    bool developmentIntegrated;
    
    // Memory system metrics
    size_t workingMemoryActiveTraces;
    size_t workingMemoryCapacity;
    size_t episodicMemoryEpisodes;
    size_t episodicMemoryMaxEpisodes;
    size_t associativeMemoryAssociations;
    
    // Prediction system metrics
    float predictionError;
    float predictionConfidence;
    
    // Neuromodulation metrics
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    // Performance metrics
    float totalReward;
    float averageFiringRate;
    size_t totalSpikeCount;
    size_t activeNeuronCount;
    size_t stepsPerSecond;
    
    // Development metrics
    DevelopmentalStage developmentStage;
    float developmentStagePlasticity;
    float developmentAgePlasticityFactor;
    size_t developmentSynaptogenesisEvents;
    size_t developmentPruningEvents;
    
    Phase6DiagnosticResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , memoryAssociativeMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , cognitionIntegrated(false)
        , developmentIntegrated(false)
        , workingMemoryActiveTraces(0)
        , workingMemoryCapacity(0)
        , episodicMemoryEpisodes(0)
        , episodicMemoryMaxEpisodes(0)
        , associativeMemoryAssociations(0)
        , predictionError(0.0f)
        , predictionConfidence(0.5f)
        , dopamineLevel(0.0f)
        , curiosityLevel(0.0f)
        , noveltyLevel(0.0f)
        , totalReward(0.0f)
        , averageFiringRate(0.0f)
        , totalSpikeCount(0)
        , activeNeuronCount(0)
        , stepsPerSecond(0)
        , developmentStage(DevelopmentalStage::Initial)
        , developmentStagePlasticity(1.0f)
        , developmentAgePlasticityFactor(1.0f)
        , developmentSynaptogenesisEvents(0)
        , developmentPruningEvents(0) {}
};

/**
 * Export format enumeration
 */
enum class ExportFormat {
    TEXT,
    JSON,
    CSV
};

/**
 * Advanced configuration for diagnostic system
 */
struct AdvancedConfig {
    bool enableDetailedLogging;
    bool enablePerformanceMetrics;
    bool enableMemoryAnalysis;
    bool enablePredictionAnalysis;
    bool enableNeuromodulationAnalysis;
    bool enableDevelopmentAnalysis;
    bool enableTimeSeriesAnalysis;
    std::vector<ExportFormat> exportFormats;
    size_t sampleInterval;
    size_t maxTimeSeriesPoints;
    float validationThreshold;
    
    AdvancedConfig()
        : enableDetailedLogging(true)
        , enablePerformanceMetrics(true)
        , enableMemoryAnalysis(true)
        , enablePredictionAnalysis(true)
        , enableNeuromodulationAnalysis(true)
        , enableDevelopmentAnalysis(true)
        , enableTimeSeriesAnalysis(true)
        , exportFormats({ExportFormat::JSON, ExportFormat::CSV, ExportFormat::TEXT})
        , sampleInterval(100)
        , maxTimeSeriesPoints(10000)
        , validationThreshold(0.8f) {}
};

/**
 * Phase 6 Diagnostic System
 * 
 * Comprehensive diagnostic and analysis system for NLM Phase 6 integration
 * Provides system analysis, validation, debugging, and reporting capabilities
 */
class Phase6Diagnostic {
public:
    Phase6Diagnostic();
    ~Phase6Diagnostic();
    
    /**
     * Analyze Phase 6 integration experiment
     * 
     * @param experiment The Phase 6 integration experiment to analyze
     * @param config Configuration for the analysis
     * @return Diagnostic result containing analysis data
     */
    Phase6DiagnosticResult analyze(const Phase6IntegratedExperiment& experiment,
                                 const Phase6Config& config);
    
    /**
     * Generate diagnostic report
     * 
     * @param result The diagnostic result to report on
     * @return Formatted diagnostic report as string
     */
    std::string generateReport(const Phase6DiagnosticResult& result);
    
    /**
     * Validate Phase 6 integration
     * 
     * @param experiment The Phase 6 integration experiment to validate
     * @param config Configuration for validation
     * @param validationMessage Output message with validation results
     * @return True if integration is valid, false otherwise
     */
    bool validateIntegration(const Phase6IntegratedExperiment& experiment,
                           const Phase6Config& config,
                           std::string& validationMessage);
    
    /**
     * Log diagnostic results to file
     * 
     * @param report The diagnostic report to log
     * @param logFile Path to log file
     */
    void logDiagnosticResults(const std::string& report, const std::string& logFile);
    
    /**
     * Export diagnostic data in specified format
     * 
     * @param result Diagnostic result to export
     * @param filepath Output file path
     * @param format Export format (JSON, CSV, or TEXT)
     * @return Diagnostic result (for chaining)
     */
    Phase6DiagnosticResult exportDiagnosticData(const Phase6DiagnosticResult& result,
                                             const std::string& filepath,
                                             ExportFormat format);
    
    /**
     * Collect metrics from brain system
     * 
     * @param brain Brain system to analyze
     * @return Diagnostic result with metrics collected
     */
    Phase6DiagnosticResult collectMetrics(std::shared_ptr<Brain> brain);
    
    /**
     * Analyze time series of diagnostic results
     * 
     * @param results Time series of diagnostic results
     * @return Aggregated diagnostic result with trend analysis
     */
    Phase6DiagnosticResult analyzeTimeSeries(const std::vector<Phase6DiagnosticResult>& results);
    
    /**
     * Collect time series data from experiment
     * 
     * @param experiment The experiment to run
     * @param config Experiment configuration
     * @param durationSteps Total steps to simulate
     * @param sampleInterval Sampling interval for data collection
     * @return Vector of diagnostic results over time
     */
    std::vector<Phase6DiagnosticResult> collectTimeSeriesData(
        const Phase6IntegratedExperiment& experiment,
        const Phase6Config& config,
        size_t durationSteps,
        size_t sampleInterval);
    
    /**
     * Calculate trend from time series data
     * 
     * @param results Time series diagnostic results
     * @return Diagnostic result with trend calculations
     */
    Phase6DiagnosticResult calculateTrend(const std::vector<Phase6DiagnosticResult>& results);
    
    /**
     * Add diagnostic commands to Command Line Interface
     * 
     * @param cli Command Line Interface instance
     */
    void addToCommandLineInterface(class CLI& cli);
    
    /**
     * Configure advanced diagnostic options
     * 
     * @param config Advanced configuration to apply
     */
    void configureAdvancedOptions(AdvancedConfig& config);
    
    /**
     * Analyze memory retention
     * 
     * @param result Diagnostic result to analyze
     * @param analysisMessage Output analysis message
     */
    void analyzeMemoryRetention(const Phase6DiagnosticResult& result,
                               std::string& analysisMessage);
    
    /**
     * Analyze prediction accuracy
     * 
     * @param result Diagnostic result to analyze
     * @param analysisMessage Output analysis message
     */
    void analyzePredictionAccuracy(const Phase6DiagnosticResult& result,
                                  std::string& analysisMessage);
    
    /**
     * Analyze continual learning capabilities
     * 
     * @param result Diagnostic result to analyze
     * @param analysisMessage Output analysis message
     */
    void analyzeContinualLearning(const Phase6DiagnosticResult& result,
                                 std::string& analysisMessage);
    
    /**
     * Analyze multi-system interaction
     * 
     * @param result Diagnostic result to analyze
     * @param analysisMessage Output analysis message
     */
    void analyzeMultiSystemInteraction(const Phase6DiagnosticResult& result,
                                      std::string& analysisMessage);
    
    /**
     * Analyze developmental progression
     * 
     * @param result Diagnostic result to analyze
     * @param analysisMessage Output analysis message
     */
    void analyzeDevelopmentalProgression(const Phase6DiagnosticResult& result,
                                         std::string& analysisMessage);
    
    /**
     * Perform advanced debugging analysis
     * 
     * @param brain Brain system to debug
     * @param result Diagnostic result from analysis
     * @param analysisMessage Output debugging analysis
     */
    void analyzeAdvancedDebugging(std::shared_ptr<Brain> brain,
                                 const Phase6DiagnosticResult& result,
                                 std::string& analysisMessage);
    
    /**
     * Generate performance report
     * 
     * @param result Diagnostic result to report on
     * @param reportMessage Output performance report
     */
    void generatePerformanceReport(const Phase6DiagnosticResult& result,
                                  std::string& reportMessage);
    
    /**
     * Export diagnostic data with chaining
     * 
     * @param results Vector of diagnostic results to export
     * @param filepath Output file path
     * @param format Export format
     * @return True if export successful
     */
    bool exportDiagnosticData(const std::vector<Phase6DiagnosticResult>& results,
                            const std::string& filepath,
                            ExportFormat format);
    
    /**
     * Export diagnostic data (chaining)
     * 
     * @param result Diagnostic result to export
     * @param filepath Output file path
     * @param format Export format
     * @return The same diagnostic result (for chaining)
     */
    Phase6DiagnosticResult exportDiagnosticData(const Phase6DiagnosticResult& result,
                                             const std::string& filepath,
                                             ExportFormat format);
    
private:
    AdvancedConfig config_;
    
    // Helper functions for report generation
    void printDiagnosticHeader(std::stringstream& ss, const std::string& title, int width);
    void printSystemStatus(std::stringstream& ss, const std::string& name, bool status, const std::string& details = "");
    void printSectionHeader(std::stringstream& ss, const std::string& title);
    void printMetric(std::stringstream& ss, const std::string& label, float value, const std::string& unit = "");
    void printMemoryMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result);
    void printPredictionMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result);
    void printNeuromodulationMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result);
    void printPerformanceMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result);
    void printDevelopmentMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result);
    void printValidationSummary(std::stringstream& ss, const Phase6DiagnosticResult& result);
};

} // namespace nlm