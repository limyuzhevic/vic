#include "ExperimentRunner.hpp"

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record data
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
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record data
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
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

// New method: Run batch with progress tracking and parallel support
std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps,
    bool enableParallel,
    size_t maxParallelWorkers) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    if (enableParallel && maxParallelWorkers > 1) {
        // Parallel batch processing
        std::vector<std::future<bool>> futures;
        
        for (size_t i = 0; i < experiments.size(); ++i) {
            auto exp = experiments[i];
            
            // Create async task for each experiment
            futures.emplace_back(std::async(std::launch::async, [this, exp, brainTemplate, environmentTemplate, maxSteps]() {
                // Create fresh brain and environment for each experiment
                auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
                brain->initialize();
                
                if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
                    return true;
                }
                return false;
            }));
        }
        
        // Collect results
        for (size_t i = 0; i < experiments.size(); ++i) {
            if (futures[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                if (futures[i].get()) {
                    results.push_back(experiments[i]);
                }
            }
        }
    } else {
        // Sequential batch processing
        for (auto& exp : experiments) {
            // Create fresh brain and environment for each experiment
            auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
            brain->initialize();
            
            // Would also create fresh environment here
            // For now, use template environment
            if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
                results.push_back(exp);
            }
        }
    }
    
    return results;
}

// New method: Run experiment with detailed progress reporting
bool ExperimentRunner::runExperimentWithProgress(
    std::shared_ptr<Experiment> experiment,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    SimulationStep maxSteps,
    std::function<void(float)> progressCallback) {
    
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop with progress tracking
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record data
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
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
        
        // Call progress callback
        if (progressCallback) {
            float progress = static_cast<float>(step) / static_cast<float>(maxSteps);
            progressCallback(progress);
        }
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

// New method: Run multiple experiments with configuration templates
std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runExperimentsWithConfigs(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    const std::vector<std::shared_ptr<Config>>& configs,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    if (experiments.size() != configs.size()) {
        // Mismatch between number of experiments and configs
        return results;
    }
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto exp = experiments[i];
        auto config = configs[i];
        
        // Create brain with specific configuration
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create environment with appropriate configuration
        auto environment = std::make_shared<Environment>(config);
        
        if (runExperiment(exp, brain, environment, maxSteps)) {
            results.push_back(exp);
        }
    }
    
    return results;
}

// New method: Get batch execution statistics
std::vector<ExperimentStats> ExperimentRunner::getBatchStatistics(
    const std::vector<std::shared_ptr<Experiment>>& experiments) const {
    
    std::vector<ExperimentStats> stats;
    
    for (const auto& exp : experiments) {
        ExperimentStats stat;
        stat.name = exp->getName();
        stat.totalSteps = exp->getEndStep() - exp->getStartStep();
        stat.neuronsAtStart = exp->getMetric("neurons_at_start");
        stat.neuronsAtEnd = exp->getMetric("neurons_at_end");
        stat.synapsesAtStart = exp->getMetric("synapses_at_start");
        stat.synapsesAtEnd = exp->getMetric("synapses_at_end");
        stat.firingAtStart = exp->getMetric("firing_at_start");
        stat.firingAtEnd = exp->getMetric("firing_at_end");
        stat.averageFiringRate = exp->getMetric("avg_firing_rate");
        stat.totalReward = exp->getMetric("total_reward");
        
        // Calculate duration
        if (stat.neuronsAtStart > 0 && stat.neuronsAtEnd > 0) {
            stat.plasticityRatio = (stat.neuronsAtEnd - stat.neuronsAtStart) / static_cast<float>(stat.neuronsAtStart);
        }
        
        stats.push_back(stat);
    }
    
    return stats;
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
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // TODO PHASE 2: Implement results saving
    return false;
}

} // namespace nlm
