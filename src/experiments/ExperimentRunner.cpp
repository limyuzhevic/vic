#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

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
    NLM_LOG_INFO("Saving experiment results to: " + filepath);
    
    // Create JSON structure for all experiments
    nlohmann::json results;
    
    // Add metadata
    results["metadata"]["version"] = "1.0";
    results["metadata"]["date"] = "2026-10-27";  // TODO: Add actual date
    results["metadata"]["num_experiments"] = pImpl->experiments.size();
    
    // Add each experiment's data
    nlohmann::json experiments_json = nlohmann::json::array();
    
    for (const auto& exp : pImpl->experiments) {
        nlohmann::json exp_json;
        exp_json["name"] = exp->getName();
        exp_json["seed"] = exp->getSeed();
        exp_json["start_step"] = exp->getStartStep();
        exp_json["end_step"] = exp->getEndStep();
        
        // Add metrics
        nlohmann::json metrics_json = nlohmann::json::object();
        const auto& expMetrics = exp->getMetrics();
        for (const auto& metric : expMetrics) {
            metrics_json[metric.first] = metric.second;
        }
        exp_json["metrics"] = metrics_json;
        
        // Add step data
        nlohmann::json steps_json = nlohmann::json::array();
        const auto& stepData = exp->getStepData();
        for (const auto& step : stepData) {
            nlohmann::json step_json;
            step_json["step"] = step.step;
            step_json["neurons"] = step.neuronCount;
            step_json["synapses"] = step.synapseCount;
            step_json["firing"] = step.firingCount;
            step_json["avg_firing_rate"] = step.avgFiringRate;
            steps_json.push_back(step_json);
        }
        exp_json["steps"] = steps_json;
        
        // Add notes
        nlohmann::json notes_json = nlohmann::json::array();
        const auto& notes = exp->getNotes();
        for (const auto& note : notes) {
            notes_json.push_back(note);
        }
        exp_json["notes"] = notes_json;
        
        experiments_json.push_back(exp_json);
    }
    
    results["experiments"] = experiments_json;
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write with indentation for readability
    file << std::setw(2) << results;
    file.close();
    
    NLM_LOG_INFO("Experiment results saved successfully to: " + filepath);
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->experiments.size()) + " experiments");
    
    return true;
}

} // namespace nlm
