#pragma once

#include "EnhancedFramework.hpp"
#include "EnhancedExperiment.hpp"
#include "EnhancedMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <random>
#include <cmath>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct ParameterTuner::Impl {
    std::string optimizationMode;
    size_t optimizationIterations;
    size_t populationSize;
    double mutationRate;
    double crossoverRate;
    std::mt19937 rng;
};

ParameterTuner::ParameterTuner() : pImpl(new Impl) {
    pImpl->optimizationMode = "bayesian";
    pImpl->optimizationIterations = 100;
    pImpl->populationSize = 50;
    pImpl->mutationRate = 0.1;
    pImpl->crossoverRate = 0.8;
    pImpl->rng.seed(std::random_device()());
}

ParameterTuner::~ParameterTuner() = default;

std::unordered_map<std::string, std::string> ParameterTuner::optimizeParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                                  const std::unordered_map<std::string, std::string>& initialParams) {
    // For now, return the initial params (simplified)
    // In a real implementation, this would run the optimization algorithms
    return initialParams;
}

std::unordered_map<std::string, std::string> ParameterTuner::bayesianOptimization(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                                    const std::unordered_map<std::string, std::string>& initialParams) {
    // Bayesian optimization (simplified)
    std::unordered_map<std::string, std::string> bestParams = initialParams;
    
    for (size_t iteration = 0; iteration < pImpl->optimizationIterations; ++iteration) {
        // Generate new parameters
        auto newParams = generateRandomParameters(initialParams);
        
        // Evaluate parameters (run experiment with new params)
        double fitness = evaluateParameters(experiment, newParams);
        
        // Update best parameters if improvement
        if (fitness > getExperimentFitness(experiment)) {
            bestParams = newParams;
        }
        
        // Update exploration-exploitation balance
        if (iteration % 10 == 0) {
            pImpl->mutationRate *= 0.9; // Decrease exploration
            pImpl->crossoverRate *= 1.1; // Increase exploitation
        }
    }
    
    return bestParams;
}

std::unordered_map<std::string, std::string> ParameterTuner::evolutionaryOptimization(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                                         const std::unordered_map<std::string, std::string>& initialParams) {
    // Evolutionary algorithm (simplified)
    std::vector<std::unordered_map<std::string, std::string>> population;
    
    // Initialize population
    for (size_t i = 0; i < pImpl->populationSize; ++i) {
        population.push_back(generateRandomParameters(initialParams));
    }
    
    // Evolution loop
    for (size_t generation = 0; generation < pImpl->optimizationIterations / 10; ++generation) {
        std::vector<double> fitnessScores;
        
        // Evaluate population
        for (const auto& params : population) {
            double fitness = evaluateParameters(experiment, params);
            fitnessScores.push_back(fitness);
        }
        
        // Create new population
        std::vector<std::unordered_map<std::string, std::string>> newPopulation;
        
        // Elitism: keep best individual
        size_t bestIndex = std::max_element(fitnessScores.begin(), fitnessScores.end()) - fitnessScores.begin();
        newPopulation.push_back(population[bestIndex]);
        
        // Generate offspring
        while (newPopulation.size() < pImpl->populationSize) {
            // Select parents
            std::uniform_int_distribution<size_t> dist(0, population.size() - 1);
            size_t parent1 = dist(pImpl->rng);
            size_t parent2 = dist(pImpl->rng);
            
            // Crossover
            auto child = crossover(population[parent1], population[parent2]);
            
            // Mutate
            if ((rand() % 100) < pImpl->mutationRate * 100) {
                mutate(child);
            }
            
            newPopulation.push_back(child);
        }
        
        population = newPopulation;
    }
    
    // Return best individual
    double maxFitness = -1.0;
    std::unordered_map<std::string, std::string> bestParams;
    
    for (size_t i = 0; i < population.size(); ++i) {
        double fitness = evaluateParameters(experiment, population[i]);
        if (fitness > maxFitness) {
            maxFitness = fitness;
            bestParams = population[i];
        }
    }
    
    return bestParams;
}

