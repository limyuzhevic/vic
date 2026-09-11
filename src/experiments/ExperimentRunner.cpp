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
        auto brain = cloneBrain(brainTemplate);
        auto environment = cloneEnvironment(environmentTemplate);
        
        if (runExperiment(exp, brain, environment, maxSteps)) {
            results.push_back(exp);
        }
    }
    
    return results;
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // TODO: Implement comprehensive results saving with JSON format
    return saveExperimentResultsToFile(filepath);
}

// Helper function implementations

// Clone brain helper function
std::shared_ptr<Brain> ExperimentRunner::cloneBrain(std::shared_ptr<Brain> brainTemplate) {
    if (!brainTemplate) {
        return nullptr;
    }
    
    // Create a new brain with the same configuration
    auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
    brain->initialize();
    
    // Try to load from checkpoint if available
    std::string checkpointFile = "brain_checkpoint_" + std::to_string(pImpl->globalSeed) + ".bin";
    if (brain->load(checkpointFile)) {
        NLM_LOG_INFO("Successfully loaded brain checkpoint: " + checkpointFile);
    }
    
    return brain;
}

// Clone environment helper function
std::shared_ptr<Environment> ExperimentRunner::cloneEnvironment(std::shared_ptr<Environment> environmentTemplate) {
    if (!environmentTemplate) {
        return nullptr;
    }
    
    // Create a fresh SimpleWorld environment
    return std::make_shared<SimpleWorld>();
}

// Save experiment results to file (comprehensive implementation)
bool ExperimentRunner::saveExperimentResultsToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty filepath provided for saving experiment results");
        return false;
    }
    
    try {
        nlohmann::json results;
        
        // Add metadata
        results["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        results["version"] = "1.0.0";
        
        // Add all experiments
        results["experiments"] = nlohmann::json::array();
        for (const auto& exp : pImpl->experiments) {
            nlohmann::json expData;
            expData["name"] = exp->getName();
            expData["seed"] = exp->getSeed();
            expData["start_step"] = exp->getStartStep();
            expData["end_step"] = exp->getEndStep();
            
            // Add metrics
            auto metrics = exp->getMetrics();
            expData["metrics"] = nlohmann::json::object();
            for (const auto& metric : metrics) {
                expData["metrics"][metric.first] = metric.second;
            }
            
            // Add step data summary
            auto stepData = exp->getStepData();
            expData["steps_count"] = stepData.size();
            if (!stepData.empty()) {
                expData["final_neuron_count"] = stepData.back().neuronCount;
                expData["final_synapse_count"] = stepData.back().synapseCount;
                expData["final_firing_count"] = stepData.back().firingCount;
                expData["final_avg_firing_rate"] = stepData.back().avgFiringRate;
            }
            
            results["experiments"].push_back(expData);
        }
        
        // Add system information
        results["system_info"] = nlohmann::json::object();
        results["system_info"]["platform"] = "Linux";
        results["system_info"]["compiler"] = "GCC";
        results["system_info"]["cpp_standard"] = "C++20";
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << results.dump(2);
        file.close();
        
        NLM_LOG_INFO("Successfully saved " + std::to_string(pImpl->experiments.size()) + 
                    " experiments to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving experiment results: ") + e.what());
        return false;
    }
}
