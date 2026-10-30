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

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".csv") {
        return saveResultsCSV(filepath);
    } else if (ext == ".json") {
        return saveResultsJSON(filepath);
    } else {
        // Default to simple text format
        return saveResultsText(filepath);
    }
}

// CSV results saving implementation
bool ExperimentRunner::saveResultsCSV(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write CSV header
    file << "Experiment Name,Seed,Start Step,End Step,Total Neurons,Total Synapses,Firing Neurons,Average Firing Rate,Total Reward,Metric Count\n";
    
    // Write experiment data
    for (const auto& exp : pImpl->experiments) {
        file << exp->getName() << ",";
        file << exp->getSeed() << ",";
        file << exp->getStartStep() << ",";
        file << exp->getEndStep() << ",";
        
        // Get metrics
        size_t totalNeurons = exp->getMetric("total_neurons");
        size_t totalSynapses = exp->getMetric("total_synapses");
        size_t firingNeurons = exp->getMetric("firing_neurons");
        float avgFiringRate = exp->getMetric("average_firing_rate");
        float totalReward = exp->getMetric("total_reward");
        
        file << totalNeurons << ",";
        file << totalSynapses << ",";
        file << firingNeurons << ",";
        file << avgFiringRate << ",";
        file << totalReward << ",";
        file << exp->getMetricCount() << "\n";
    }
    
    return true;
}

// JSON results saving implementation
bool ExperimentRunner::saveResultsJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"experiments\": [\n";
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& exp = pImpl->experiments[i];
        
        file << "    {\n";
        file << "      \"name\": \"" << exp->getName() << "\",\n";
        file << "      \"seed\": " << exp->getSeed() << ",\n";
        file << "      \"start_step\": " << exp->getStartStep() << ",\n";
        file << "      \"end_step\": " << exp->getEndStep() << ",\n";
        
        // Get metrics
        file << "      \"metrics\": {\n";
        for (size_t j = 0; j < exp->getMetricCount(); ++j) {
            const auto& metricName = exp->getMetricName(j);
            double metricValue = exp->getMetric(metricName);
            
            file << "        \"" << metricName << "\": " << metricValue;
            if (j < exp->getMetricCount() - 1) {
                file << ",";
            }
            file << "\n";
        }
        file << "      }\n";
        
        if (i < pImpl->experiments.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"global_seed\": " << pImpl->globalSeed << "\n";
    file << "}\n";
    
    return true;
}

// Text results saving implementation
bool ExperimentRunner::saveResultsText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Experiment Results\n";
    file << "======================\n\n";
    
    file << "Total Experiments: " << pImpl->experiments.size() << "\n";
    file << "Global Seed: " << pImpl->globalSeed << "\n\n";
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& exp = pImpl->experiments[i];
        
        file << "Experiment " << (i + 1) << ":\n";
        file << "  Name: " << exp->getName() << "\n";
        file << "  Seed: " << exp->getSeed() << "\n";
        file << "  Steps: " << exp->getStartStep() << " - " << exp->getEndStep() << "\n";
        file << "  Metrics Count: " << exp->getMetricCount() << "\n";
        
        file << "  Metric Details:\n";
        for (size_t j = 0; j < exp->getMetricCount(); ++j) {
            const auto& metricName = exp->getMetricName(j);
            double metricValue = exp->getMetric(metricName);
            file << "    " << metricName << ": " << metricValue << "\n";
        }
        
        if (i < pImpl->experiments.size() - 1) {
            file << "\n";
        }
    }
    
    return true;
}

} // namespace nlm