std::unordered_map<std::string, std::string> ParameterTuner::gridSearch(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                         const std::unordered_map<std::string, std::vector<std::string>>& parameterGrid) {
    // Grid search (simplified)
    std::unordered_map<std::string, std::string> bestParams;
    double bestFitness = -1.0;
    
    // This is a simplified implementation - full grid search would be computationally expensive
    // For demonstration, just return the first set of parameters
    if (!parameterGrid.empty()) {
        std::unordered_map<std::string, std::string> currentParams;
        for (const auto& pair : parameterGrid) {
            currentParams[pair.first] = pair.second.front();
        }
        
        double fitness = evaluateParameters(experiment, currentParams);
        if (fitness > bestFitness) {
            bestFitness = fitness;
            bestParams = currentParams;
        }
    }
    
    return bestParams;
}

std::unordered_map<std::string, std::string> ParameterTuner::adaptParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                              const std::unordered_map<std::string, std::string>& currentParams,
                                                                              const std::string& adaptationStrategy) {
    if (adaptationStrategy == "simple") {
        return simpleAdaptation(experiment, currentParams);
    } else if (adaptationStrategy == "ml_based") {
        return mlBasedAdaptation(experiment, currentParams);
    } else {
        return currentParams; // No adaptation
    }
}

bool ParameterTuner::validateParameters(const std::unordered_map<std::string, std::string>& params,
                                        const std::shared_ptr<EnhancedExperiment>& experiment) {
    // Basic parameter validation
    if (!experiment) return false;
    
    // Check required parameters
    auto requiredParams = getExperimentRequiredParameters(experiment);
    for (const auto& param : requiredParams) {
        if (params.find(param) == params.end()) {
            return false;
        }
    }
    
    // Validate parameter ranges
    auto paramRanges = getExperimentParameterRanges(experiment);
    for (const auto& pair : params) {
        auto rangeIt = paramRanges.find(pair.first);
        if (rangeIt != paramRanges.end()) {
            // In a real implementation, would validate parameter is within range
        }
    }
    
    return true;
}

std::vector<std::string> ParameterTuner::getParameterRanges(const std::string& parameterName,
                                                            const std::shared_ptr<EnhancedExperiment>& experiment) {
    return {}; // Empty for now
}

std::unordered_map<std::string, std::string> ParameterTuner::suggestParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                               const std::string& experimentType) {
    // Suggest parameters based on experiment type
    std::unordered_map<std::string, std::string> suggestions;
    
    if (experimentType == "memory_integration") {
        suggestions = {
            {"neuron_count", "100"},
            {"working_memory_capacity", "50"},
            {"encoding_strength", "0.8"},
            {"consolidation_rate", "0.01"}
        };
    } else if (experimentType == "prediction_system") {
        suggestions = {
            {"neuron_count", "100"},
            {"prediction_horizon", "3"},
            {"learning_rate", "0.01"},
            {"exploration_rate", "0.1"}
        };
    } else if (experimentType == "cognitive_benchmark") {
        suggestions = {
            {"neuron_count", "100"},
            {"enable_working_memory", "true"},
            {"enable_prediction", "true"},
            {"enable_planning", "true"}
        };
    } else if (experimentType == "neuromodulation") {
        suggestions = {
            {"neuron_count", "100"},
            {"dopamine_level", "1.0"},
            {"curiosity_level", "0.8"},
            {"novelty_level", "0.7"}
        };
    } else if (experimentType == "development") {
        suggestions = {
            {"neuron_count", "100"},
            {"developmental_stages", "5"},
            {"plasticity_rate", "0.1"},
            {"maturation_rate", "0.05"}
        };
    } else if (experimentType == "social_learning") {
        suggestions = {
            {"neuron_count", "100"},
            {"num_agents", "2"},
            {"imitation_rate", "0.7"},
            {"observation_efficiency", "0.8"}
        };
    }
    
    return suggestions;
}

