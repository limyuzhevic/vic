#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <sstream>
#include "../experiments/ExperimentConfiguration.h"
#include "../experiments/AdvancedExperiment.h"

namespace nlm {

/**
 * Parameter optimization system for experiment configuration and performance
 * optimization using gradient descent, genetic algorithms, and other optimization
 * techniques to find optimal parameter values for experiments.
 */
class ParameterOptimizer {
public:
    // Optimization result
    struct OptimizationResult {
        std::string experimentName;
        ExperimentConfiguration optimizedConfig;
        double objectiveValue;
        double convergenceScore;
        size_t iterations;
        std::chrono::duration<double> optimizationTime;
        std::vector<double> gradientHistory;
        std::string optimizationMethod;
        
        OptimizationResult() : objectiveValue(0.0), convergenceScore(0.0), iterations(0) {}
    };
    
    // Optimization configuration
    struct OptimizationConfig {
        std::string optimizationMethod; // "gradient_descent", "genetic_algorithm", "random_search", "bayesian_optimization"
        double learningRate;
        double mutationRate;
        double crossoverRate;
        size_t populationSize;
        size_t maxIterations;
        double convergenceThreshold;
        bool useParallelOptimization;
        bool useConstraints;
        double penaltyWeight;
        
        OptimizationConfig()
            : optimizationMethod("gradient_descent"), learningRate(0.01), mutationRate(0.1),
              crossoverRate(0.8), populationSize(10), maxIterations(100),
              convergenceThreshold(1e-6), useParallelOptimization(false),
              useConstraints(true), penaltyWeight(10.0) {}
    };
    
    // Fitness function
    using FitnessFunction = std::function<double(const ExperimentConfiguration& config)>;
    
    // Constraint function
    using ConstraintFunction = std::function<bool(const ExperimentConfiguration& config)>;
    
    // Objective function
    using ObjectiveFunction = std::function<double(const std::vector<double>& parameters)>;
    
    ParameterOptimizer();
    ~ParameterOptimizer() = default;
    
    // Optimization
    OptimizationResult optimize(const std::string& experimentName, 
                              const ExperimentConfiguration& initialConfig,
                              const FitnessFunction& fitnessFunction,
                              const OptimizationConfig& config = OptimizationConfig());
    
    OptimizationResult optimize(const std::string& experimentName,
                              const std::vector<double>& initialParameters,
                              const ObjectiveFunction& objectiveFunction,
                              const OptimizationConfig& config = OptimizationConfig());
    
    // Individual optimization methods
    OptimizationResult gradientDescentOptimization(const std::string& experimentName,
                                                 const ExperimentConfiguration& initialConfig,
                                                 const FitnessFunction& fitnessFunction,
                                                 const OptimizationConfig& config);
    
    OptimizationResult geneticAlgorithmOptimization(const std::string& experimentName,
                                                   const ExperimentConfiguration& initialConfig,
                                                   const FitnessFunction& fitnessFunction,
                                                   const OptimizationConfig& config);
    
    OptimizationResult randomSearchOptimization(const std::string& experimentName,
                                               const ExperimentConfiguration& initialConfig,
                                               const FitnessFunction& fitnessFunction,
                                               const OptimizationConfig& config);
    
    OptimizationResult bayesianOptimization(const std::string& experimentName,
                                           const ExperimentConfiguration& initialConfig,
                                           const FitnessFunction& fitnessFunction,
                                           const OptimizationConfig& config);
    
    // Parameter encoding/decoding
    std::vector<double> encodeConfiguration(const ExperimentConfiguration& config) const;
    ExperimentConfiguration decodeParameters(const std::vector<double>& parameters,
                                            const ExperimentConfiguration& templateConfig) const;
    
    // Population management
    std::vector<ExperimentConfiguration> generateInitialPopulation(const ExperimentConfiguration& templateConfig,
                                                                  size_t populationSize) const;
    ExperimentConfiguration selectBestIndividual(const std::vector<ExperimentConfiguration>& population,
                                               const FitnessFunction& fitnessFunction) const;
    std::vector<ExperimentConfiguration> crossover(const ExperimentConfiguration& parent1,
                                                 const ExperimentConfiguration& parent2,
                                                 double crossoverRate) const;
    std::vector<ExperimentConfiguration> mutate(const std::vector<ExperimentConfiguration>& population,
                                               double mutationRate) const;
    
    // Fitness evaluation
    double evaluateFitness(const ExperimentConfiguration& config, const FitnessFunction& fitnessFunction) const;
    double evaluateObjective(const std::vector<double>& parameters,
                            const ObjectiveFunction& objectiveFunction,
                            const std::vector<ConstraintFunction>& constraints) const;
    
    // Convergence check
    bool hasConverged(const std::vector<double>& history, double threshold) const;
    double calculateConvergenceScore(const std::vector<double>& history) const;
    
    // Configuration management
    void setOptimizationConfig(const OptimizationConfig& config);
    const OptimizationConfig& getOptimizationConfig() const;
    
    // Result export/import
    bool exportOptimizationResults(const OptimizationResult& result, const std::string& filepath) const;
    bool importOptimizationResults(OptimizationResult& result, const std::string& filepath);
    
    // Optimization statistics
    std::string getOptimizationStatistics(const std::string& experimentName) const;
    double getBestObjectiveValue(const std::string& experimentName) const;
    size_t getOptimizationIterations(const std::string& experimentName) const;
    
    // Optimization history
    void recordOptimizationResult(const OptimizationResult& result);
    std::vector<OptimizationResult> getOptimizationHistory() const;
    void clearOptimizationHistory();
    
    // Configuration templates
    static std::vector<ExperimentConfiguration> getDefaultOptimizationConfigurations();
    static ExperimentConfiguration getTemplateConfig(const std::string& experimentName);
    
    // Utility functions
    std::vector<double> normalizeParameters(const std::vector<double>& parameters,
                                           const std::vector<double>& minValues,
                                           const std::vector<double>& maxValues) const;
    std::vector<double> denormalizeParameters(const std::vector<double>& parameters,
                                             const std::vector<double>& minValues,
                                             const std::vector<double>& maxValues) const;
    double calculateDistance(const std::vector<double>& params1, const std::vector<double>& params2) const;
    
private:
    // Optimization storage
    std::vector<OptimizationResult> optimizationHistory;
    
    // Configuration
    OptimizationConfig config;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> uniformDist;
    std::normal_distribution<double> normalDist;
    
    // Helper functions
    std::vector<double> computeGradient(const std::vector<double>& parameters,
                                        const ObjectiveFunction& objectiveFunction,
                                        double epsilon = 1e-6) const;
    std::vector<double> clipParameters(const std::vector<double>& parameters,
                                       const std::vector<double>& minValues,
                                       const std::vector<double>& maxValues) const;
    std::vector<double> applyConstraints(const std::vector<double>& parameters,
                                        const std::vector<ConstraintFunction>& constraints) const;
};

} // namespace nlm