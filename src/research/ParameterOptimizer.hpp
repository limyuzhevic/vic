#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <random>
#include <functional>
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/Experiment.hpp"
#include "../debug/VisualizationInterface.hpp"
#include "../debug/BrainInspector.hpp"
#include "../debug/MemoryProfiler.hpp"

namespace nlm {

class ParameterOptimizer {
public:
    ParameterOptimizer();
    ~ParameterOptimizer();
    
    // Optimization algorithms
    enum class Algorithm {
        RandomSearch,
        GridSearch,
        BayesianOptimization,
        GeneticAlgorithm,
        SimulatedAnnealing,
        ParticleSwarm
    };
    
    // Parameter definition for optimization
    struct Parameter {
        std::string name;
        double minValue;
        double maxValue;
        double currentValue;
        std::string distribution; // "uniform", "normal", "log-normal", etc.
        bool isDiscrete;
        double stepSize;
        
        Parameter(const std::string& n, double min, double max, bool discrete = false)
            : name(n), minValue(min), maxValue(max), currentValue(min), 
              distribution("uniform"), isDiscrete(discrete), stepSize(0.0) {}
    };
    
    // Optimization result
    struct OptimizationResult {
        std::vector<Parameter> parameters;
        double score;
        size_t experimentId;
        std::string experimentName;
        double executionTime;
        std::map<std::string, double> metricValues;
    };
    
    // Initialize optimizer
    bool initialize(Algorithm algorithm = Algorithm::BayesianOptimization,
                   size_t populationSize = 50,
                   size_t maxGenerations = 100,
                   double convergenceThreshold = 0.001);
    
    // Parameter management
    void addParameter(const Parameter& param);
    void setParameterValue(const std::string& name, double value);
    double getParameterValue(const std::string& name) const;
    const std::vector<Parameter>& getParameters() const;
    void clearParameters();
    
    // Objective function for optimization
    using ObjectiveFunction = std::function<double(const std::vector<Parameter>&)>;
    void setObjectiveFunction(ObjectiveFunction func);
    
    // Run optimization
    std::vector<OptimizationResult> runOptimization(
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        size_t numRuns = 1);
    
    // Individual optimization runs
    OptimizationResult optimizeSingleRun(
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        uint64_t seed = 42);
    
    // Get best results
    OptimizationResult getBestResult() const;
    std::vector<OptimizationResult> getTopResults(size_t count) const;
    
    // Analysis and reporting
    bool exportOptimizationResults(const std::string& filename) const;
    bool generateConvergenceReport(const std::string& filename) const;
    bool generateParameterSensitivityReport(const std::string& filename) const;
    
    // Configuration
    void setPopulationSize(size_t size);
    void setMaxGenerations(size_t generations);
    void setConvergenceThreshold(double threshold);
    void setMutationRate(double rate);
    void setCrossoverRate(double rate);
    void enableEarlyStopping(bool enable, double improvementThreshold = 0.01);
    
    // Progress tracking
    double getOptimizationProgress() const;
    std::string getCurrentOptimizationStatus() const;
    size_t getCurrentGeneration() const;
    size_t getTotalGenerations() const;
    
    // Progress callbacks
    using OptimizationProgressCallback = std::function<void(double progress, const std::string& status, size_t generation)>;
    void setProgressCallback(OptimizationProgressCallback callback);
    
    // Visualization integration
    void enableVisualization(bool enable);
    void setVisualizationInterface(VisualizationInterface* viz);
    
    // Analysis tools
    struct ParameterAnalysis {
        std::string parameterName;
        double mean;
        double stddev;
        double min;
        double max;
        double correlationWithScore;
        std::vector<double> values;
        std::vector<double> scores;
    };
    
    std::vector<ParameterAnalysis> analyzeParameterSensitivity(
        const std::vector<OptimizationResult>& results) const;
    
    double calculateParameterImportance(const ParameterAnalysis& analysis) const;
    
    // Advanced features
    void enableParallelOptimization(bool enable);
    void enableHyperbandOptimization(bool enable);
    void enableAsyncEvaluation(bool enable);
    
    // Results validation
    bool validateResults(const std::vector<OptimizationResult>& results) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