std::vector<std::string> ParameterTuner::getParameterRecommendations(const std::string& experimentType) {
    std::vector<std::string> recommendations;
    
    if (experimentType == "memory_integration") {
        recommendations = {
            "Start with moderate neuron count (50-200)",
            "Set working memory capacity to ~50% of neuron count",
            "Use encoding strength between 0.6-0.9",
            "Set consolidation rate based on expected memory lifetime"
        };
    } else if (experimentType == "prediction_system") {
        recommendations = {
            "Set prediction horizon to 2-5 time steps",
            "Use learning rate between 0.001-0.05",
            "Balance exploration rate based on task complexity",
            "Start with higher prediction accuracy (0.6-0.8)"
        };
    }
    
    return recommendations;
}

void ParameterTuner::setOptimizationMode(const std::string& mode) {
    pImpl->optimizationMode = mode;
}

void ParameterTuner::setOptimizationIterations(size_t iterations) {
    pImpl->optimizationIterations = iterations;
}

void ParameterTuner::setPopulationSize(size_t size) {
    pImpl->populationSize = size;
}

void ParameterTuner::setMutationRate(double rate) {
    pImpl->mutationRate = std::min(1.0, std::max(0.0, rate));
}

void ParameterTuner::setCrossoverRate(double rate) {
    pImpl->crossoverRate = std::min(1.0, std::max(0.0, rate));
}

std::string ParameterTuner::generateOptimizationReport(const std::unordered_map<std::string, std::string>& bestParams,
                                                       const std::vector<double>& fitnessScores) const {
    std::stringstream ss;
    ss << "Parameter Optimization Report" << std::endl;
    ss << "===========================" << std::endl << std::endl;
    
    ss << "Optimization Mode: " << pImpl->optimizationMode << std::endl;
    ss << "Iterations: " << pImpl->optimizationIterations << std::endl;
    ss << "Population Size: " << pImpl->populationSize << std::endl << std::endl;
    
    ss << "Best Parameters:" << std::endl;
    for (const auto& pair : bestParams) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    if (!fitnessScores.empty()) {
        double avgFitness = std::accumulate(fitnessScores.begin(), fitnessScores.end(), 0.0) / fitnessScores.size();
        double maxFitness = *std::max_element(fitnessScores.begin(), fitnessScores.end());
        double minFitness = *std::min_element(fitnessScores.begin(), fitnessScores.end());
        
        ss << std::endl << "Fitness Statistics:" << std::endl;
        ss << "  Average: " << avgFitness << std::endl;
        ss << "  Maximum: " << maxFitness << std::endl;
        ss << "  Minimum: " << minFitness << std::endl;
        ss << "  Range: " << (maxFitness - minFitness) << std::endl;
    }
    
    return ss.str();
}

std::string ParameterTuner::generateParameterSensitivityReport(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                const std::unordered_map<std::string, std::string>& params) const {
    std::stringstream ss;
    ss << "Parameter Sensitivity Report" << std::endl;
    ss << "===========================" << std::endl << std::endl;
    
    ss << "Experiment: " << (experiment ? experiment->getName() : "Unknown") << std::endl;
    ss << std::endl << "Parameters:" << std::endl;
    
    for (const auto& pair : params) {
        // In a real implementation, would calculate sensitivity
        double sensitivity = 0.5; // Placeholder
        
        ss << "  " << pair.first << ": " << pair.second << " (sensitivity: " << sensitivity << ")" << std::endl;
    }
    
    return ss.str();
}

// Helper methods
std::unordered_map<std::string, std::string> ParameterTuner::generateRandomParameters(const std::unordered_map<std::string, std::string>& baseParams) {
    std::unordered_map<std::string, std::string> params = baseParams;
    
    // Apply random mutations
    std::uniform_int_distribution<size_t> keyDist(0, params.size() - 1);
    size_t keyIndex = keyDist(pImpl->rng);
    
    auto it = std::next(params.begin(), keyIndex);
    std::string newValue = std::to_string(rand() % 100); // Random value
    params[it->first] = newValue;
    
    return params;
}

