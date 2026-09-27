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
        // Create fresh brain from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Create fresh environment from template if available
        auto environment = environmentTemplate ? 
            environmentTemplate : std::make_shared<Environment>();
        environment->initialize();
        
        if (runExperiment(exp, brain, environment, maxSteps)) {
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
    // Save experiment results to JSON file
    try {
        nlohmann::json j;
        
        // Convert experiments to JSON
        for (const auto& exp : pImpl->experiments) {
            nlohmann::json expJson;
            expJson["name"] = exp->getName();
            expJson["startStep"] = exp->getStartStep();
            expJson["endStep"] = exp->getEndStep();
            expJson["totalReward"] = exp->getMetric("total_reward");
            
            // Add all metrics
            for (const auto& metricName : exp->getMetricNames()) {
                if (metricName != "total_reward") {
                    expJson["metrics"][metricName] = exp->getMetric(metricName);
                }
            }
            
            j["experiments"].push_back(expJson);
        }
        
        // Write JSON to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2); // Pretty print with 2-space indentation
        return true;
        
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

} // namespace nlm
