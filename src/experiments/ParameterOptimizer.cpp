#include "AdvancedExperimentManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>

namespace nlm {

struct ParameterOptimizer::Impl {
    json optimizationHistory;
    std::map<std::string, json> optimizationResults;
    bool initialized;
};

ParameterOptimizer::ParameterOptimizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

ParameterOptimizer::~ParameterOptimizer() = default;

Phase6Config ParameterOptimizer::optimizeParameters(
    const std::vector<Phase6Config>& initialParams,
    const std::function<double(const Phase6Config&)>& fitnessFunction,
    size_t populationSize,
    size_t generations) {
    
    std::cout << "Running evolutionary optimization..." << std::endl;
    std::cout << "  Population size: " << populationSize << ", Generations: " << generations << std::endl;
    
    // Initialize population
    std::vector<Phase6Config> population = initialParams;
    std::vector<double> fitnesses;
    
    // Fill population with random variations of initial params if needed
    while (population.size() < populationSize && !initialParams.empty()) {
        Phase6Config newConfig = initialParams[0]; // Use first config as base
        
        // Add random variations
        newConfig.neuronCount = static_cast<size_t>(newConfig.neuronCount * (1.0 + (std::rand() % 100 - 50) / 100.0));
        newConfig.maxSteps = static_cast<uint64_t>(newConfig.maxSteps * (1.0 + (std::rand() % 100 - 50) / 100.0));
        newConfig.connectionProbability = std::max(0.01f, std::min(1.0f, newConfig.connectionProbability + (std::rand() % 100 - 50) / 1000.0f));
        
        population.push_back(newConfig);
    }
    
    // Evaluate initial population
    fitnesses.reserve(population.size());
    for (const auto& config : population) {
        double fitness = fitnessFunction(config);
        fitnesses.push_back(fitness);
    }
    
    std::cout << "  Initial population evaluated." << std::endl;
    
    // Evolution loop
    for (size_t generation = 0; generation < generations; ++generation) {
        std::cout << "  Generation " << generation + 1 << "/" << generations << std::endl;
        
        // Select parents
        std::vector<Phase6Config> newPopulation;
        newPopulation.reserve(populationSize);
        
        for (size_t i = 0; i < populationSize; ++i) {
            // Tournament selection
            size_t tournamentSize = 3;
            size_t bestIdx = i % population.size();
            
            for (size_t j = 1; j < tournamentSize; ++j) {
                size_t challenger = (i + j) % population.size();
                if (fitnesses[challenger] > fitnesses[bestIdx]) {
                    bestIdx = challenger;
                }
            }
            
            Phase6Config parent = population[bestIdx];
            
            // Crossover
            if (i % 2 == 0 && population.size() > 1) {
                size_t partner = (i + 1) % population.size();
                Phase6Config child = crossover(parent, population[partner]);
                
                // Mutation
                child = mutate(child, 0.1); // 10% mutation rate
                
                newPopulation.push_back(child);
            } else {
                newPopulation.push_back(parent);
            }
        }
        
        // Replace population
        population = std::move(newPopulation);
        
        // Evaluate new population
        fitnesses.clear();
        for (const auto& config : population) {
            double fitness = fitnessFunction(config);
            fitnesses.push_back(fitness);
        }
        
        // Log best fitness
        auto bestFitnessIt = std::max_element(fitnesses.begin(), fitnesses.end());
        size_t bestIdx = std::distance(fitnesses.begin(), bestFitnessIt);
        std::cout << "    Best fitness: " << *bestFitnessIt << " (config " << bestIdx << ")" << std::endl;
    }
    
    // Find best solution
    auto bestFitnessIt = std::max_element(fitnesses.begin(), fitnesses.end());
    size_t bestIdx = std::distance(fitnesses.begin(), bestFitnessIt);
    
    std::cout << "  Optimization completed. Best fitness: " << *bestFitnessIt << std::endl;
    std::cout << "  Best configuration: neurons=" << population[bestIdx].neuronCount 
              << ", steps=" << population[bestIdx].maxSteps 
              << ", conn=" << population[bestIdx].connectionProbability << std::endl;
    
    pImpl->optimizationResults["best_config"] = {
        {"neuronCount", population[bestIdx].neuronCount},
        {"maxSteps", population[bestIdx].maxSteps},
        {"regionCount", population[bestIdx].regionCount},
        {"connectionProbability", population[bestIdx].connectionProbability},
        {"enableCheckpointing", population[bestIdx].enableCheckpointing},
        {"enableReplay", population[bestIdx].enableReplay},
        {"enableDevelopment", population[bestIdx].enableDevelopment}
    };
    
    pImpl->optimizationResults["best_fitness"] = *bestFitnessIt;
    pImpl->optimizationResults["generations"] = static_cast<int>(generations);
    
    pImpl->initialized = true;
    
    return population[bestIdx];
}
json ParameterOptimizer::gridSearch(
    const std::map<std::string, std::vector<double>>& parameterRanges,
    const std::function<double(const Phase6Config&)>& fitnessFunction) {
    
    std::cout << "Running grid search..." << std::endl;
    
    json results;
    std::vector<Phase6Config> configs;
    std::vector<double> fitnesses;
    
    // Create grid from parameter ranges
    // For demonstration, use simple ranges
    std::vector<double> neuronCounts = {500, 1000, 2000};
    std::vector<double> steps = {1000, 5000, 10000};
    std::vector<double> connectionProbs = {0.05, 0.1, 0.2};
    
    for (double neurons : neuronCounts) {
        for (double step : steps) {
            for (double connProb : connectionProbs) {
                Phase6Config config;
                config.neuronCount = static_cast<size_t>(neurons);
                config.maxSteps = static_cast<uint64_t>(step);
                config.regionCount = 1;
                config.connectionProbability = static_cast<float>(connProb);
                config.enableCheckpointing = true;
                config.enableReplay = true;
                config.enableDevelopment = true;
                
                configs.push_back(config);
            }
        }
    }
    
    std::cout << "  Created grid with " << configs.size() << " configurations" << std::endl;
    
    // Evaluate all configurations
    fitnesses.reserve(configs.size());
    for (const auto& config : configs) {
        double fitness = fitnessFunction(config);
        fitnesses.push_back(fitness);
    }
    
    // Find best configuration
    auto bestFitnessIt = std::max_element(fitnesses.begin(), fitnesses.end());
    size_t bestIdx = std::distance(fitnesses.begin(), bestFitnessIt);
    
    results["best_config"] = configs[bestIdx];
    results["best_fitness"] = *bestFitnessIt;
    results["total_evaluated"] = configs.size();
    
    // Create results table
    json resultsTable = json::array();
    for (size_t i = 0; i < configs.size(); ++i) {
        json row;
        row["config_id"] = i;
        row["neurons"] = configs[i].neuronCount;
        row["steps"] = configs[i].maxSteps;
        row["connection_prob"] = configs[i].connectionProbability;
        row["fitness"] = fitnesses[i];
        row["rank"] = static_cast<int>(configs.size() - i); // Lower is better rank
        
        resultsTable.push_back(row);
    }
    
    results["results_table"] = resultsTable;
    
    std::cout << "  Grid search completed. Best fitness: " << *bestFitnessIt << std::endl;
    
    return results;
}
Phase6Config ParameterOptimizer::randomSearch(
    const std::map<std::string, std::vector<double>>& parameterRanges,
    const std::function<double(const Phase6Config&)>& fitnessFunction,
    size_t iterations) {
    
    std::cout << "Running random search with " << iterations << " iterations" << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    
    // Parameter ranges for random search
    std::vector<double> neuronRanges = {100, 1000, 5000};
    std::vector<double> stepRanges = {500, 2000, 10000};
    std::vector<double> connRanges = {0.01, 0.1, 0.5};
    
    Phase6Config bestConfig;
    double bestFitness = -1.0;
    
    for (size_t i = 0; i < iterations; ++i) {
        Phase6Config config;
        
        // Randomly select parameter values
        config.neuronCount = static_cast<size_t>(neuronRanges[uniform(gen) * neuronRanges.size()]);
        config.maxSteps = static_cast<uint64_t>(stepRanges[uniform(gen) * stepRanges.size()]);
        config.regionCount = 1;
        config.connectionProbability = static_cast<float>(connRanges[uniform(gen) * connRanges.size()]);
        config.enableCheckpointing = uniform(gen) > 0.5;
        config.enableReplay = uniform(gen) > 0.5;
        config.enableDevelopment = uniform(gen) > 0.5;
        
        double fitness = fitnessFunction(config);
        
        if (fitness > bestFitness) {
            bestFitness = fitness;
            bestConfig = config;
        }
        
        if ((i + 1) % 100 == 0) {
            std::cout << "    Iteration " << i + 1 << "/" << iterations << ", best fitness: " << bestFitness << std::endl;
        }
    }
    
    std::cout << "  Random search completed. Best fitness: " << bestFitness << std::endl;
    
    // Store results
    pImpl->optimizationResults["random_search_best"] = {
        {"config", bestConfig},
        {"fitness", bestFitness},
        {"iterations", static_cast<int>(iterations)}
    };
    
    return bestConfig;
}
Phase6Config ParameterOptimizer::bayesianOptimization(
    const std::map<std::string, std::vector<double>>& parameterRanges,
    const std::function<double(const Phase6Config&)>& fitnessFunction,
    size_t iterations) {
    
    std::cout << "Running Bayesian optimization with " << iterations << " iterations" << std::endl;
    
    // For demonstration, implement a simple Bayesian optimization approximation
    // In a real implementation, this would use Gaussian processes or similar
    
    std::cout << "  Bayesian optimization requires extensive data collection." << std::endl;
    std::cout << "  Implementing simplified version for demonstration." << std::endl;
    
    // Create a few initial points
    std::vector<Phase6Config> initialPoints;
    std::vector<double> initialFitnesses;
    
    // Generate a few diverse initial configurations
    for (size_t i = 0; i < std::min(size_t(10), iterations); ++i) {
        Phase6Config config;
        config.neuronCount = 500 + (i * 500);
        config.maxSteps = 1000 + (i * 2000);
        config.regionCount = 1;
        config.connectionProbability = 0.05 + (i * 0.05);
        config.enableCheckpointing = true;
        config.enableReplay = i % 2 == 0;
        config.enableDevelopment = i % 3 == 0;
        
        double fitness = fitnessFunction(config);
        initialPoints.push_back(config);
        initialFitnesses.push_back(fitness);
    }
    
    // Find best initial point
    auto bestIt = std::max_element(initialFitnesses.begin(), initialFitnesses.end());
    size_t bestIdx = std::distance(initialFitnesses.begin(), bestIt);
    
    std::cout << "  Best initial fitness: " << *bestIt << std::endl;
    
    // Store results
    pImpl->optimizationResults["bayesian_optimization"] = {
        {"best_config", initialPoints[bestIdx]},
        {"best_fitness", *bestIt},
        {"iterations", static_cast<int>(iterations)},
        {"initial_points", initialPoints.size()}
    };
    
    return initialPoints[bestIdx];
}
bool ParameterOptimizer::saveOptimizationResults(const std::string& filename) {
    std::cout << "Saving optimization results to: " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << pImpl->optimizationResults.dump(2);
    file.close();
    
    std::cout << "  Optimization results saved successfully" << std::endl;
    return true;
}
bool ParameterOptimizer::loadOptimizationResults(const std::string& filename) {
    std::cout << "Loading optimization results from: " << filename << std::endl;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        file >> pImpl->optimizationResults;
    } catch (const json::parse_error& e) {
        std::cout << "Error parsing optimization results: " << e.what() << std::endl;
        return false;
    }
    
