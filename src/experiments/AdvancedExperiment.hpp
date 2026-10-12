#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../core/Config/AdvancedConfig.hpp"
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <set>
#include <variant>
#include <numeric>
#include <limits>
#include <fstream>
#include <iomanip>
#include <thread>
#include <future>

namespace nlm {

// Advanced experiment types
using ParameterValue = std::variant<int, double, bool, std::string>;
using ParameterMap = std::unordered_map<std::string, ParameterValue>;
using MetricType = std::variant<double, int, float>;
using MetricStats = struct {
    double mean = 0.0;
    double stddev = 0.0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    size_t count = 0;
    double median = 0.0;
    double p95 = 0.0; // 95th percentile
    double p99 = 0.0; // 99th percentile
};

// Advanced experiment parameter
struct ParameterSpec {
    std::string name;
    ParameterValue defaultValue;
    ParameterValue minValue;
    ParameterValue maxValue;
    std::string type; // "int", "double", "bool", "string"
    std::string unit;
    std::string description;
    bool logScale; // For numeric parameters
    std::vector<ParameterValue> categoricalValues; // For categorical parameters
    double stepSize; // For grid search
    bool includeInOutput;
    
    ParameterSpec() : logScale(false), stepSize(1.0), includeInOutput(true) {}
};

// Statistical analysis results
struct StatisticalAnalysis {
    std::string metricName;
    std::vector<double> values;
    MetricStats statistics;
    std::string distributionType; // "normal", "poisson", "exponential", etc.
    std::string significanceMethod; // "t-test", "chi-square", etc.
    bool isSignificant;
    double pValue;
    double effectSize;
    std::string interpretation;
    
    StatisticalAnalysis() : isSignificant(false), pValue(1.0), effectSize(0.0) {}
};

// Experiment comparison results
struct ExperimentComparison {
    std::string metricName;
    std::vector<double> experiment1Values;
    std::vector<double> experiment2Values;
    double meanDifference;
    double pooledStddev;
    double tStatistic;
    double pValue;
    double cohensD;
    std::string conclusion;
    
    ExperimentComparison() : meanDifference(0.0), pooledStddev(0.0), 
                            tStatistic(0.0), pValue(1.0), cohensD(0.0) {}
};

// Experiment template for parameterized experiments
struct ExperimentTemplate {
    std::string name;
    std::vector<ParameterSpec> parameters;
    std::vector<std::pair<std::string, MetricType>> defaultMetrics;
    std::string brainConfigTemplate;
    std::string environmentConfigTemplate;
    std::chrono::seconds maxDuration;
    SimulationStep maxSteps;
    bool autoGenerateParameterCombos;
    bool useDistributedComputing;
    
    ExperimentTemplate() : maxDuration(300), maxSteps(1000), 
                          autoGenerateParameterCombos(true), useDistributedComputing(false) {}
};

// Real-time experiment monitoring
struct ExperimentMonitor {
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastUpdate;
    SimulationStep currentStep;
    SimulationStep maxSteps;
    std::vector<double> performanceMetrics;
    std::vector<std::string> activeWarnings;
    std::vector<std::string> activeErrors;
    std::function<void(const std::string&)> progressCallback;
    std::function<void(const std::string&)> warningCallback;
    std::function<void(const std::string&)> errorCallback;
    
    ExperimentMonitor() : currentStep(0), maxSteps(0) {}
};

// Advanced experiment class with parameter sweeps, statistical analysis, and monitoring
class AdvancedExperiment {
public:
    AdvancedExperiment(const std::string& name);
    AdvancedExperiment(const ExperimentTemplate& template);
    ~AdvancedExperiment();
    
    // Basic experiment info
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Parameter management
    void addParameter(const ParameterSpec& param);
    bool hasParameter(const std::string& name) const;
    ParameterSpec getParameter(const std::string& name) const;
    const std::vector<ParameterSpec>& getParameters() const;
    
    // Parameter value setting
    void setParameter(const std::string& name, const ParameterValue& value);
    void setParameters(const ParameterMap& params);
    ParameterValue getParameter(const std::string& name) const;
    const ParameterMap& getParameters() const;
    
    // Experiment execution
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment, 
             std::shared_ptr<ExperimentMonitor> monitor = nullptr);
    bool runWithConfiguration(const AdvancedConfig& config, 
                             std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                             std::shared_ptr<ExperimentMonitor> monitor = nullptr);
    
    // Parameter sweep execution
    std::vector<std::shared_ptr<AdvancedExperiment>> runParameterSweep(
        const std::string& paramName,
        const std::vector<ParameterValue>& values,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        std::shared_ptr<ExperimentMonitor> monitor = nullptr);
    
    // Experiment comparison
    ExperimentComparison compareExperiments(
        const std::shared_ptr<AdvancedExperiment>& otherExperiment,
        const std::string& metricName) const;
    
    // Statistical analysis
    StatisticalAnalysis analyzeMetric(const std::string& metricName) const;
    std::vector<StatisticalAnalysis> analyzeAllMetrics() const;
    
    // Parameter optimization
    ParameterMap optimizeParameters(std::shared_ptr<Brain> brainTemplate,
                                     std::shared_ptr<Environment> environmentTemplate,
                                     const std::vector<std::string>& objectiveMetrics,
                                     const std::unordered_map<std::string, std::vector<ParameterValue>>& searchSpace,
                                     std::shared_ptr<ExperimentMonitor> monitor = nullptr);
    
    // Data management
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    std::optional<MetricType> getMetric(const std::string& name) const;
    const std::unordered_map<std::string, std::vector<MetricType>>& getMetrics() const;
    
