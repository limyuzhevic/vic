#pragma once

#include "../experiments/ExperimentRunner.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Advanced experiment management and comparison tools
class AdvancedExperimentManager {
public:
    AdvancedExperimentManager();
    ~AdvancedExperimentManager();
    
    // Run multiple experiments with different parameters
    std::vector<Phase6IntegrationResult> runExperimentSet(
        const std::vector<Phase6Config>& configs,
        const std::string& experimentPrefix = "exp");
    
    // Compare experiment results
    json compareExperiments(const std::vector<Phase6IntegrationResult>& results);
    
    // Find optimal parameters based on performance
    Phase6Config findOptimalParameters(const std::vector<Phase6Config>& configs,
                                       const std::vector<Phase6IntegrationResult>& results);
    
    // Export results in multiple formats
    bool exportResults(const std::vector<Phase6IntegrationResult>& results,
                      const std::string& directory,
                      const std::vector<std::string>& formats = {"json", "csv", "markdown"});
    
    // Create experiment report with analysis
    std::string generateReport(const std::vector<Phase6IntegrationResult>& results,
                               const std::string& title = "NLM Experiment Analysis");
    
    // Statistical analysis of results
    json performStatisticalAnalysis(const std::vector<Phase6IntegrationResult>& results);
    
    // Parameter sensitivity analysis
    json analyzeParameterSensitivity(const std::vector<Phase6Config>& configs,
                                    const std::vector<Phase6IntegrationResult>& results,
                                    const std::string& parameter);
    
    // Get experiment statistics
    struct ExperimentStats {
        double meanTotalReward;
        double stdDevTotalReward;
        double meanFiringRate;
        double stdDevFiringRate;
        double meanWallClockTime;
        std::vector<float> firingRates;
        std::vector<float> totalRewards;
        std::vector<double> wallClockTimes;
    };
    
    ExperimentStats calculateStats(const std::vector<Phase6IntegrationResult>& results);
    
    // Save experiment configurations
    bool saveConfigurations(const std::vector<Phase6Config>& configs, const std::string& filename);
    bool loadConfigurations(const std::string& filename, std::vector<Phase6Config>& configs);
    
    // Create experiment grid for systematic parameter search
    std::vector<Phase6Config> createParameterGrid(const std::map<std::string, std::vector<double>>& parameterRanges);
    
    // Batch experiment execution
    void runBatchExperiments(const std::vector<Phase6Config>& configs,
                            const std::string& outputDir,
                            bool saveResults = true);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced visualization system for research
class ResearchVisualization {
public:
    ResearchVisualization();
    ~ResearchVisualization();
    
    // Initialize with data from multiple experiments
    void initialize(const std::vector<Phase6IntegrationResult>& results);
    
    // Generate comparative plots
    bool generateComparativePlots(const std::string& outputPrefix, const std::set<std::string>& metrics = {});
    
    // Create learning curves
    bool generateLearningCurves(const std::string& outputPrefix);
    
    // Generate heat maps for parameter effects
    bool generateParameterHeatmap(const json& sensitivityData, const std::string& outputFile);
    
    // Create network visualization for best result
    bool createNetworkVisualization(const std::string& resultFile, const std::string& outputFile);
    
    // Generate statistical visualization
    bool generateStatisticalPlots(const std::string& outputPrefix);
    
    // Create experiment timeline visualization
    bool generateExperimentTimeline(const std::vector<Phase6IntegrationResult>& results, const std::string& outputFile);
    
    // Export visualization data
    json exportVisualizationData() const;
    
    // Set visualization options
    void setVisualizationOptions(const json& options);
    
    // Get current visualization data
    json getCurrentData() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced metrics collection and analysis
class AdvancedMetrics {
public:
    AdvancedMetrics();
    ~AdvancedMetrics();
    
    // Collect metrics during simulation
    void collectMetrics(const std::string& experimentName, const Phase6IntegrationResult& result);
    
    // Calculate advanced statistics
    json calculateAdvancedStats(const std::string& experimentName) const;
    
    // Perform time-series analysis
    json analyzeTimeSeries(const std::string& experimentName, const std::string& metric) const;
    
    // Calculate learning progress
    json calculateLearningProgress(const std::string& experimentName) const;
    
    // Compute correlation between metrics
    json computeMetricCorrelations(const std::string& experimentName) const;
    
    // Generate performance scores
    json generatePerformanceScore(const std::string& experimentName, const std::vector<std::string>& targetMetrics) const;
    
    // Get best performing experiments
    std::vector<std::string> getBestExperiments(const std::string& metric, size_t topN = 5) const;
    
    // Export metrics
    bool exportMetrics(const std::string& filename, const std::string& format = "json") const;
    
    // Import metrics
    bool importMetrics(const std::string& filename);
    
    // Clean old metrics
    void cleanupOldMetrics(double maxAgeHours = 24.0);
    
    // Set analysis parameters
    void setAnalysisParameters(const json& params);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment parameter optimization
class ParameterOptimizer {
public:
    ParameterOptimizer();
    ~ParameterOptimizer();
    
    // Run evolutionary algorithm for parameter optimization
    Phase6Config optimizeParameters(const std::vector<Phase6Config>& initialParams,
                                    const std::function<double(const Phase6Config&)>& fitnessFunction,
                                    size_t populationSize = 10,
                                    size_t generations = 50);
    
    // Run grid search
    json gridSearch(const std::map<std::string, std::vector<double>>& parameterRanges,
                   const std::function<double(const Phase6Config&)>& fitnessFunction);
    
    // Run random search
    Phase6Config randomSearch(const std::map<std::string, std::vector<double>>& parameterRanges,
                             const std::function<double(const Phase6Config&)>& fitnessFunction,
                             size_t iterations = 100);
    
    // Run Bayesian optimization
    Phase6Config bayesianOptimization(const std::map<std::string, std::vector<double>>& parameterRanges,
                                     const std::function<double(const Phase6Config&)>& fitnessFunction,
                                     size_t iterations = 50);
    
    // Save optimization results
    bool saveOptimizationResults(const std::string& filename);
    
    // Load optimization results
    bool loadOptimizationResults(const std::string& filename);
    
    // Get optimization history
    json getOptimizationHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main research toolkit that integrates all advanced features
class ResearchToolkit {
public:
    ResearchToolkit();
    ~ResearchToolkit();
    
    // Initialize toolkit
    bool initialize();
    
    // Run comprehensive research pipeline
    bool runResearchPipeline(const std::vector<Phase6Config>& configs,
                            const std::string& outputDir = "research_results");
    
    // Generate complete research report
    std::string generateResearchReport(const std::string& title = "NLM Research Analysis");
    
    // Export all research data
    bool exportAllData(const std::string& outputDir);
    
    // Get current results
    json getCurrentResults() const;
    
    // Set research configuration
    void setResearchConfig(const json& config);
    
    // Get research configuration
    json getResearchConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
