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

// Save experiment results to file
    // Implements CSV and JSON output formats for analysis
    bool ExperimentRunner::saveResults(const std::string& filepath) const {
        if (filepath.empty()) {
            NLM_LOG_ERROR("Cannot save results: empty file path");
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        // Write header
        file << "experiment_name,start_step,end_step,total_reward,total_steps,avg_firing_rate,total_neurons,total_synapses\n";
        
        // Write experiment data
        bool allSuccessful = true;
        for (const auto& exp : pImpl->experiments) {
            std::string name = exp->getName();
            SimulationStep startStep = exp->getStartStep();
            SimulationStep endStep = exp->getEndStep();
            float totalReward = 0.0f;
            SimulationStep totalSteps = endStep - startStep;
            float avgFiringRate = exp->getAverageFiringRate();
            size_t totalNeurons = exp->getTotalNeuronCount();
            size_t totalSynapses = exp->getTotalSynapseCount();
            
            // Extract metrics from experiment
            // In a real implementation, you'd have proper getter methods
            if (exp->hasMetric("total_reward")) {
                totalReward = exp->getMetric("total_reward");
            }
            
            file << name << "," << startStep << "," << endStep << ","
                 << totalReward << "," << totalSteps << "," 
                 << avgFiringRate << "," << totalNeurons << "," << totalSynapses << "\n";
        }
        
        file.close();
        
        NLM_LOG_INFO("Successfully saved experiment results to: " + filepath);
        return true;
    }
    
    bool ExperimentRunner::saveResultsJSON(const std::string& filepath) const {
        if (filepath.empty()) {
            NLM_LOG_ERROR("Cannot save results: empty file path");
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << "{\n";
        file << "  \"experiments\": [\n";
        
        for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
            const auto& exp = pImpl->experiments[i];
            file << "    {\n";
            file << "      \"name\": \"" << exp->getName() << "\",\n";
            file << "      \"start_step\": " << exp->getStartStep() << ",\n";
            file << "      \"end_step\": " << exp->getEndStep() << ",\n";
            
            float totalReward = 0.0f;
            if (exp->hasMetric("total_reward")) {
                totalReward = exp->getMetric("total_reward");
            }
            file << "      \"total_reward\": " << totalReward << ",\n";
            
            file << "      \"total_steps\": " << (exp->getEndStep() - exp->getStartStep()) << ",\n";
            file << "      \"avg_firing_rate\": " << exp->getAverageFiringRate() << ",\n";
            file << "      \"total_neurons\": " << exp->getTotalNeuronCount() << ",\n";
            file << "      \"total_synapses\": " << exp->getTotalSynapseCount();
            
            if (i < pImpl->experiments.size() - 1) {
                file << ",\n";
            }
            file << "\n    }";
        }
        
        file << "\n  ],\n";
        file << "  \"global_seed\": " << pImpl->globalSeed << "\n";
        file << "}\n";
        
        file.close();
        
        NLM_LOG_INFO("Successfully saved experiment results (JSON) to: " + filepath);
        return true;
    }

} // namespace nlm
