#pragma once

#include "BatchProcessor.hpp"
#include "ParameterSweep.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <future>
#include <thread>

namespace nlm {

struct BatchProcessor::Impl {
    // Configuration
    size_t maxParallelBatches = 4;
    size_t defaultBatchSize = 10;
    float progressUpdateInterval = 1.0f; // seconds
    
    // Random number generator
    std::mt19937 rng;
    
    // Progress tracking
    std::map<std::string, float> batchProgress;
    std::map<std::string, std::chrono::steady_clock::time_point> batchStartTimes;
    std::mutex progressMutex;
    
    Impl() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}
};

BatchProcessor::BatchProcessor() : pImpl(std::make_unique<Impl>()) {}

BatchProcessor::~BatchProcessor() = default;

bool BatchProcessor::validateBatchConfig(const BatchConfig& config) const {
    if (config.batchName.empty()) {
        return false;
    }
    
    if (config.simulations.empty()) {
        return false;
    }
    
    // Validate simulation configurations
    for (const auto& sim : config.simulations) {
        if (sim.name.empty()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::executeBatch(const BatchConfig& config) {
    if (!validateBatchConfig(config)) {
        NLM_LOG_ERROR("Invalid batch configuration");
        return {};
    }
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    // Initialize progress tracking
    {
        std::lock_guard<std::mutex> lock(pImpl->progressMutex);
        pImpl->batchProgress[config.batchName] = 0.0f;
        pImpl->batchStartTimes[config.batchName] = std::chrono::steady_clock::now();
    }
    
    NLM_LOG_INFO("Starting batch: " + config.batchName + ", simulations: " + std::to_string(config.simulations.size()));
    
    // Execute simulations
    size_t completed = 0;
    
    for (const auto& sim : config.simulations) {
        // Run simulation
        auto experiment = runSingleSimulation(sim);
        if (experiment) {
            results.push_back(experiment);
        }
        
        completed++;
        
        // Update progress
        float progress = static_cast<float>(completed) / config.simulations.size();
        {
            std::lock_guard<std::mutex> lock(pImpl->progressMutex);
            pImpl->batchProgress[config.batchName] = progress;
        }
        
        NLM_LOG_INFO("Batch " + config.batchName + ": " + std::to_string(completed) + 
                    "/" + std::to_string(config.simulations.size()) + " completed, " +
                    std::to_string(sim.parameterSets.size()) + " parameter sets each")
    }
    
    // Clean up progress tracking
    {
        std::lock_guard<std::mutex> lock(pImpl->progressMutex);
        pImpl->batchProgress.erase(config.batchName);
        pImpl->batchStartTimes.erase(config.batchName);
    }
    
    NLM_LOG_INFO("Batch " + config.batchName + " completed: " + std::to_string(results.size()) + 
                "/" + std::to_string(config.simulations.size()) + " successful simulations");
    
    return results;
}

std::vector<std::vector<std::shared_ptr<Experiment>>> BatchProcessor::executeBatchInParallel(
    const std::vector<BatchConfig>& configs) {
    std::vector<std::vector<std::shared_ptr<Experiment>>> allResults;
    
    // Create threads for parallel execution
    std::vector<std::future<std::vector<std::shared_ptr<Experiment>>>> futures;
    
    for (const auto& config : configs) {
        futures.push_back(std::async(std::launch::async, 
            [this, config]() { return executeBatch(config); }));
    }
    
    // Wait for all to complete
    for (auto& future : futures) {
        try {
            auto results = future.get();
            allResults.push_back(results);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Parallel batch execution failed: " + std::string(e.what()));
        }
    }
    
    return allResults;
}

std::shared_ptr<Experiment> BatchProcessor::runSingleSimulation(const SimulationConfig& sim) {
    std::shared_ptr<Experiment> experiment;
    
    NLM_LOG_INFO("Running simulation: " + sim.name + ", parameter sets: " + std::to_string(sim.parameterSets.size()));
    
    // For each parameter set, create a simulation
    for (const auto& params : sim.parameterSets) {
        // In a real implementation, this would create a brain with specific parameters
        // and run the simulation
        
        // For now, create a placeholder experiment
        auto exp = std::make_shared<Experiment>(sim.name + "_params_" + std::to_string(params.size()));
        
        // Add some metrics based on parameters
        exp->recordMetric("total_neurons", static_cast<double>(params.count("neuron_count") ? 
            std::get<size_t>(params.at("neuron_count")) : 1000));
        exp->recordMetric("connection_probability", static_cast<double>(params.count("connection_probability") ? 
            std::get<float>(params.at("connection_probability")) : 0.1f));
        exp->recordMetric("plasticity_rate", static_cast<double>(params.count("plasticity_rate") ? 
            std::get<float>(params.at("plasticity_rate")) : 0.01f));
        
        if (!experiment) {
            experiment = exp;
        }
        
        NLM_LOG_INFO("  Completed parameter set for simulation: " + sim.name);
    }
    
    return experiment;
}

std::vector<SimulationConfig> BatchProcessor::generateParameterSets(const ParameterSweep& sweep) {
    std::vector<SimulationConfig> simulations;
    
    // For now, create a single simulation with the sweep parameters
    SimulationConfig sim;
    sim.name = "parameter_sweep";
    
    // Extract parameter sets from sweep
    for (const auto& param : sweep.getParameters()) {
        std::vector<double> values = param.second;
        
        // For simplicity, just create one combination
        std::map<std::string, std::any> paramSet;
        size_t index = 0;
        for (const auto& [key, values] : sweep.getParameters()) {
            if (!values.empty()) {
                // Use first value for demonstration
                paramSet[key] = values[0];
                index++;
            }
        }
        
        sim.parameterSets.push_back(paramSet);
    }
    
    simulations.push_back(sim);
    return simulations;
}

BatchConfig BatchProcessor::createParameterSweepBatch(const std::string& batchName,
                                                     const ParameterSweep& sweep,
                                                     size_t batchSize) {
    BatchConfig config;
    config.batchName = batchName;
    config.type = "parameter_sweep";
    
    // Generate parameter sets
    std::vector<SimulationConfig> simulations = generateParameterSets(sweep);
    
    // Adjust batch size if needed
    if (batchSize > 0 && simulations.size() > batchSize) {
        // This would implement batch splitting logic
        NLM_LOG_WARNING("Batch splitting logic not fully implemented yet");
    }
    
    config.simulations = simulations;
    config.progressTracking = true;
    config.parallelExecution = true;
    
    return config;
}

std::string BatchProcessor::generateBatchReport(const std::vector<std::shared_ptr<Experiment>>& experiments) const {
    std::stringstream ss;
    ss << "Batch Processing Report:\n";
    ss << "=======================\n\n";
    
    if (experiments.empty()) {
        ss << "No experiments executed.\n";
        return ss.str();
    }
    
    ss << "Summary:\n";
    ss << "  Total experiments: " << experiments.size() << "\n";
    ss << "  Successful: " << countSuccessfulExperiments(experiments) << "\n";
    ss << "  Success rate: " << (countSuccessfulExperiments(experiments) * 100.0 / experiments.size()) << "%\n\n";
    
    ss << "Detailed Results:\n";
    for (size_t i = 0; i < experiments.size(); ++i) {
        ss << "  Experiment " << i + 1 << ": " << experiments[i]->getName() << "\n";
        
        // Display key metrics
        const auto& metrics = experiments[i]->getMetrics();
        for (const auto& metric : metrics) {
            ss << "    " << metric.first << ": " << metric.second << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

float BatchProcessor::getBatchProgress(const std::string& batchName) const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    auto it = pImpl->batchProgress.find(batchName);
    if (it == pImpl->batchProgress.end()) {
        return 0.0f;
    }
    return it->second;
}

std::map<std::string, float> BatchProcessor::getAllBatchProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    return pImpl->batchProgress;
}

float BatchProcessor::estimateBatchCompletionTime(const BatchConfig& config) const {
    // Estimate based on simulation count and average simulation time
    // For now, return a placeholder
    return static_cast<float>(config.simulations.size() * 100.0);
}

bool BatchProcessor::cancelBatch(const std::string& batchName) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    auto it = pImpl->batchProgress.find(batchName);
    if (it == pImpl->batchProgress.end()) {
        return false;
    }
    
    it->second = -1.0f; // Canceled
    return true;
}

bool BatchProcessor::pauseBatch(const std::string& batchName) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    auto it = pImpl->batchProgress.find(batchName);
    if (it == pImpl->batchProgress.end()) {
        return false;
    }
    
    // Set to paused state
    it->second = -2.0f; // Paused
    return true;
}

bool BatchProcessor::resumeBatch(const std::string& batchName) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    auto it = pImpl->batchProgress.find(batchName);
    if (it == pImpl->batchProgress.end()) {
        return false;
    }
    
    // Resume from current progress
    if (it->second < 0) {
        it->second = 0.0f; // Start from beginning
    }
    return true;
}

std::vector<std::string> BatchProcessor::getActiveBatches() const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    std::vector<std::string> activeBatches;
    
    for (const auto& pair : pImpl->batchProgress) {
        if (pair.second >= 0.0f && pair.second <= 1.0f) {
            activeBatches.push_back(pair.first);
        }
    }
    
    return activeBatches;
}