    // Step data recording
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    struct StepData {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    const std::vector<StepData>& getStepData() const;
    
    // Results analysis
    double getObjectiveValue(const std::string& metricName) const;
    std::vector<double> getObjectiveVector(const std::vector<std::string>& metricNames) const;
    std::string generateReport() const;
    bool exportResults(const std::string& filepath) const;
    
    // Experiment metadata
    void setStartStep(SimulationStep step);
    void setEndStep(SimulationStep step);
    SimulationStep getStartStep() const;
    SimulationStep getEndStep() const;
    
    // Notes and tags
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    void addTag(const std::string& tag);
    const std::set<std::string>& getTags() const;
    
    // Random seed management
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration management
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Monitoring
    void setMonitor(std::shared_ptr<ExperimentMonitor> monitor);
    std::shared_ptr<ExperimentMonitor> getMonitor() const;
    
    // Comparison and cloning
    bool operator==(const AdvancedExperiment& other) const;
    bool operator!=(const AdvancedExperiment& other) const;
    
    std::shared_ptr<AdvancedExperiment> clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void updateMonitorProgress(std::shared_ptr<ExperimentMonitor> monitor, 
                              SimulationStep currentStep, SimulationStep maxSteps) const;
    void checkMonitorWarnings(std::shared_ptr<ExperimentMonitor> monitor) const;
    std::vector<ParameterValue> generateParameterCombination(
        const std::vector<ParameterSpec>& parameters) const;
    static bool isValueInRange(const ParameterValue& value, 
                             const ParameterValue& min, const ParameterValue& max);
    static double toDouble(const ParameterValue& value);
    static std::string parameterValueToString(const ParameterValue& value);
    static ParameterValue stringToParameterValue(const std::string& str, const std::string& type);
};

// Advanced experiment runner with batch processing and parallel execution
class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner();
    ~AdvancedExperimentRunner();
    
    // Experiment management
    std::shared_ptr<AdvancedExperiment> createExperiment(const std::string& name);
    std::shared_ptr<AdvancedExperiment> createExperiment(const ExperimentTemplate& template);
    
    // Batch operations
    std::vector<std::shared_ptr<AdvancedExperiment>> createBatch(
        const std::vector<std::string>& names);
    std::vector<std::shared_ptr<AdvancedExperiment>> createParameterSweep(
        const ExperimentTemplate& template,
        const std::string& paramName,
        const std::vector<ParameterValue>& values,
        size_t maxParallel = 4);
    
    // Running experiments
    bool runExperiment(std::shared_ptr<AdvancedExperiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      std::shared_ptr<ExperimentMonitor> monitor = nullptr,
                      bool useAsync = false);
    
    std::vector<std::shared_ptr<AdvancedExperiment>> runBatch(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        bool useAsync = false,
        size_t maxParallel = 4);
    
    // Experiment storage and retrieval
    void addExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    std::shared_ptr<AdvancedExperiment> getExperiment(const std::string& name) const;
    const std::vector<std::shared_ptr<AdvancedExperiment>>& getExperiments() const;
    
    // Batch results
    struct BatchResults {
        std::vector<std::shared_ptr<AdvancedExperiment>> successful;
        std::vector<std::shared_ptr<AdvancedExperiment>> failed;
        std::unordered_map<std::string, std::string> errors;
        double totalObjectiveValue;
        size_t totalStepsCompleted;
    };
    
    BatchResults runBatchWithResults(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        bool useAsync = false,
        size_t maxParallel = 4);
    
    // Experiment comparison
    std::vector<ExperimentComparison> compareExperiments(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
        const std::string& metricName) const;
    
    // Results aggregation
    std::vector<double> aggregateMetric(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
        const std::string& metricName,
        const std::string& aggregationMethod = "mean") const;
    
    // Results storage
    bool saveResults(const std::string& filepath) const;
    bool loadResults(const std::string& filepath);
    
    // Configuration management
    void setGlobalSeed(uint64_t seed);
    uint64_t getGlobalSeed() const;
    
    // Template management
    void addTemplate(const ExperimentTemplate& template);
    ExperimentTemplate getTemplate(const std::string& name) const;
    const std::unordered_map<std::string, ExperimentTemplate>& getTemplates() const;
    
    // Statistics and monitoring
    struct RunnerStats {
        size_t totalExperimentsRun;
        size_t successfulExperiments;
        size_t failedExperiments;
        double averageObjectiveValue;
        std::chrono::system_clock::time_point lastRunTime;
        std::chrono::seconds totalRunTime;
    };
    RunnerStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread pool management
    void initializeThreadPool(size_t threadCount);
    void shutdownThreadPool();
};

// Utility functions for advanced experiments
namespace ExperimentUtils {
    // Parameter validation
    bool validateParameterRange(const ParameterSpec& param, const ParameterValue& value);
    bool validateParameterValue(const ParameterSpec& param, const ParameterValue& value);
    
    // Statistical functions
    double calculateMean(const std::vector<double>& values);
    double calculateStddev(const std::vector<double>& values, double mean);
    double calculatePercentile(const std::vector<double>& values, double percentile);
    double calculateTTest(const std::vector<double>& sample1, const std::vector<double>& sample2);
    
    // Parameter combination generation
    std::vector<ParameterMap> generateGrid(const std::vector<ParameterSpec>& parameters);
    std::vector<ParameterMap> generateRandom(const std::vector<ParameterSpec>& parameters, 
                                            size_t numSamples, uint64_t seed = 42);
    
    // File I/O
    bool saveExperimentResults(const AdvancedExperiment& experiment, const std::string& filepath);
    bool loadExperimentResults(AdvancedExperiment& experiment, const std::string& filepath);
    
    // Experiment comparison
    std::string compareExperiments(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
}

} // namespace nlm
