#include "ExperimentRunner.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    mutable std::vector<std::string> lastErrors;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    NLM_LOG_INFO("Creating experiment: " + name);
    
    if (name.empty()) {
        NLM_LOG_ERROR("Cannot create experiment with empty name");
        return nullptr;
    }
    
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    
    NLM_LOG_INFO("Experiment created successfully: " + name + " (seed: " + std::to_string(exp->getSeed()) + ")");
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment) {
        pImpl->lastErrors.push_back("Experiment pointer is null");
        return false;
    }
    if (!brain) {
        pImpl->lastErrors.push_back("Brain pointer is null");
        return false;
    }
    if (!environment) {
        pImpl->lastErrors.push_back("Environment pointer is null");
        return false;
    }
    
    NLM_LOG_INFO("Running experiment: " + experiment->getName() + " (max steps: " + std::to_string(maxSteps) + ")");
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    if (!observation) {
        NLM_LOG_ERROR("Environment reset failed");
        return false;
    }
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop with progress logging
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
        if (!action) {
            NLM_LOG_WARNING("Brain produced null action at step " + std::to_string(step));
            continue;
        }
        
        // Environment step
        observation = environment->step(*action);
        if (!observation) {
            NLM_LOG_WARNING("Environment step returned null at step " + std::to_string(step));
            break; // Environment ended
        }
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
        
        // Log progress every 10% or every 1000 steps
        if (step % 100 == 0 || step % (maxSteps / 10) == 0) {
            NLM_LOG_INFO("Experiment " + experiment->getName() + 
                        " progress: step " + std::to_string(step) + 
                        "/" + std::to_string(maxSteps) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Reward: " + std::to_string(reward));
        }
    }
    
    experiment->setEndStep(maxSteps);
    
    NLM_LOG_INFO("Experiment completed: " + experiment->getName() + 
                " | Final reward: " + std::to_string(experiment->getMetric("total_reward")) +
                " | Final spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    if (experiments.empty()) {
        NLM_LOG_WARNING("Empty experiment list provided to runBatch");
        return {};
    }
    
    if (!brainTemplate) {
        NLM_LOG_ERROR("Brain template is null");
        return {};
    }
    
    if (!environmentTemplate) {
        NLM_LOG_ERROR("Environment template is null");
        return {};
    }
    
    NLM_LOG_INFO("Running batch experiment with " + std::to_string(experiments.size()) + 
                " experiments (max steps: " + std::to_string(maxSteps) + ")");
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& exp = experiments[i];
        NLM_LOG_INFO("Running experiment " + std::to_string(i + 1) + "/" + 
                    std::to_string(experiments.size()) + ": " + exp->getName());
        
        try {
            // Create fresh brain and environment for each experiment
            auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
            brain->initialize();
            
            // Create fresh environment (in real implementation, this would clone properly)
            auto environment = std::make_shared<Environment>(*environmentTemplate);
            environment->reset();
            
            if (runExperiment(exp, brain, environment, maxSteps)) {
                results.push_back(exp);
                NLM_LOG_INFO("Experiment completed successfully: " + exp->getName());
            } else {
                NLM_LOG_ERROR("Experiment failed: " + exp->getName());
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in experiment " + exp->getName() + ": " + std::string(e.what()));
        }
    }
    
    NLM_LOG_INFO("Batch experiment completed: " + std::to_string(results.size()) + 
                "/" + std::to_string(experiments.size()) + " experiments succeeded");
    
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
    NLM_LOG_INFO("Global seed set to: " + std::to_string(seed));
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    NLM_LOG_INFO("Saving experiment results to: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write results in a structured format
    file << "# NLM Experiment Results" << std::endl;
    file << "# Generated on " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    for (const auto& exp : pImpl->experiments) {
        file << "=== Experiment: " << exp->getName() << " ===" << std::endl;
        file << "Seed: " << exp->getSeed() << std::endl;
        file << "Start Step: " << exp->getStartStep() << std::endl;
        file << "End Step: " << exp->getEndStep() << std::endl;
        file << "Total Reward: " << exp->getMetric("total_reward") << std::endl;
        file << std::endl;
        
        // Write detailed step data if available
        if (exp->getStepCount() > 0) {
            file << "Steps:" << std::endl;
            for (SimulationStep step = 0; step < exp->getStepCount(); ++step) {
                auto& stepData = exp->getStepData(step);
                file << "  Step " << step << ": "
                     << "neurons=" << stepData.neuronCount
                     << " synapses=" << stepData.synapseCount
                     << " firing=" << stepData.firingNeuronCount
                     << " rate=" << stepData.firingRate << std::endl;
            }
        }
        file << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Results saved successfully to: " + filepath);
    return true;
}

// Static timestamp helper
std::string ExperimentRunner::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

const std::vector<std::string>& ExperimentRunner::getLastErrors() const {
    return pImpl->lastErrors;
}

void ExperimentRunner::clearLastErrors() {
    pImpl->lastErrors.clear();
}

} // namespace nlm
