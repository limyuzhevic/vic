#pragma once

#include "BatchUtils.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <future>

namespace nlm {

// Batch configuration generator for parameter sweeps
class BatchConfigGenerator {
public:
    struct ParameterRange {
        double min;
        double max;
        std::string type; // "uniform", "logarithmic", "exponential"
        std::vector<double> customValues;
        
        ParameterRange(double minVal, double maxVal, const std::string& t = "uniform")
            : min(minVal), max(maxVal), type(t) {}
        
        std::vector<double> generateValues(size_t count) const;
    };
    
    struct SweepConfig {
        std::string paramName;
        ParameterRange range;
        size_t iterations;
        bool logarithmicSpacing{false};
    };
    
    // Generate parameter grid from multiple parameter configurations
    static std::vector<std::map<std::string, double>> generateParameterGrid(
        const std::vector<SweepConfig>& sweeps);
    
    // Generate cross-validation folds
    static std::vector<std::vector<size_t>> generateCrossValidationFolds(
        size_t totalSamples, size_t numFolds, bool shuffle = true);
    
    // Create batch configuration from parameter grid
    static std::vector<std::shared_ptr<Config>> createBatchConfigurations(
        const std::vector<std::map<std::string, double>>& parameterGrid);
};

// Batch simulation runner with progress tracking
class BatchSimulationRunner {
public:
    BatchSimulationRunner(std::shared_ptr<Brain> brainTemplate,
                         std::shared_ptr<Environment> environmentTemplate,
                         BatchProgress& progress);
    
    // Run batch simulation with single configuration
    BatchSimulationResult runSingleSimulation(
        size_t batchIndex,
        std::shared_ptr<Config> config,
        int timesteps = 100);
    
    // Run batch of simulations in parallel
    std::vector<BatchSimulationResult> runParallelBatch(
        const std::vector<std::shared_ptr<Config>>& configs,
        int timesteps = 100,
        size_t maxThreads = 0);
    
    // Run batch of simulations sequentially
    std::vector<BatchSimulationResult> runSequentialBatch(
        const std::vector<std::shared_ptr<Config>>& configs,
        int timesteps = 100);
    
    // Get performance metrics for a batch
    std::map<std::string, double> getBatchPerformanceMetrics(
        const std::vector<BatchSimulationResult>& results) const;
    
private:
    std::shared_ptr<Brain> brainTemplate_;
    std::shared_ptr<Environment> environmentTemplate_;
    BatchProgress& progress_;
    ThreadPool threadPool_;
    
    BatchSimulationResult runSingleSimulationImpl(
        size_t batchIndex,
        std::shared_ptr<Config> config,
        int timesteps);
};

// Cross-validation utilities for neural networks
class CrossValidationUtilities {
public:
    struct CrossValidationResult {
        std::vector<double> foldMetrics; // Accuracy/F1 scores for each fold
        double meanScore;
        double stdDeviation;
        double confidenceInterval; // 95% CI
        
        CrossValidationResult(const std::vector<double>& metrics)
            : foldMetrics(metrics) {
            if (!metrics.empty()) {
                meanScore = std::accumulate(metrics.begin(), metrics.end(), 0.0) / metrics.size();
                double sumSq = 0.0;
                for (double m : metrics) {
                    sumSq += (m - meanScore) * (m - meanScore);
                }
                stdDeviation = std::sqrt(sumSq / metrics.size());
            } else {
                meanScore = stdDeviation = confidenceInterval = 0.0;
            }
            confidenceInterval = 1.96 * stdDeviation / std::sqrt(metrics.size()); // 95% CI
        }
    };
    
    // Perform k-fold cross-validation on brain simulations
    static CrossValidationResult crossValidate(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment,
        const std::vector<std::vector<size_t>>& folds,
        int timestepsPerFold = 50);
    
    // Generate stratified folds for balanced sampling
    static std::vector<std::vector<size_t>> stratifiedFolds(
        size_t totalSamples, size_t numFolds, const std::vector<size_t>& labels);
};nlm