// Advanced Experiment Configuration System
#pragma once

#include "experiments/Experiment.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm {

// Parameter definition for advanced configuration
struct Parameter {
    std::string name;
    std::string type; // "int", "float", "double", "string", "bool", "enum"
    std::string description;
    
    // For numeric types
    double minValue = 0.0;
    double maxValue = 1.0;
    double defaultValue = 0.0;
    double stepSize = 1.0;
    
    // For string types
    std::vector<std::string> stringOptions;
    
    // For enum types
    std::vector<std::string> enumOptions;
    
    // Validation function
    std::function<bool(double)> validationFunc;
    
    Parameter(const std::string& name, const std::string& type,
              const std::string& description = "",
              double min = 0.0, double max = 1.0, double defaultVal = 0.0,
              double step = 1.0)
        : name(name), type(type), description(description),
          minValue(min), maxValue(max), defaultValue(defaultVal), stepSize(step) {}
};

// Experiment parameter configuration
struct ExperimentConfig {
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    std::string creationDate;
    
    // Parameter definitions
    std::vector<Parameter> parameters;
    
    // Current parameter values
    std::unordered_map<std::string, double> currentValues;
    std::unordered_map<std::string, std::string> currentStrings;
    
    // Parameter ranges for sweeps
    struct ParameterRange {
        std::string paramName;
        std::vector<double> values;
        bool useLogScale = false;
    };
    std::vector<ParameterRange> parameterRanges;
    
    // Constraints and dependencies
    std::vector<std::function<bool(const std::unordered_map<std::string, double>&)>> constraints;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    
    // Template parameters
    bool isTemplate = false;
    std::string templateType; // "grid_search", "random", "bayesian", "adaptive"
    
    // Experiment metadata
    std::unordered_map<std::string, std::string> metadata;
    
