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

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Experiment name cannot be empty");
    }
    
    auto exp = std::make_shared<Experiment>(name);
    if (!exp) {
        throw std::runtime_error("Failed to allocate experiment");
    }
    
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    // Comprehensive validation
    if (!experiment) {
        NLM_LOG_ERROR("Experiment is null");
        return false;
    }
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    if (!environment) {
        NLM_LOG_ERROR("Environment is null");
        return false;
    }
    
    if (maxSteps <= 0) {
        NLM_LOG_ERROR("Invalid maxSteps: " + std::to_string(maxSteps));
        return false;
    }
    
    NLM_LOG_INFO("Running experiment: " + experiment->getName() + " for " + 
                 std::to_string(maxSteps) + " steps");
    
    experiment->setStartStep(0);
    
    // Reset environment and brain with error handling
    auto observation = environment->reset();
    if (!observation) {
        NLM_LOG_ERROR("Environment failed to reset");
        return false;
    }
    
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop with error handling
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        try {
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
            if (!action) {
                NLM_LOG_WARNING("Brain produced null action at step " + std::to_string(step));
                continue;
            }
            
            // Environment step
            observation = environment->step(*action);
            if (!observation) {
                NLM_LOG_ERROR("Environment step returned null observation at step " + std::to_string(step));
                break;
            }
            
            // Send observation to brain
            brain->receiveSensoryInput(*observation);
            
            // Record reward
            float reward = environment->getLastReward();
            experiment->recordMetric("total_reward", 
                experiment->getMetric("total_reward") + reward);
                
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception at step " + std::to_string(step) + ": " + std::string(e.what()));
            break;
        } catch (...) {
            NLM_LOG_ERROR("Unknown exception at step " + std::to_string(step));
            break;
        }
    }
    
    experiment->setEndStep(maxSteps);
    NLM_LOG_INFO("Experiment " + experiment->getName() + " completed successfully");
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        // PLACEHOLDER: In real implementation, would clone from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Would also create fresh environment here
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
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
