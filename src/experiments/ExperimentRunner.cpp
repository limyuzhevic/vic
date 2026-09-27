#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <fstream>
#include <algorithm>
#include <sstream>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {
        // Initialize random number generator with seed
        rng = std::make_unique<RandomGenerator>(globalSeed);
    }
    
    std::unique_ptr<RandomGenerator> rng;
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    /*
     * Create a new experiment
     * 
     * The experiment is added to the collection and can be run later.
     * If an experiment with the same name already exists, returns nullptr.
     */
    // Check if experiment with this name already exists
    for (const auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            NLM_LOG_WARNING("Experiment with name '" + name + "' already exists");
            return nullptr;
        }
    }
    
    // Create new experiment (basic implementation)
    auto experiment = std::make_shared<Experiment>(name);
    experiment->setRunner(this);
    
    pImpl->experiments.push_back(experiment);
    NLM_LOG_INFO("Created new experiment: " + name);
    
    return experiment;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<Environment> environment,
                                   SimulationStep maxSteps) {
    /*
     * Run a single experiment
     * 
     * The experiment is run with the given brain and environment
     * for the specified maximum number of steps.
     * Returns true if the experiment completed successfully.
     */
    if (!experiment || !brain || !environment) {
        NLM_LOG_ERROR("Invalid parameters for experiment run");
        return false;
    }
    
    NLM_LOG_INFO("Running experiment: " + experiment->getName() + 
                 " for " + std::to_string(maxSteps) + " steps");
    
    // Reset brain and environment
    brain->reset();
    experiment->reset();
    
    // Set random seed for reproducibility
    pImpl->rng->setSeed(pImpl->globalSeed);
    brain->getRandomGenerator()->setSeed(pImpl->globalSeed);
    
    // Run simulation steps
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        // Update experiment state
        experiment->step(step);
        
        // Update environment
        environment->step(step);
        
        // Get sensory input from environment
        auto sensoryInput = environment->getSensoryInput();
        
        // Feed input to brain
        if (sensoryInput) {
            brain->receiveSensoryInput(*sensoryInput);
        }
        
        // Brain simulation step
        brain->step(step);
        
        // Generate action based on brain state
        auto action = brain->produceAction();
        
        // Apply action to environment
        if (action) {
            environment->applyAction(*action);
        }
        
        // Collect experiment data
        experiment->collectData(brain, environment, step);
        
        // Check for experiment termination conditions
        if (experiment->shouldTerminate(step)) {
            NLM_LOG_INFO("Experiment " + experiment->getName() + 
                         " terminated early at step " + std::to_string(step));
            break;
        }
    }
    
    // Finalize experiment results
    experiment->finalizeResults();
    
    NLM_LOG_INFO("Experiment " + experiment->getName() + " completed");
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    /*
     * Run a batch of experiments
     * 
     * Each experiment is run with its own copy of the brain and environment
     * (deep copies if supported). Returns results of all completed experiments.
     */
    std::vector<std::shared_ptr<Experiment>> completedExperiments;
    
    NLM_LOG_INFO("Running batch of " + std::to_string(experiments.size()) + " experiments");
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& experiment = experiments[i];
        
        if (!experiment) {
            NLM_LOG_WARNING("Skipping null experiment in batch");
            continue;
        }
        
        // Create deep copies of brain and environment for isolation
        auto brainCopy = createBrainCopy(brainTemplate.get());
        auto environmentCopy = createEnvironmentCopy(environmentTemplate.get());
        
        // Run experiment with copies
        bool success = runExperiment(experiment, brainCopy, environmentCopy, maxSteps);
        
        if (success) {
            completedExperiments.push_back(experiment);
        } else {
            NLM_LOG_ERROR("Batch experiment " + experiment->getName() + " failed");
        }
    }
    
    NLM_LOG_INFO("Batch completed: " + std::to_string(completedExperiments.size()) + "/" + 
                 std::to_string(experiments.size()) + " experiments succeeded");
    
    return completedExperiments;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    /*
     * Get experiment by name
     * 
     * Searches through all registered experiments and returns the one with
     * the given name, or nullptr if not found.
     */
    for (const auto& experiment : pImpl->experiments) {
        if (experiment->getName() == name) {
            return experiment;
        }
    }
    
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    /*
     * Clear all experiments
     * 
     * Removes all experiments from the runner and frees any allocated resources.
     */
    NLM_LOG_INFO("Clearing all experiments (" + std::to_string(pImpl->experiments.size()) + " experiments)");
    
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
    pImpl->rng->setSeed(seed);
    NLM_LOG_INFO("Set global seed to " + std::to_string(seed));
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    /*
     * Save experiment results
     * 
     * Writes all experiment results to the specified file.
     * Uses a simple text format for readability.
     */
    NLM_LOG_INFO("Saving experiment results to " + filepath);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << "=== NLM Experiment Results ===\n\n";
        
        for (const auto& experiment : pImpl->experiments) {
            file << "Experiment: " << experiment->getName() << "\n";
            file << "=============================\n";
            
            // Write experiment metadata
            file << "Metrics:\n";
            auto metrics = experiment->getMetrics();
            for (const auto& metric : metrics) {
                file << "  " << metric.name << ": " << metric.value << "\n";
            }
            
            file << "\n";
        }
        
        file << "Total experiments: " << pImpl->experiments.size() << "\n";
        file.close();
        
        NLM_LOG_INFO("Successfully saved experiment results");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving experiment results: ") + e.what());
        return false;
    }
}

// Helper method to create a copy of a brain
std::shared_ptr<Brain> ExperimentRunner::createBrainCopy(Brain* original) {
    if (!original) return nullptr;
    
    // For now, return nullptr - in a real implementation this would
    // properly deep copy the brain state. This is a placeholder.
    NLM_LOG_WARNING("Brain copying not implemented yet - using original brain");
    return std::shared_ptr<Brain>(original);
}

// Helper method to create a copy of an environment
std::shared_ptr<Environment> ExperimentRunner::createEnvironmentCopy(Environment* original) {
    if (!original) return nullptr;
    
    // For now, return nullptr - in a real implementation this would
    // properly deep copy the environment state. This is a placeholder.
    NLM_LOG_WARNING("Environment copying not implemented yet - using original environment");
    return std::shared_ptr<Environment>(original);
}

} // namespace nlm