    ExperimentConfig(const std::string& name = "New Experiment")
        : name(name), version("1.0"), isTemplate(false) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        creationDate = std::ctime(&time);
        creationDate.erase(creationDate.find_last_not_of("\n\r\t \v") + 1);
    }
    
    // Add a parameter to the configuration
    void addParameter(const Parameter& param) {
        parameters.push_back(param);
        // Set default value
        if (param.type == "int" || param.type == "float" || param.type == "double") {
            currentValues[param.name] = param.defaultValue;
        } else if (param.type == "string") {
            currentStrings[param.name] = "";
        } else if (param.type == "bool") {
            currentValues[param.name] = param.defaultValue > 0.5 ? 1.0 : 0.0;
        }
    }
    
    // Set parameter value with validation
    bool setParameterValue(const std::string& name, double value) {
        auto it = std::find_if(parameters.begin(), parameters.end(),
                              [&name](const Parameter& p) { return p.name == name; });
        if (it == parameters.end()) return false;
        
        if (it->validationFunc && !it->validationFunc(value)) return false;
        if (value < it->minValue || value > it->maxValue) return false;
        
        currentValues[name] = value;
        return true;
    }
    
    // Get parameter value
    double getParameterValue(const std::string& name, double defaultVal = 0.0) const {
        auto it = currentValues.find(name);
        if (it != currentValues.end()) return it->second;
        return defaultVal;
    }
    
    // Get all current parameters as map
    std::unordered_map<std::string, double> getAllParameterValues() const {
        return currentValues;
    }
    
    // Validate current configuration
    bool validateConfiguration() const {
        for (const auto& constraint : constraints) {
            if (!constraint(currentValues)) return false;
        }
        return true;
    }
    
    // Generate parameter combinations for grid search
    std::vector<std::unordered_map<std::string, double>> generateGridCombinations() const {
        std::vector<std::unordered_map<std::string, double>> combinations;
        std::vector<ParameterRange> activeRanges;
        
        for (const auto& range : parameterRanges) {
            if (std::find_if(parameters.begin(), parameters.end(),
                            [&range](const Parameter& p) { return p.name == range.paramName; }) != parameters.end()) {
                activeRanges.push_back(range);
            }
        }
        
        if (activeRanges.empty()) {
            // Use current values if no ranges specified
            combinations.push_back(currentValues);
            return combinations;
        }
        
        // Generate all combinations
        std::vector<std::vector<double>> rangeValues;
        std::vector<std::string> paramNames;
        
        for (const auto& range : activeRanges) {
            rangeValues.push_back(range.values);
            paramNames.push_back(range.paramName);
        }
        
        // Simple recursive combination generation
        std::vector<std::vector<double>> currentCombinations;
        for (size_t i = 0; i < rangeValues.size(); ++i) {
            if (currentCombinations.empty()) {
                for (double val : rangeValues[i]) {
                    currentCombinations.push_back(std::vector<double>(1, val));
                }
            } else {
                std::vector<std::vector<double>> newCombinations;
                for (const auto& combo : currentCombinations) {
                    for (double val : rangeValues[i]) {
                        auto newCombo = combo;
                        newCombo.push_back(val);
                        newCombinations.push_back(newCombo);
                    }
                }
                currentCombinations = newCombinations;
            }
        }
        
        // Convert combinations to parameter maps
        for (const auto& combo : currentCombinations) {
            std::unordered_map<std::string, double> paramMap;
            for (size_t i = 0; i < paramNames.size(); ++i) {
                paramMap[paramNames[i]] = combo[i];
            }
            
            // Include non-swept parameters
            for (const auto& [name, value] : currentValues) {
                if (std::find(paramNames.begin(), paramNames.end(), name) == paramNames.end()) {
                    paramMap[name] = value;
                }
            }
            
            combinations.push_back(paramMap);
        }
        
        return combinations;
    }
    
    // Generate random parameter combinations
    std::vector<std::unordered_map<std::string, double>> generateRandomCombinations(size_t count) const {
        std::vector<std::unordered_map<std::string, double>> combinations;
        std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
        
        for (size_t i = 0; i < count; ++i) {
            std::unordered_map<std::string, double> paramMap;
            
            for (const auto& param : parameters) {
                if (param.type == "int" || param.type == "float" || param.type == "double") {
                    std::uniform_real_distribution<double> dist(param.minValue, param.maxValue);
                    paramMap[param.name] = dist(rng);
                } else if (param.type == "bool") {
                    std::uniform_real_distribution<double> dist(0.0, 1.0);
                    paramMap[param.name] = (dist(rng) > 0.5) ? 1.0 : 0.0;
                }
            }
            
            combinations.push_back(paramMap);
        }
        
        return combinations;
    }
    
    // Create a copy of the configuration
    ExperimentConfig clone() const {
        ExperimentConfig copy(name);
        copy.description = description;
        copy.author = author;
        copy.version = version;
        copy.parameters = parameters;
        copy.currentValues = currentValues;
        copy.currentStrings = currentStrings;
        copy.parameterRanges = parameterRanges;
        copy.constraints = constraints;
        copy.dependencies = dependencies;
        copy.isTemplate = isTemplate;
        copy.templateType = templateType;
        copy.metadata = metadata;
        return copy;
    }
};

// Advanced experiment class with comprehensive configuration
class AdvancedExperiment {
public:
    AdvancedExperiment();
    AdvancedExperiment(const std::string& name, const std::string& description = "");
    ~AdvancedExperiment();
    
    // Configuration management
    void setConfig(const ExperimentConfig& config);
    const ExperimentConfig& getConfig() const { return config; }
    
    // Parameter management
    void addParameter(const Parameter& param);
    bool updateParameter(const std::string& name, double value);
    double getParameter(const std::string& name) const;
    const std::vector<Parameter>& getParameters() const { return config.parameters; }
    
    // Dynamic parameter adjustment
    void registerParameterCallback(const std::string& paramName,
                                   std::function<void(double)> callback);
    void triggerParameterUpdate(const std::string& paramName, double value);
    
