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
                                     SimulationStep maxSteps,
                                     std::function<void(float)> progressCallback) {
        if (!experiment || !brain || !environment) {
            return false;
        }
        
        experiment->setStartStep(0);
        
        // Reset environment and brain
        auto observation = environment->reset();
        brain->receiveSensoryInput(*observation);
        
        // Run simulation loop
        size_t totalSteps = static_cast<size_t>(maxSteps);
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
            
            // Report progress
            if (progressCallback) {
                float progress = static_cast<float>(step + 1) / totalSteps;
                progressCallback(progress);
            }
        }
        
        experiment->setEndStep(maxSteps);
        return true;
    }

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps,
    std::function<void(float)> progressCallback) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    size_t totalExperiments = experiments.size();
    size_t completedExperiments = 0;
    
    for (size_t i = 0; i < totalExperiments; ++i) {
        auto& exp = experiments[i];
        
        // Create fresh brain and environment for each experiment
        // Clone from template to ensure independent experiments
        
        // Clone brain using the new clone() method
        auto clonedBrain = brainTemplate->clone();
        if (!clonedBrain) {
            NLM_LOG_ERROR("Failed to clone brain template");
            continue;
        }
        clonedBrain->initialize();
        
        // Clone environment using the new clone() method
        auto clonedEnvironment = environmentTemplate->clone();
        if (!clonedEnvironment) {
            NLM_LOG_ERROR("Failed to clone environment template");
            continue;
        }
        
        // Run the experiment
        if (runExperiment(exp, clonedBrain, clonedEnvironment, maxSteps, 
                         [this, &progressCallback, i, totalExperiments](float expProgress) {
                            // Calculate overall progress for batch
                            float overallProgress = 
                                (static_cast<float>(i) / totalExperiments) + 
                                (expProgress / totalExperiments);
                            if (progressCallback) {
                                progressCallback(overallProgress);
                            }
                         })) {
            results.push_back(exp);
            ++completedExperiments;
        }
        
        // Report progress for batch
        if (progressCallback) {
            float batchProgress = static_cast<float>(completedExperiments) / totalExperiments;
            progressCallback(batchProgress);
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
    NLM_LOG_INFO("Saving experiment results to " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        // Write header
        file << "NLM Experiment Results" << std::endl;
        file << "=======================" << std::endl;
        file << std::endl;
        
        // Write experiment information
        file << "Experiments: " << pImpl->experiments.size() << std::endl;
        file << std::endl;
        
        // Write each experiment's data
        for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
            const auto& exp = pImpl->experiments[i];
            file << "Experiment " << i << ": " << exp->getName() << std::endl;
            file << "  Seed: " << exp->getSeed() << std::endl;
            file << "  Start Step: " << exp->getStartStep() << std::endl;
            file << "  End Step: " << exp->getEndStep() << std::endl;
            
            // Write metrics
            file << "  Metrics:" << std::endl;
            const auto& metrics = exp->getMetrics();
            for (const auto& metric : metrics) {
                file << "    " << metric.first << ": " << metric.second << std::endl;
            }
            
            // Write step data
            file << "  Step Data:" << std::endl;
            const auto& stepData = exp->getStepData();
            file << "    Steps: " << stepData.size() << std::endl;
            file << "    Step  NeuronCount  SynapseCount  FiringCount  AvgFiringRate" << std::endl;
            
            for (size_t j = 0; j < stepData.size(); ++j) {
                const auto& step = stepData[j];
                file << "    " << std::setw(4) << step.step
                     << "     " << std::setw(11) << step.neuronCount
                     << "     " << std::setw(12) << step.synapseCount
                     << "     " << std::setw(12) << step.firingCount
                     << "     " << std::setw(13) << step.avgFiringRate << std::endl;
            }
            
            // Write notes
            const auto& notes = exp->getNotes();
            if (!notes.empty()) {
                file << "  Notes:" << std::endl;
                for (const auto& note : notes) {
                    file << "    " << note << std::endl;
                }
            }
            
            file << std::endl;
        }
        
        NLM_LOG_INFO("Successfully saved " + std::to_string(pImpl->experiments.size()) + 
                    " experiments to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving results: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