    file.close();
    
    pImpl->initialized = !pImpl->optimizationResults.empty();
    
    std::cout << "  Optimization results loaded successfully" << std::endl;
    return true;
}
json ParameterOptimizer::getOptimizationHistory() const {
    return pImpl->optimizationResults;
}

// Helper functions for ParameterOptimizer

Phase6Config ParameterOptimizer::crossover(const Phase6Config& parent1, const Phase6Config& parent2) {
    Phase6Config child;
    
    // Simple crossover: mix parameters from parents
    child.neuronCount = (parent1.neuronCount + parent2.neuronCount) / 2;
    child.maxSteps = (parent1.maxSteps + parent2.maxSteps) / 2;
    child.regionCount = (parent1.regionCount + parent2.regionCount) / 2;
    child.connectionProbability = (parent1.connectionProbability + parent2.connectionProbability) / 2;
    
    // Randomly set boolean flags
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    
    child.enableCheckpointing = uniform(gen) > 0.5;
    child.enableReplay = uniform(gen) > 0.5;
    child.enableDevelopment = uniform(gen) > 0.5;
    
    child.checkpointPath = "crossover_checkpoint.bin";
    
    return child;
}

Phase6Config ParameterOptimizer::mutate(const Phase6Config& config, double mutationRate) {
    Phase6Config mutated = config;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    std::uniform_int_distribution<size_t> intDist(0, 2);
    
    if (uniform(gen) < mutationRate) {
        // Mutate neuron count
        double change = (uniform(gen) - 0.5) * 0.2; // +/- 20%
        double newNeurons = mutated.neuronCount * (1.0 + change);
        mutated.neuronCount = static_cast<size_t>(std::max(100.0, std::min(10000.0, newNeurons)));
    }
    
    if (uniform(gen) < mutationRate) {
        // Mutate steps
        double change = (uniform(gen) - 0.5) * 0.3; // +/- 30%
        double newSteps = static_cast<double>(mutated.maxSteps) * (1.0 + change);
        mutated.maxSteps = static_cast<uint64_t>(std::max(100.0, std::min(100000.0, newSteps)));
    }
    
    if (uniform(gen) < mutationRate) {
        // Mutate connection probability
        double change = (uniform(gen) - 0.5) * 0.4; // +/- 40%
        double newConn = mutated.connectionProbability + change;
        mutated.connectionProbability = std::max(0.01f, std::min(1.0f, newConn));
    }
    
    // Randomly flip boolean flags with small probability
    if (uniform(gen) < mutationRate * 0.5) {
        mutated.enableCheckpointing = !mutated.enableCheckpointing;
    }
    
    if (uniform(gen) < mutationRate * 0.5) {
        mutated.enableReplay = !mutated.enableReplay;
    }
    
    if (uniform(gen) < mutationRate * 0.5) {
        mutated.enableDevelopment = !mutated.enableDevelopment;
    }
    
    mutated.checkpointPath = "mutated_checkpoint.bin";
    
    return mutated;
}

} // namespace nlm
