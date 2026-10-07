// Advanced Experiment Modes and Automation
// Automated experiment scheduling, parameter optimization, and batch processing

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <random>
#include <thread>

namespace nlm {

class ExperimentScheduler {
public:
    ExperimentScheduler();
    ~ExperimentScheduler();
    
    void loadSchedule(const std::string& filename);
    bool saveSchedule(const std::string& filename) const;
    
    void addExperiment(const std::string& name, const std::map<std::string, std::string>& parameters);
    void removeExperiment(const std::string& name);
    void runExperiment(const std::string& name);
    void runBatch(const std::vector<std::string>& experimentNames);
    
    // Scheduling features
    void setScheduleInterval(int minutes);
    void setMaxConcurrentExperiments(int maxConcurrent);
    void enableAutoScheduling(bool enable);
    
    // Experiment configuration
    void setParameterRange(const std::string& experimentName, 
                          const std::string& parameter,
                          const std::vector<std::string>& values);
    void setParameterRandomRange(const std::string& experimentName,
                                const std::string& parameter,
                                double min, double max);
    
    // Results management
    std::map<std::string, std::vector<std::string>> getExperimentResults(const std::string& experimentName) const;
    std::map<std::string, std::vector<std::string>> getAllResults() const;
    
    // Analysis
    std::string analyzeExperimentResults(const std::string& experimentName) const;
    std::string compareExperiments(const std::vector<std::string>& experimentNames) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class ParameterOptimizer {
public:
    ParameterOptimizer();
    ~ParameterOptimizer();
    
    void attach(std::shared_ptr<class Brain> brain);
    void setObjectiveFunction(std::function<double(const std::map<std::string, double>&)> objective);
    
    // Optimization algorithms
    std::map<std::string, double> optimizeGrid(const std::map<std::string, std::vector<double>>& parameterGrid);
    std::map<std::string, double> optimizeRandom(const std::map<std::string, std::pair<double, double>>& parameterRanges,
                                                 int iterations = 1000);
    std::map<std::string, double> optimizeGenetic(const std::map<std::string, std::pair<double, double>>& parameterRanges,
                                                  int populationSize = 50, int generations = 100);
    std::map<std::string, double> optimizeBayesian(const std::map<std::string, std::pair<double, double>>& parameterRanges,
                                                   int iterations = 200);
    
    // Convergence and quality
    bool checkConvergence(const std::vector<double>& history, double tolerance = 1e-6) const;
    double calculateFitness(const std::map<std::string, double>& parameters) const;
    std::string getOptimizationResult(const std::map<std::string, double>& parameters) const;
    
    // Optimization management
    void resetOptimization();
    void setOptimizationBounds(const std::map<std::string, std::pair<double, double>>& bounds);
    void setOptimizationConstraints(const std::vector<std::function<bool(const std::map<std::string, double>&)>>& constraints);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class AutomatedExperimentRunner {
public:
    AutomatedExperimentRunner();
    ~AutomatedExperimentRunner();
    
    void attach(std::shared_ptr<class Brain> brain);
    void attach(std::shared_ptr<ExperimentScheduler> scheduler);
    void attach(std::shared_ptr<ParameterOptimizer> optimizer);
    
    // Experiment automation
    void runAutomatedExperiment(const std::string& experimentName,
                               const std::map<std::string, double>& parameters);
    void runParameterSearch(const std::string& experimentName,
                           const std::map<std::string, std::pair<double, double>>& parameterRanges,
                           int iterations = 100);
    void runEvolutionaryExperiment(const std::string& experimentName,
                                  const std::map<std::string, std::pair<double, double>>& parameterRanges,
                                  int generations = 50);
    
    // Continuous learning
    void enableContinuousLearning(bool enable);
    void setLearningInterval(int steps);
    void updateParametersBasedOnResults(const std::map<std::string, double>& results);
    
    // Quality control
    bool validateParameters(const std::map<std::string, double>& parameters) const;
    std::vector<std::string> getParameterValidationErrors(const std::map<std::string, double>& parameters) const;
    
    // Results analysis
    std::string analyzeExperimentalResults(const std::string& experimentName) const;
    std::string generateExperimentalReport(const std::string& experimentName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class ExperimentBatchProcessor {
public:
    ExperimentBatchProcessor();
    ~ExperimentBatchProcessor();
    
    void attach(std::shared_ptr<class Brain> brain);
    
    // Batch processing
    void processBatch(const std::vector<std::map<std::string, double>>& parameterSets,
                     const std::string& outputDirectory = "batch_results");
    void processBatchWithParallelism(const std::vector<std::map<std::string, double>>& parameterSets,
                                    int maxThreads = std::thread::hardware_concurrency());
    
    // Batch management
    void setBatchSize(int size);
    void setBatchOutputFormat(const std::string& format);
    void enableBatchResume(bool enable);
    void enableBatchValidation(bool enable);
    
    // Progress tracking
    std::vector<std::string> getBatchStatus() const;
    double getBatchProgress() const;
    std::string getCurrentBatchTask() const;
    
    // Results aggregation
    std::map<std::string, double> aggregateResults(const std::vector<std::map<std::string, double>>& individualResults) const;
    std::string generateBatchReport(const std::string& outputDirectory) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced experiment configurations
struct AdvancedExperimentConfig {
    std::string experimentName;
    std::map<std::string, double> parameters;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point scheduledTime;
    int priority;
    std::string experimentType; // "basic", "plasticity", "stdp", "custom"
    
    // Optimization settings
    std::string optimizationAlgorithm; // "grid", "random", "genetic", "bayesian"
    int optimizationIterations;
    double optimizationTolerance;
    
    // Quality settings
    double qualityThreshold;
    int maxRuntimeSeconds;
    int maxMemoryMB;
    
    // Output settings
    bool saveIntermediateResults;
    bool generateDetailedReport;
    std::vector<std::string> outputFormats;
};

// Experiment automation utilities
namespace ExperimentUtils {
    // Parameter generation
    std::map<std::string, double> generateRandomParameters(const std::map<std::string, std::pair<double, double>>& ranges);
    std::map<std::string, double> generateLogarithmicParameters(const std::map<std::string, std::pair<double, double>>& ranges);
    std::map<std::string, double> generateLinearlySpacedParameters(const std::map<std::string, std::pair<double, double>>& ranges, int count);
    
    // Result analysis
    double calculateExperimentSuccess(const std::map<std::string, double>& results);
    std::string interpretResults(const std::map<std::string, double>& results, const std::string& experimentType);
    std::string getExperimentRecommendation(const std::map<std::string, double>& results);
    
    // Output formatting
    std::string formatExperimentParameters(const std::map<std::string, double>& parameters);
    std::string formatExperimentResults(const std::map<std::string, double>& results);
    std::string generateExperimentReport(const AdvancedExperimentConfig& config, 
                                        const std::map<std::string, double>& results);
    
    // Validation
    bool validateExperimentConfig(const AdvancedExperimentConfig& config);
    std::string getExperimentValidationError(const AdvancedExperimentConfig& config);
}

} // namespace nlm
