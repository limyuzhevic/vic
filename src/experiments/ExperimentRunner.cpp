#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    std::string resultsDirectory;
    
    Impl() : globalSeed(42), resultsDirectory("./experiment_results") {}
    
    bool ensureDirectoryExists(const std::string& path) {
        try {
            return std::filesystem::create_directories(path);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to create directory " + path + ": " + e.what());
            return false;
        }
    }
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {
    // Ensure results directory exists
    pImpl->ensureDirectoryExists(pImpl->resultsDirectory);
}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    NLM_LOG_INFO("Created experiment: " + name + " (seed: " + std::to_string(exp->getSeed()) + ")");
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<Environment> environment,
                                   SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        NLM_LOG_ERROR("Invalid experiment, brain, or environment");
        return false;
    }
    
    NLM_LOG_INFO("Running experiment: " + experiment->getName() + " (seed: " + std::to_string(experiment->getSeed()) + ")");
    
    // Record start step
    experiment->setStartStep(0);
    
    // Reset environment and brain for clean experiment run
    auto observation = environment->reset();
    if (observation) {
        brain->receiveSensoryInput(*observation);
    }
    
    // Run simulation loop with detailed metrics collection
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record comprehensive metrics
        experiment->recordStep(
            step,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        if (observation) {
            brain->receiveSensoryInput(*observation);
        }
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", experiment->getMetric("total_reward") + reward);
        
        // Record additional metrics every 100 steps
        if (step % 100 == 0) {
            experiment->recordMetric("avg_firing_rate_step_" + std::to_string(step), 
                                   brain->getAverageFiringRate());
            experiment->recordMetric("active_neurons_step_" + std::to_string(step), 
                                   brain->getActiveNeuronCount());
            experiment->recordMetric("total_spikes_step_" + std::to_string(step), 
                                   brain->getTotalSpikeCount());
        }
    }
    
    experiment->setEndStep(maxSteps);
    
    NLM_LOG_INFO("Experiment " + experiment->getName() + " completed in " + std::to_string(maxSteps) + " steps");
    NLM_LOG_INFO("  Total reward: " + std::to_string(experiment->getMetric("total_reward")));
    NLM_LOG_INFO("  Final firing rate: " + std::to_string(experiment->getStepData().back().avgFiringRate));
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running batch of " + std::to_string(experiments.size()) + " experiments");
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& exp = experiments[i];
        
        // Create fresh brain clone from template
        auto brain = cloneBrain(brainTemplate, exp->getSeed());
        if (!brain) {
            NLM_LOG_ERROR("Failed to clone brain for experiment " + exp->getName());
            continue;
        }
        
        // Initialize the cloned brain
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize cloned brain for experiment " + exp->getName());
            continue;
        }
        
        // Create fresh environment clone from template
        auto environment = cloneEnvironment(environmentTemplate, exp->getSeed());
        if (!environment) {
            NLM_LOG_ERROR("Failed to clone environment for experiment " + exp->getName());
            continue;
        }
        
        // Initialize the cloned environment
        environment->initialize();
        
        // Run the experiment
        if (runExperiment(exp, brain, environment, maxSteps)) {
            results.push_back(exp);
            NLM_LOG_INFO("Successfully completed experiment " + exp->getName() + 
                        " (" + std::to_string(i + 1) + "/" + std::to_string(experiments.size()) + ")");
        } else {
            NLM_LOG_ERROR("Failed to complete experiment " + exp->getName());
        }
    }
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    NLM_LOG_INFO("Clearing all experiments");
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
    NLM_LOG_INFO("Set global experiment seed to " + std::to_string(seed));
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    if (pImpl->experiments.empty()) {
        NLM_LOG_ERROR("No experiments to save");
        return false;
    }
    
    // Create JSON representation of all experiments
    nlohmann::json jsonData;
    
    for (const auto& exp : pImpl->experiments) {
        nlohmann::json expJson;
        
        // Basic experiment info
        expJson["name"] = exp->getName();
        expJson["seed"] = exp->getSeed();
        expJson["start_step"] = exp->getStartStep();
        expJson["end_step"] = exp->getEndStep();
        
        // Config values
        nlohmann::json configJson;
        for (const auto& pair : exp->getConfig()) {
            configJson[pair.first] = pair.second;
        }
        expJson["config"] = configJson;
        
        // Metrics
        nlohmann::json metricsJson;
        for (const auto& pair : exp->getMetrics()) {
            metricsJson[pair.first] = pair.second;
        }
        expJson["metrics"] = metricsJson;
        
        // Step data
        nlohmann::json stepsJson = nlohmann::json::array();
        for (const auto& stepData : exp->getStepData()) {
            nlohmann::json stepJson;
            stepJson["step"] = stepData.step;
            stepJson["neuron_count"] = stepData.neuronCount;
            stepJson["synapse_count"] = stepData.synapseCount;
            stepJson["firing_count"] = stepData.firingCount;
            stepJson["avg_firing_rate"] = stepData.avgFiringRate;
            stepsJson.push_back(stepJson);
        }
        expJson["steps"] = stepsJson;
        
        // Notes
        nlohmann::json notesJson = nlohmann::json::array();
        for (const auto& note : exp->getNotes()) {
            notesJson.push_back(note);
        }
        expJson["notes"] = notesJson;
        
        jsonData.push_back(expJson);
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write in pretty JSON format
    file << jsonData.dump(2);
    
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->experiments.size()) + 
                " experiments to " + filepath);
    
    return true;
}

// Helper functions for cloning
std::shared_ptr<Brain> ExperimentRunner::cloneBrain(std::shared_ptr<Brain> brainTemplate, uint64_t seed) {
    if (!brainTemplate) {
        return nullptr;
    }
    
    // Create new brain with same configuration
    auto config = std::make_shared<Config>(*brainTemplate->getConfig());
    
    // Override seed for this experiment
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize the brain (this creates neurons, synapses, etc.)
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize cloned brain");
        return nullptr;
    }
    
    return brain;
}

std::shared_ptr<Environment> ExperimentRunner::cloneEnvironment(std::shared_ptr<Environment> envTemplate, uint64_t seed) {
    if (!envTemplate) {
        return nullptr;
    }
    
    // Create new environment with same settings
    auto environment = std::make_shared<Environment>(*envTemplate->getConfig());
    
    // Initialize the environment
    if (!environment->initialize()) {
        NLM_LOG_ERROR("Failed to initialize cloned environment");
        return nullptr;
    }
    
    return environment;
}

} // namespace nlm