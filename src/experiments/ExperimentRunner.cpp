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
        // Log error for null pointer
        if (!experiment) {
            // TODO: Add proper logging instead of returning silent failure
        } else if (!brain) {
            // TODO: Add proper logging instead of returning silent failure
        } else if (!environment) {
            // TODO: Add proper logging instead of returning silent failure
        }
        return false;
    }
    
    try {
        experiment->setStartStep(0);
        
        // Reset environment and brain with error checking
        auto observation = environment->reset();
        if (!observation) {
            // Environment reset failed
            experiment->recordMetric("error", 1.0f);
            return false;
        }
        
        brain->receiveSensoryInput(*observation);
        
        // Run simulation loop with error handling
        for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
            // Brain step with error checking
            if (!brain->step(step)) {
                experiment->recordMetric("step_error", 1.0f);
                break;
            }
            
            // Record data with bounds checking
            try {
                experiment->recordStep(
                    step,
                    brain->getTotalNeuronCount(),
                    brain->getTotalSynapseCount(),
                    brain->getFiringNeuronCount(),
                    brain->getAverageFiringRate()
                );
            } catch (const std::exception& e) {
                // Log recording error
                experiment->recordMetric("recording_error", 1.0f);
            }
            
            // Get action from brain with error checking
            auto action = brain->produceAction();
            if (!action) {
                // No action produced, treat as error
                experiment->recordMetric("no_action", 1.0f);
                break;
            }
            
            // Environment step with error checking
            observation = environment->step(*action);
            if (!observation) {
                // Environment step failed
                experiment->recordMetric("environment_step_error", 1.0f);
                break;
            }
            
            // Send observation to brain with error checking
            brain->receiveSensoryInput(*observation);
            
            // Record reward with bounds checking
            try {
                float reward = environment->getLastReward();
                experiment->recordMetric("total_reward", 
                    experiment->getMetric("total_reward") + reward);
            } catch (const std::exception& e) {
                // Log reward recording error
                experiment->recordMetric("reward_error", 1.0f);
            }
        }
        
        experiment->setEndStep(maxSteps);
        return true;
        
    } catch (const std::exception& e) {
        // Catch any unexpected exceptions
        // TODO: Add proper logging of exception
        if (experiment) {
            experiment->recordMetric("exception", 1.0f);
        }
        return false;
    }
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    if (experiments.empty()) {
        return {};
    }
    
    if (!brainTemplate || !environmentTemplate) {
        // Log error for null template
        // TODO: Add proper logging instead of returning empty
        return {};
    }
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        if (!exp) {
            // Skip null experiment pointer
            continue;
        }
        
        try {
            // Create fresh brain and environment for each experiment
            auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
            if (!brain) {
                // Failed to create brain
                exp->recordMetric("brain_creation_error", 1.0f);
                continue;
            }
            
            if (!brain->initialize()) {
                // Brain initialization failed
                exp->recordMetric("brain_init_error", 1.0f);
                continue;
            }
            
            // Would also create fresh environment here
            // TODO: Implement proper environment cloning
            
            if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
                results.push_back(exp);
            }
        } catch (const std::exception& e) {
            // Catch any unexpected exceptions during batch execution
            if (exp) {
                exp->recordMetric("batch_exception", 1.0f);
                // TODO: Add proper logging of exception
            }
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
