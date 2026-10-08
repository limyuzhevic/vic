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
    // Create results structure for all experiments
    nlohmann::json results;
    results["experiments"] = nlohmann::json::array();
    
    for (const auto& experiment : pImpl->experiments) {
        nlohmann::json expData;
        expData["name"] = experiment->getName();
        expData["seed"] = experiment->getSeed();
        expData["startStep"] = experiment->getStartStep();
        expData["endStep"] = experiment->getEndStep();
        
        // Add step data
        nlohmann::json steps = nlohmann::json::array();
        for (const auto& step : experiment->getStepData()) {
            nlohmann::json stepData;
            stepData["step"] = step.step;
            stepData["neuronCount"] = step.neuronCount;
            stepData["synapseCount"] = step.synapseCount;
            stepData["firingCount"] = step.firingCount;
            stepData["avgFiringRate"] = step.avgFiringRate;
            steps.push_back(stepData);
        }
        expData["steps"] = steps;
        
        // Add metrics
        nlohmann::json metrics = nlohmann::json::array();
        for (const auto& metric : experiment->getMetrics()) {
            nlohmann::json metricData;
            metricData["name"] = metric.first;
            metricData["value"] = metric.second;
            metrics.push_back(metricData);
        }
        expData["metrics"] = metrics;
        
        // Add notes
        nlohmann::json notes = nlohmann::json::array();
        for (const auto& note : experiment->getNotes()) {
            notes.push_back(note);
        }
        expData["notes"] = notes;
        
        results["experiments"].push_back(expData);
    }
    
    // Write results to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << results.dump(2);
    return true;
}

} // namespace nlm