    // Experiment execution
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
             SimulationStep maxSteps, std::function<bool(uint64_t step, double progress)> progressCallback = nullptr);
    
    // Parameter sweep capabilities
    std::vector<std::shared_ptr<AdvancedExperiment>> generateSweepConfigurations(const std::string& sweepType,
                                                                                  size_t numCombinations = 10);
    std::vector<std::shared_ptr<AdvancedExperiment>> generateGridSearch(const std::vector<std::string>& paramNames,
                                                                        const std::vector<std::vector<double>>& paramValues);
    std::vector<std::shared_ptr<AdvancedExperiment>> generateRandomSearch(size_t numCombinations);
    
    // Experiment metadata
    void setVersion(const std::string& version) { config.version = version; }
    void setAuthor(const std::string& author) { config.author = author; }
    void setDescription(const std::string& desc) { config.description = desc; }
    std::string getAuthor() const { return config.author; }
    std::string getDescription() const { return config.description; }
    std::string getVersion() const { return config.version; }
    
    // Checkpoint and recovery
    bool saveCheckpoint(const std::string& filepath) const;
    bool loadCheckpoint(const std::string& filepath);
    
    // Configuration templates
    static ExperimentConfig createTemplate(const std::string& type,
                                           const std::unordered_map<std::string, double>& baseValues);
    static std::vector<ExperimentConfig> getAvailableTemplates();
    
    // Comparison with other experiments
    double compareWith(const AdvancedExperiment& other) const;
    
    // Export and import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& jsonStr);
    
    // Get experiment ID
    const std::string& getId() const { return id; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string id;
    ExperimentConfig config;
    
    // Generate unique ID
    static std::string generateId();
};