BatchConfig BatchProcessor::createOptimizedBatch(const std::string& batchName,
                                                const std::vector<SimulationConfig>& simulations,
                                                const OptimizationCriteria& criteria) {
    BatchConfig config;
    config.batchName = batchName;
    config.type = "optimized";
    config.simulations = simulations;
    config.optimizationCriteria = criteria;
    
    // Sort simulations based on optimization criteria
    std::sort(config.simulations.begin(), config.simulations.end(),
        [&criteria](const SimulationConfig& a, const SimulationConfig& b) {
            return evaluateSimulationPriority(a, criteria) > evaluateSimulationPriority(b, criteria);
        }
    );
    
    return config;
}

float BatchProcessor::evaluateSimulationPriority(const SimulationConfig& sim,
                                                const OptimizationCriteria& criteria) const {
    // Evaluate simulation priority based on criteria
    // For now, return a placeholder
    return 1.0f;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runHyperparameterOptimization(
    const HyperparameterConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running hyperparameter optimization for: " + config.objectiveFunction);
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.numIterations; ++i) {
        auto exp = std::make_shared<Experiment>("hyperparameter_iteration_" + std::to_string(i));
        exp->recordMetric("objective_value", static_cast<double>(0.5 + (i * 0.1)));
        exp->recordMetric("parameters", "param_" + std::to_string(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runEvolutionaryAlgorithm(
    const EvolutionConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running evolutionary algorithm for: " + config.objectiveFunction);
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.generations; ++i) {
        auto exp = std::make_shared<Experiment>("evolution_generation_" + std::to_string(i));
        exp->recordMetric("fitness", static_cast<double>(0.3 + (i * 0.05)));
        exp->recordMetric("generation", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runGeneticAlgorithm(
    const GeneticAlgorithmConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running genetic algorithm with population: " + std::to_string(config.populationSize));
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.generations; ++i) {
        auto exp = std::make_shared<Experiment>("genetic_generation_" + std::to_string(i));
        exp->recordMetric("best_fitness", static_cast<double>(0.4 + (i * 0.03)));
        exp->recordMetric("average_fitness", static_cast<double>(0.2 + (i * 0.02)));
        exp->recordMetric("generation", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runParticleSwarmOptimization(
    const ParticleSwarmConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running particle swarm optimization with particles: " + std::to_string(config.numParticles));
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.maxIterations; ++i) {
        auto exp = std::make_shared<Experiment>("particle_iteration_" + std::to_string(i));
        exp->recordMetric("best_position", static_cast<double>(i * 0.01));
        exp->recordMetric("best_value", static_cast<double>(0.6 - (i * 0.005)));
        exp->recordMetric("iteration", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runSimulatedAnnealing(
    const SimulatedAnnealingConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running simulated annealing with initial temperature: " + std::to_string(config.initialTemp));
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.maxIterations; ++i) {
        auto exp = std::make_shared<Experiment>("annealing_iteration_" + std::to_string(i));
        double temperature = config.initialTemp * std::pow(config.coolingRate, i);
        exp->recordMetric("temperature", temperature);
        exp->recordMetric("energy", static_cast<double>(0.8 - (i * 0.002)));
        exp->recordMetric("iteration", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runBayesianOptimization(
    const BayesianOptimizationConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running Bayesian optimization with acquisition function: " + config.acquisitionFunction);
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.numIterations; ++i) {
        auto exp = std::make_shared<Experiment>("bayesian_iteration_" + std::to_string(i));
        exp->recordMetric("expected_improvement", static_cast<double>(0.1 * std::exp(-i * 0.1)));
        exp->recordMetric("acquisition_value", static_cast<double>(0.9 - (i * 0.01)));
        exp->recordMetric("iteration", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runRandomSearch(const RandomSearchConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running random search with iterations: " + std::to_string(config.numIterations));
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.numIterations; ++i) {
        auto exp = std::make_shared<Experiment>("random_iteration_" + std::to_string(i));
        exp->recordMetric("objective_value", static_cast<double>(pImpl->rng() % 1000) / 1000.0);
        exp->recordMetric("iteration", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runGridSearch(const GridSearchConfig& config) {
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running grid search with parameter combinations: " + std::to_string(config.numCombinations));
    
    // For now, return placeholder results
    for (size_t i = 0; i < config.numCombinations; ++i) {
        auto exp = std::make_shared<Experiment>("grid_combination_" + std::to_string(i));
        exp->recordMetric("combination_index", static_cast<double>(i));
        results.push_back(exp);
    }
    
    return results;
}

size_t BatchProcessor::countSuccessfulExperiments(
    const std::vector<std::shared_ptr<Experiment>>& experiments) const {
    size_t count = 0;
    
    for (const auto& exp : experiments) {
        // Check if experiment has metrics (indicating successful execution)
        if (!exp->getMetrics().empty()) {
            count++;
        }
    }
    
    return count;
}

} // namespace nlm