double ParameterTuner::evaluateParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                          const std::unordered_map<std::string, std::string>& params) {
    if (!experiment) return 0.0;
    
    // Run experiment with given parameters and return fitness
    // This is a placeholder - in a real implementation would run the experiment
    return 0.5 + (rand() % 50) / 100.0; // Random fitness between 0.5 and 1.0
}

double ParameterTuner::getExperimentFitness(const std::shared_ptr<EnhancedExperiment>& experiment) {
    if (!experiment) return 0.0;
    
    // Get fitness from experiment metrics
    double fitness = 0.0;
    size_t metricCount = 0;
    
    for (const auto& metric : experiment->getMetrics()) {
        if (metric.first.find("accuracy") != std::string::npos ||
            metric.first.find("success") != std::string::npos) {
            fitness += metric.second;
            metricCount++;
        }
    }
    
    return metricCount > 0 ? fitness / metricCount : 0.0;
}

std::unordered_map<std::string, std::string> ParameterTuner::simpleAdaptation(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                                 const std::unordered_map<std::string, std::string>& currentParams) {
    // Simple parameter adaptation based on experiment results
    std::unordered_map<std::string, std::string> adaptedParams = currentParams;
    
    if (!experiment) return adaptedParams;
    
    // Adjust parameters based on experiment performance
    double performance = getExperimentFitness(experiment);
    
    if (performance < 0.5) {
        // If performance is poor, increase exploration or reduce complexity
        for (auto& pair : adaptedParams) {
            if (pair.first == "learning_rate") {
                double newRate = std::stod(pair.second) * 1.2;
                if (newRate > 0.5) newRate = 0.5;
                pair.second = std::to_string(newRate);
            } else if (pair.first == "exploration_rate") {
                double newRate = std::stod(pair.second) * 1.5;
                if (newRate > 1.0) newRate = 1.0;
                pair.second = std::to_string(newRate);
            }
        }
    }
    
    return adaptedParams;
}

std::unordered_map<std::string, std::string> ParameterTuner::mlBasedAdaptation(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                                  const std::unordered_map<std::string, std::string>& currentParams) {
    // ML-based parameter adaptation (placeholder)
    return currentParams;
}

std::vector<std::string> ParameterTuner::getExperimentRequiredParameters(const std::shared_ptr<EnhancedExperiment>& experiment) {
    return {}; // Empty for now
}

std::unordered_map<std::string, std::string> ParameterTuner::getExperimentParameterRanges(const std::shared_ptr<EnhancedExperiment>& experiment) {
    return {}; // Empty for now
}

std::unordered_map<std::string, std::string> ParameterTuner::crossover(const std::unordered_map<std::string, std::string>& parent1,
                                                                       const std::unordered_map<std::string, std::string>& parent2) {
    std::unordered_map<std::string, std::string> child;
    
    for (const auto& pair : parent1) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        if (dist(pImpl->rng) < pImpl->crossoverRate) {
            child[pair.first] = pair.second;
        } else {
            auto it = parent2.find(pair.first);
            if (it != parent2.end()) {
                child[pair.first] = it->second;
            } else {
                child[pair.first] = pair.second;
            }
        }
    }
    
    return child;
}

void ParameterTuner::mutate(std::unordered_map<std::string, std::string>& params) {
    std::uniform_int_distribution<size_t> keyDist(0, params.size() - 1);
    size_t keyIndex = keyDist(pImpl->rng);
    
    auto it = std::next(params.begin(), keyIndex);
    double value = std::stod(it->second);
    double mutation = (rand() % 200 - 100) / 100.0 * pImpl->mutationRate; // ±mutation * mutationRate
    value += mutation;
    
    // Ensure value is reasonable
    if (value < 0.0) value = 0.0;
    if (value > 1.0) value = 1.0;
    
    it->second = std::to_string(value);
}

} // namespace nlm
