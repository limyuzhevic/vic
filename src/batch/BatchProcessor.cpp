// src/batch/BatchProcessor.cpp
#include "BatchProcessor.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../world/SimpleWorld.hpp"
#include <fstream>
#include <thread>
#include <future>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace nlm {

struct BatchProcessor::Impl {
    std::vector<std::shared_ptr<Config>> configurations;
    std::vector<std::shared_ptr<Experiment>> experiments;
    size_t maxConcurrent;
    bool verbose;
    
    Impl() : maxConcurrent(4), verbose(false) {}
};

BatchProcessor::BatchProcessor() {
    pImpl = std::make_unique<Impl>();
    verbose_ = false;
    maxConcurrent_ = 4;
}

BatchProcessor::~BatchProcessor() = default;

void BatchProcessor::addConfiguration(const std::shared_ptr<Config>& config, const std::string& name) {
    if (config) {
        if (!name.empty()) {
            // Add name as a metadata field
            config->set("_batch_name", name, ConfigSource::Runtime);
        }
        pImpl->configurations.push_back(config);
        if (verbose_) {
            Logger::getGlobal().info("Added configuration to batch: " + (name.empty() ? "unnamed" : name));
        }
    }
}

const std::vector<std::shared_ptr<Config>>& BatchProcessor::getConfigurations() const {
    return pImpl->configurations;
}

void BatchProcessor::clearConfigurations() {
    pImpl->configurations.clear();
    pImpl->experiments.clear();
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runBatchSimulation(
    const std::shared_ptr<Environment>& environmentTemplate,
    SimulationStep maxStepsPerSimulation,
    const std::function<void(std::shared_ptr<Brain>, SimulationStep)>& progressCallback) {
    
    pImpl->experiments.clear();
    
    if (pImpl->configurations.empty()) {
        Logger::getGlobal().warning("No configurations to run in batch simulation");
        return pImpl->experiments;
    }
    
    ExperimentRunner runner;
    
    // Cast to SimpleWorld for compatibility
    auto simpleWorld = std::dynamic_pointer_cast<SimpleWorld>(environmentTemplate);
    if (!simpleWorld) {
        Logger::getGlobal().warning("Environment template is not a SimpleWorld, using defaults");
        simpleWorld = std::make_shared<SimpleWorld>();
    }
    
    // Create environment copy for each simulation
    auto world = std::make_shared<SimpleWorld>(*simpleWorld);
    
    for (size_t i = 0; i < pImpl->configurations.size(); ++i) {
        auto config = pImpl->configurations[i];
        auto brain = std::make_shared<Brain>(config);
        
        if (verbose_) {
            Logger::getGlobal().info("Running batch simulation " + std::to_string(i + 1) + 
                                   " of " + std::to_string(pImpl->configurations.size()));
        }
        
        auto experiment = runner.createExperiment("batch_simulation_" + std::to_string(i + 1));
        experiment->setConfigValue("config_name", config->getOr<std::string>("_batch_name", "unnamed"));
        
        bool success = runner.runExperiment(experiment, brain, world, maxStepsPerSimulation);
        
        if (success) {
            pImpl->experiments.push_back(experiment);
        }
        
        if (progressCallback) {
            progressCallback(brain, static_cast<SimulationStep>(i + 1) * maxStepsPerSimulation);
        }
    }
    
    return pImpl->experiments;
}

std::shared_ptr<Config> BatchProcessor::runHyperparameterSearch(
    const std::vector<std::string>& hyperparameterNames,
    const std::vector<std::vector<ConfigValue>>& hyperparameterValues,
    const std::shared_ptr<Environment>& environmentTemplate,
    SimulationStep maxSteps,
    const std::function<void(double)>& progressCallback) {
    
    if (hyperparameterNames.empty() || hyperparameterValues.empty() || hyperparameterNames.size() != hyperparameterValues.size()) {
        Logger::getGlobal().error("Invalid hyperparameter search configuration");
        return nullptr;
    }
    
    // Generate all hyperparameter combinations
    std::vector<std::vector<ConfigValue>> combinations;
    generateHyperparameterCombinations(hyperparameterNames, hyperparameterValues, 0, std::vector<ConfigValue>(), combinations);
    
    double bestScore = -std::numeric_limits<double>::max();
    std::shared_ptr<Config> bestConfig;
    
    ExperimentRunner runner;
    
    // Cast to SimpleWorld for compatibility
    auto simpleWorld = std::dynamic_pointer_cast<SimpleWorld>(environmentTemplate);
    if (!simpleWorld) {
        simpleWorld = std::make_shared<SimpleWorld>();
    }
    
    for (size_t i = 0; i < combinations.size(); ++i) {
        auto config = std::make_shared<Config>();
        
        // Apply hyperparameter combination
        for (size_t j = 0; j < hyperparameterNames.size(); ++j) {
            config->set(hyperparameterNames[j], combinations[i][j], ConfigSource::Runtime);
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        if (verbose_) {
            Logger::getGlobal().info("Testing hyperparameter combination " + std::to_string(i + 1) + 
                                   " of " + std::to_string(combinations.size()));
        }
        
        auto experiment = runner.createExperiment("hyperparameter_" + std::to_string(i + 1));
        
        bool success = runner.runExperiment(experiment, brain, simpleWorld, maxSteps);
        
        if (success) {
            double score = computeExperimentScore(experiment);
            
            if (score > bestScore) {
                bestScore = score;
                bestConfig = config;
            }
        }
        
        if (progressCallback) {
            progressCallback(static_cast<double>(i + 1) / combinations.size());
        }
    }
    
    return bestConfig;
}

std::vector<std::shared_ptr<Experiment>> BatchProcessor::runCrossValidation(
    const std::vector<std::shared_ptr<Config>>& configs,
    const std::shared_ptr<Environment>& environmentTemplate,
    SimulationStep maxSteps,
    double trainTestRatio,
    const std::function<void(double)>& progressCallback) {
    
    if (configs.size() < 2) {
        Logger::getGlobal().warning("Need at least 2 configurations for cross-validation");
        return {};
    }
    
    std::vector<std::shared_ptr<Experiment>> results;
    size_t splitIndex = static_cast<size_t>(configs.size() * trainTestRatio);
    
    if (splitIndex < 2 || splitIndex >= configs.size()) {
        Logger::getGlobal().error("Invalid train/test split");
        return {};
    }
    
    ExperimentRunner runner;
    
    // Cast to SimpleWorld for compatibility
    auto simpleWorld = std::dynamic_pointer_cast<SimpleWorld>(environmentTemplate);
    if (!simpleWorld) {
        simpleWorld = std::make_shared<SimpleWorld>();
    }
    
    for (size_t i = 0; i < configs.size(); ++i) {
        // Create validation set (current config)
        std::vector<std::shared_ptr<Config>> validationSet = { configs[i] };
        
        // Create training set (all other configs)
        std::vector<std::shared_ptr<Config>> trainingSet;
        for (size_t j = 0; j < configs.size(); ++j) {
            if (j != i) {
                trainingSet.push_back(configs[j]);
            }
        }
        
        auto experiment = runner.createExperiment("cross_validation_" + std::to_string(i + 1));
        
        // Run validation on this fold
        bool success = runner.runExperiment(experiment, 
                                           std::make_shared<Brain>(configs[i]), 
                                           simpleWorld, 
                                           maxSteps);
        
        if (success) {
            results.push_back(experiment);
        }
        
        if (progressCallback) {
            progressCallback(static_cast<double>(i + 1) / configs.size());
        }
    }
    
    return results;
}

bool BatchProcessor::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Batch Processing Results\n";
    file << "========================\n\n";
    
    file << "Configurations: " << pImpl->configurations.size() << "\n";
    file << "Experiments: " << pImpl->experiments.size() << "\n\n";
    
    // Save configuration details
    file << "Configurations:\n";
    for (size_t i = 0; i < pImpl->configurations.size(); ++i) {
        file << "  Config " << i + 1 << ": ";
        auto name = pImpl->configurations[i]->getOr<std::string>("_batch_name", "unnamed");
        file << name << "\n";
    }
    
    // Save experiment results
    file << "\nExperiment Results:\n";
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        file << "  Experiment " << i + 1 << ": " << pImpl->experiments[i]->getName() << "\n";
    }
    
    return true;
}

