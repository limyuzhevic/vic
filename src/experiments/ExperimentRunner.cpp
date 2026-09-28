#include "ExperimentRunner.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(std::make_unique<Impl>()) {}

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
    // Validate input parameters
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
    
    try {
        experiment->setStartStep(0);
        
        // Reset environment and brain to initial state
        auto observation = environment->reset();
        brain->receiveSensoryInput(*observation);
        
        // Run simulation loop with error handling
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
        NLM_LOG_INFO("Experiment completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running experiment: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    // Validate input parameters
    if (!brainTemplate) {
        NLM_LOG_ERROR("Brain template is null");
        return {};
    }
    if (!environmentTemplate) {
        NLM_LOG_ERROR("Environment template is null");
        return {};
    }
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        if (!exp) {
            NLM_LOG_WARNING("Skipping null experiment in batch");
            continue;
        }
        
        try {
            // Create fresh brain by cloning from template
            auto brain = cloneBrainFromTemplate(brainTemplate);
            if (!brain || !brain->initialize()) {
                NLM_LOG_ERROR("Failed to create fresh brain for experiment: " + exp->getName());
                continue;
            }
            
            // Run experiment
            if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
                results.push_back(exp);
                NLM_LOG_INFO("Successfully completed experiment: " + exp->getName());
            } else {
                NLM_LOG_ERROR("Experiment failed: " + exp->getName());
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception running experiment " + exp->getName() + ": " + std::string(e.what()));
        }
    }
    
    return results;
}

// Helper method to clone brain from template
std::shared_ptr<Brain> ExperimentRunner::cloneBrainFromTemplate(std::shared_ptr<Brain> templateBrain) {
    if (!templateBrain) {
        NLM_LOG_ERROR("Cannot clone from null brain template");
        return nullptr;
    }
    
    try {
        // Get the configuration from the template brain
        auto config = templateBrain->getConfig();
        
        // Create a new brain with the same configuration
        auto clonedBrain = std::make_shared<Brain>(config);
        
        // Initialize the cloned brain
        if (!clonedBrain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize cloned brain");
            return nullptr;
        }
        
        // Copy important state from template if needed
        // This is a simplified implementation - in practice, you might need
        // to copy more complex state like neuron weights, synapse connections, etc.
        
        NLM_LOG_INFO("Successfully cloned brain from template");
        return clonedBrain;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception cloning brain: " + std::string(e.what()));
        return nullptr;
    }
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp && exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
    NLM_LOG_INFO("Cleared all experiments");
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
    NLM_LOG_INFO("Set global seed to: " + std::to_string(seed));
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        // Write header
        file << "NLM Experiment Results" << std::endl;
        file << "=====================" << std::endl;
        file << "Generated on: " << std::chrono::system_clock::now() << std::endl;
        file << "Global Seed: " << pImpl->globalSeed << std::endl;
        file << std::endl;
        
        // Write experiment data
        file << "Experiments (" << pImpl->experiments.size() << "):" << std::endl;
        file << "------------" << std::endl;
        
        for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
            const auto& experiment = pImpl->experiments[i];
            if (!experiment) {
                continue;
            }
            
            file << "Experiment " << (i + 1) << ": " << experiment->getName() << std::endl;
            file << "  Seed: " << experiment->getSeed() << std::endl;
            file << "  Start Step: " << experiment->getStartStep() << std::endl;
            file << "  End Step: " << experiment->getEndStep() << std::endl;
            file << "  Duration: " << (experiment->getEndStep() - experiment->getStartStep()) << " steps" << std::endl;
            
            // Write metrics
            const auto& metrics = experiment->getMetrics();
            if (!metrics.empty()) {
                file << "  Metrics:" << std::endl;
                for (const auto& metric : metrics) {
                    file << "    " << metric.first << ": " << metric.second << std::endl;
                }
            }
            
            // Write step data summary
            const auto& stepData = experiment->getStepData();
            if (!stepData.empty()) {
                file << "  Step Data Summary:" << std::endl;
                file << "    Steps recorded: " << stepData.size() << std::endl;
                file << "    Average neurons per step: " << getAverageValue(stepData, 
                    [](const Experiment::StepData& sd) { return (double)sd.neuronCount; }) << std::endl;
                file << "    Average synapses per step: " << getAverageValue(stepData, 
                    [](const Experiment::StepData& sd) { return (double)sd.synapseCount; }) << std::endl;
                file << "    Average firing count per step: " << getAverageValue(stepData, 
                    [](const Experiment::StepData& sd) { return (double)sd.firingCount; }) << std::endl;
                file << "    Average firing rate per step: " << getAverageValue(stepData, 
                    [](const Experiment::StepData& sd) { return (double)sd.avgFiringRate; }) << std::endl;
            }
            
            file << "  ---" << std::endl;
        }
        
        file << std::endl;
        
        // Write summary statistics
        file << "Summary Statistics:" << std::endl;
        file << "----------------" << std::endl;
        file << "Total experiments: " << pImpl->experiments.size() << std::endl;
        
        size_t totalSteps = 0;
        float totalMetrics = 0.0f;
        size_t metricsCount = 0;
        
        for (const auto& experiment : pImpl->experiments) {
            if (!experiment) continue;
            
            totalSteps += (experiment->getEndStep() - experiment->getStartStep());
            
            const auto& metrics = experiment->getMetrics();
            for (const auto& metric : metrics) {
                totalMetrics += metric.second;
                metricsCount++;
            }
        }
        
        file << "Total simulation steps: " << totalSteps << std::endl;
        if (metricsCount > 0) {
            file << "Average metric value: " << (totalMetrics / metricsCount) << std::endl;
        }
        
        NLM_LOG_INFO("Successfully saved results to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving results: " + std::string(e.what()));
        return false;
    }
}

// Template function to calculate average value from step data
template<typename Func>
double ExperimentRunner::getAverageValue(const std::vector<Experiment::StepData>& stepData, Func extractor) const {
    if (stepData.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& step : stepData) {
        sum += extractor(step);
    }
    
    return sum / stepData.size();
}

} // namespace nlm
