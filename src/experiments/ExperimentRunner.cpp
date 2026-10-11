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
    // Implement results saving using JSON format
    #ifdef __has_include
    #if __has_include(<nlohmann/json.hpp>)
    #include <nlohmann/json.hpp>
    #include <fstream>
    #include <iomanip>
    
    try {
        nlohmann::json resultsJson;
        
        // Add metadata
        resultsJson["metadata"]["format"] = "NLM Experiment Results";
        resultsJson["metadata"]["version"] = "Phase 2";
        resultsJson["metadata"]["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        // Add experiment data
        resultsJson["experiments"] = nlohmann::json::array();
        for (const auto& experiment : pImpl->experiments) {
            nlohmann::json expJson;
            expJson["name"] = experiment->getName();
            expJson["seed"] = experiment->getSeed();
            expJson["startStep"] = experiment->getStartStep();
            expJson["endStep"] = experiment->getEndStep();
            
            // Add step data
            expJson["stepData"] = nlohmann::json::array();
            for (const auto& step : experiment->getStepData()) {
                nlohmann::json stepJson;
                stepJson["step"] = step.step;
                stepJson["neuronCount"] = step.neuronCount;
                stepJson["synapseCount"] = step.synapseCount;
                stepJson["firingCount"] = step.firingCount;
                stepJson["avgFiringRate"] = step.avgFiringRate;
                expJson["stepData"].push_back(stepJson);
            }
            
            // Add metrics
            expJson["metrics"] = nlohmann::json::array();
            for (const auto& metric : experiment->getMetrics()) {
                nlohmann::json metricJson;
                metricJson["name"] = metric.first;
                metricJson["value"] = metric.second;
                expJson["metrics"].push_back(metricJson);
            }
            
            // Add notes
            expJson["notes"] = experiment->getNotes();
            
            resultsJson["experiments"].push_back(expJson);
        }
        
        // Write to file with pretty formatting
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << resultsJson << std::endl;
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        // TODO: Add proper logging for error
        return false;
    }
    
    #else
    // nlohmann/json not available, create simple text format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Experiment Results (Phase 2)\n";
    file << "=============================\n\n";
    
    for (const auto& experiment : pImpl->experiments) {
        file << "Experiment: " << experiment->getName() << "\n";
        file << "Seed: " << experiment->getSeed() << "\n";
        file << "Duration: " << (experiment->getEndStep() - experiment->getStartStep()) << " steps\n";
        file << "Metrics:\n";
        
        for (const auto& metric : experiment->getMetrics()) {
            file << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        file << "\n";
    }
    
    file.close();
    return true;
    
    #endif
    
    return false;
}

} // namespace nlm