// Advanced experiment runner with sophisticated execution capabilities
class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner();
    ~AdvancedExperimentRunner();
    
    // Create advanced experiment
    std::shared_ptr<AdvancedExperiment> createExperiment(const std::string& name, const std::string& description = "");
    
    // Run experiments with advanced options
    std::vector<std::shared_ptr<AdvancedExperiment>> runBatch(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
                                                               std::shared_ptr<Brain> brainTemplate,
                                                               std::shared_ptr<Environment> environmentTemplate,
                                                               SimulationStep maxSteps,
                                                               size_t maxParallel = 1,
                                                               std::function<bool(uint64_t, uint64_t)> progressCallback = nullptr);
    
    // Parameter sweep execution
    std::vector<std::shared_ptr<AdvancedExperiment>> runParameterSweep(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& baseExperiments,
        const std::string& sweepType, size_t numCombinations,
        std::shared_ptr<Brain> brainTemplate, std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Multi-seed execution for statistical significance
    std::vector<std::shared_ptr<AdvancedExperiment>> runMultiSeed(const std::shared_ptr<AdvancedExperiment>& experiment,
                                                                   size_t numSeeds,
                                                                   std::shared_ptr<Brain> brainTemplate,
                                                                   std::shared_ptr<Environment> environmentTemplate,
                                                                   SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<AdvancedExperiment>>& getExperiments() const;
    
    // Get experiment by ID
    std::shared_ptr<AdvancedExperiment> getExperiment(const std::string& id) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Configuration management
    void setGlobalParameter(const std::string& name, double value);
    double getGlobalParameter(const std::string& name) const;
    
    // Results management
    bool saveAllResults(const std::string& directory) const;
    bool loadResults(const std::string& directory);
    
    // Experiment versioning
    void createVersion(const std::string& experimentId, const std::string& versionName);
    std::vector<std::string> getExperimentVersions(const std::string& experimentId) const;
    std::shared_ptr<AdvancedExperiment> getExperimentVersion(const std::string& experimentId, const std::string& versionName) const;
    
    // Batch operations
    void deleteExperiments(const std::vector<std::string>& experimentIds);
    void exportExperiments(const std::vector<std::string>& experimentIds, const std::string& directory) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced analysis utilities
namespace AdvancedAnalysis {
    // Statistical analysis
    struct StatisticalSummary {
        double mean;
        double stdDev;
        double min;
        double max;
        double median;
        double variance;
        size_t count;
        double confidenceInterval;
    };
    
    // Multi-dimensional data analysis
    struct MultiDimAnalysis {
        std::vector<double> pcaComponents;
        std::vector<std::vector<double>> covarianceMatrix;
        std::vector<std::vector<double>> correlationMatrix;
        std::vector<double> eigenvalues;
        std::vector<double> explainedVariance;
        std::vector<std::vector<double>> principalComponents;
    };
    
    // Time-series analysis
    struct TimeSeriesAnalysis {
        std::vector<double> autocorrelation;
        std::vector<double> partialAutocorrelation;
        std::vector<double> detrendedData;
        double trend; // linear regression slope
        double seasonalityStrength;
        std::vector<double> residuals;
        double spectralDensity;
    };
    
    // Graph theory analysis
    struct GraphAnalysis {
        std::vector<std::vector<double>> adjacencyMatrix;
        std::vector<double> clusteringCoefficient;
        std::vector<double> betweennessCentrality;
        std::vector<double> closenessCentrality;
        std::vector<double> degreeCentrality;
        std::vector<double> eigenvectorCentrality;
        std::vector<size_t> connectedComponents;
        double modularity;
    };
    
    // Machine learning integration utilities
    class MLIntegration {
    public:
        MLIntegration();
        ~MLIntegration();
        
        // Pattern recognition
        std::vector<double> patternRecognition(const std::vector<std::vector<double>>& data,
                                               const std::vector<double>& labels,
                                               int numComponents = 2);
        
        // Neural network analysis
        struct NeuralNetworkAnalysis {
            std::vector<double> connectivityMatrix;
            std::vector<double> plasticityWeights;
            std::vector<double> learningRates;
            std::vector<double> stabilityMetrics;
            std::vector<double> convergenceRates;
        };
        
        NeuralNetworkAnalysis analyzeNeuralNetwork(const std::shared_ptr<Brain>& brain);
        
        // Cross-experiment comparison
        struct CrossExperimentResult {
            double similarityScore;
            std::vector<double> parameterAlignment;
            std::vector<double> performanceCorrelation;
            std::vector<std::string> matchingFeatures;
            std::vector<std::string> differingFeatures;
        };
        
        CrossExperimentResult compareExperiments(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
        
        // Bayesian optimization utilities
        struct BayesianOptimizationResult {
            std::vector<double> optimalParameters;
            double optimalValue;
            std::vector<std::pair<std::vector<double>, double>> suggestions;
            std::vector<double> acquisitionFunctionValues;
        };
        
        BayesianOptimizationResult bayesianOptimize(const std::function<double(const std::vector<double>&)>& objectiveFunction,
                                                  const std::vector<Parameter>& parameterSpace,
                                                  size_t numIterations = 50);
    };
    
    // Utility functions
    StatisticalSummary computeStatistics(const std::vector<double>& data);
    MultiDimAnalysis performPCA(const std::vector<std::vector<double>>& data, int nComponents = 2);
    TimeSeriesAnalysis analyzeTimeSeries(const std::vector<double>& timeSeries);
    GraphAnalysis analyzeGraph(const std::vector<std::vector<double>>& adjacencyMatrix);
    
    // Hypothesis testing
    double tTest(const std::vector<double>& group1, const std::vector<double>& group2);
    double anova(const std::vector<std::vector<double>>& groups);
    double correlationTest(const std::vector<double>& x, const std::vector<double>& y);
    
    // Clustering
    std::vector<size_t> kMeansClustering(const std::vector<std::vector<double>>& data, size_t k, size_t maxIterations = 100);
    std::vector<size_t> hierarchicalClustering(const std::vector<std::vector<double>>& data, double threshold = 0.5);
}

// Advanced visualization for experiment results
namespace AdvancedVisualization {
    // Custom visualization with statistical overlays
    class AdvancedVisualizer {
    public:
        AdvancedVisualizer();
        ~AdvancedVisualizer();
        
        // Configure visualization
        void configure(const std::unordered_map<std::string, bool>& options);
        
        // Multi-dimensional plotting
        void plotMultiDimensional(const std::vector<std::vector<double>>& data,
                                 const std::string& title = "",
                                 const std::vector<std::string>& axisLabels = {});
        
        // Heat maps and clustering visualizations
        void plotHeatmap(const std::vector<std::vector<double>>& data,
                         const std::string& title = "",
                         const std::string& xlabel = "",
                         const std::string& ylabel = "");
        
        // Statistical result visualization
        void plotStatisticalResults(const AdvancedAnalysis::StatisticalSummary& stats,
                                    const std::string& title = "");
        
        // Progress tracking for long-running experiments
        void plotProgress(const std::vector<std::pair<uint64_t, double>>& progressData,
                          const std::string& title = "");
        
        // Publication-quality result formatting
        std::string generatePublicationQualityPlot(const std::shared_ptr<AdvancedExperiment>& experiment);
        
        // Interactive dashboard
        void createInteractiveDashboard(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
        
        // Time-series analysis for temporal dynamics
        void plotTimeSeries(const std::vector<double>& timeSeries,
                            const std::vector<double>& timePoints,
                            const std::string& title = "");
        
        // Graph theory analysis for network properties
        void plotNetworkGraph(const AdvancedAnalysis::GraphAnalysis& graphAnalysis,
                             const std::string& title = "");
        
        // Export results
        bool exportToPDF(const std::string& filepath);
        bool exportToJSON(const std::string& filepath);
        bool exportToCSV(const std::string& filepath);
        
        // Real-time visualization
        void startRealTimeVisualization(std::function<std::vector<double>(void)> dataProvider);
        void stopRealTimeVisualization();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

// Power user features for advanced control
namespace PowerUser {
    // Low-level experimental control and hooks
    class ExperimentHookManager {
    public:
        ExperimentHookManager();
        ~ExperimentHookManager();
        
        // Register hooks for experiment lifecycle
        void registerPreRunHook(std::function<bool(std::shared_ptr<AdvancedExperiment>)> hook);
        void registerPostRunHook(std::function<void(std::shared_ptr<AdvancedExperiment>)> hook);
        void registerStepHook(std::function<void(uint64_t step, std::shared_ptr<AdvancedExperiment>)> hook);
        void registerErrorHook(std::function<void(const std::string&, std::shared_ptr<AdvancedExperiment>)> hook);
        
        // Trigger hooks
        void triggerPreRun(std::shared_ptr<AdvancedExperiment> experiment);
        void triggerPostRun(std::shared_ptr<AdvancedExperiment> experiment);
        void triggerStep(uint64_t step, std::shared_ptr<AdvancedExperiment> experiment);
        void triggerError(const std::string& error, std::shared_ptr<AdvancedExperiment> experiment);
        
        // Custom event handling
        void registerEventHandler(const std::string& eventType, std::function<void(const std::unordered_map<std::string, std::string>&)> handler);
        void triggerEvent(const std::string& eventType, const std::unordered_map<std::string, std::string>& data);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Advanced memory management options
    class AdvancedMemoryManager {
    public:
        AdvancedMemoryManager();
        ~AdvancedMemoryManager();
        
        // Memory optimization
        void enableMemoryPool(bool enable);
        void setMemoryPoolSize(size_t size);
        void enableMemoryCompression(bool enable);
        void enableSwapping(bool enable);
        
        // Checkpoint optimization
        void enableAdvancedCheckpointing(bool enable);
        void setCheckpointFrequency(uint64_t frequency);
        void setCheckpointCompressionLevel(int level);
        
        // Performance profiling
        void startProfiling();
        void stopProfiling();
        std::unordered_map<std::string, double> getProfilingResults() const;
        
        // Hardware-specific optimizations
        void setThreadCount(size_t count);
        void enableSIMD(bool enable);
        void enableOpenMP(bool enable);
        void enableGPUEnabled(bool enable);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Custom experiment execution models
    class CustomExecutionModel {
    public:
        CustomExecutionModel();
        ~CustomExecutionModel();
        
        // Define custom execution strategy
        void setExecutionStrategy(std::function<std::vector<uint64_t>(const AdvancedExperimentConfig&)> strategy);
        std::vector<uint64_t> getExecutionSchedule(const AdvancedExperimentConfig& config);
        
        // Custom checkpointing
        void setCustomCheckpointManager(std::function<bool(const std::string&, const AdvancedExperiment&)> save,
                                         std::function<bool(const std::string&, AdvancedExperiment&)> load);
        
        // Custom result collection
        void setCustomResultCollector(std::function<void(const AdvancedExperiment&, std::shared_ptr<Experiment>)> collector);
        
        // Custom error handling
        void setCustomErrorHandler(std::function<void(const std::string&, AdvancedExperiment&)> handler);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Real-time experiment streaming
    class ExperimentStreamer {
    public:
        ExperimentStreamer();
        ~ExperimentStreamer();
        
        // Start streaming
        void startStreaming(const std::string& streamId,
                            std::function<std::shared_ptr<AdvancedExperiment>(void)> experimentProvider,
                            std::function<void(const std::shared_ptr<AdvancedExperiment>&)> resultHandler);
        
        // Stop streaming
        void stopStreaming(const std::string& streamId);
        
        // Control streaming
        void pauseStreaming(const std::string& streamId);
        void resumeStreaming(const std::string& streamId);
        
        // Configuration
        void setStreamRate(size_t rate);
        void setStreamBufferSize(size_t size);
        void enableCompression(bool enable);
        
        // Event streaming
        void streamEvent(const std::string& streamId, const std::string& eventType, const std::string& eventData);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

} // namespace nlm