bool BatchProcessor::loadResults(const std::string& filepath) {
    // Implementation would load from saved file
    // This is a placeholder for now
    return false;
}

std::string BatchProcessor::getStatistics() const {
    std::stringstream ss;
    ss << "Batch Processor Statistics\n";
    ss << "==========================\n";
    ss << "Total configurations: " << pImpl->configurations.size() << "\n";
    ss << "Total experiments: " << pImpl->experiments.size() << "\n";
    ss << "Maximum concurrent: " << maxConcurrent_ << "\n";
    ss << "Verbose output: " << (verbose_ ? "enabled" : "disabled") << "\n";
    return ss.str();
}

void BatchProcessor::generateHyperparameterCombinations(
    const std::vector<std::string>& names,
    const std::vector<std::vector<ConfigValue>>& values,
    size_t index,
    std::vector<ConfigValue> current,
    std::vector<std::vector<ConfigValue>>& result) {
    
    if (index >= names.size()) {
        result.push_back(current);
        return;
    }
    
    for (const auto& value : values[index]) {
        current.push_back(value);
        generateHyperparameterCombinations(names, values, index + 1, current, result);
        current.pop_back();
    }
}

double BatchProcessor::computeExperimentScore(const std::shared_ptr<Experiment>& experiment) const {
    if (!experiment) {
        return -std::numeric_limits<double>::max();
    }
    
    double score = 0.0;
    
    // Incorporate various metrics
    const auto& metrics = experiment->getMetrics();
    for (const auto& metric : metrics) {
        if (metric.first == "firing_rate") {
            score += metric.second * 0.4;  // 40% weight on firing rate
        } else if (metric.first == "spike_count") {
            score += metric.second * 0.3;  // 30% weight on spike count
        } else if (metric.first == "learning_rate") {
            score += metric.second * 0.3;  // 30% weight on learning rate
        }
    }
    
    return score;
}

} // namespace nlm