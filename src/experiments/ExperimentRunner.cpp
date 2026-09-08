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
    // Real results saving with JSON format for interoperability
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write results in JSON-like format
    file << "{\n";
    file << "  \"experiments\": [\n";
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& experiment = pImpl->experiments[i];
        file << "    {\n";
        file << "      \"name\": \"" << experiment->getName() << "\",\n";
        file << "      \"start_step\": " << experiment->getStartStep() << ",\n";
        file << "      \"end_step\": " << experiment->getEndStep() << ",\n";
        file << "      \"metrics\": {\n";
        
        // Save all metrics
        const auto& metrics = experiment->getMetrics();
        bool firstMetric = true;
        for (const auto& pair : metrics) {
            if (!firstMetric) file << ",\n";
            firstMetric = false;
            file << "        \"" << pair.first << "\": " << pair.second;
        }
        
        file << "\n      }\n";
        
        if (i < pImpl->experiments.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"timestamp\": \"" << getCurrentTimeString() << "\"\n";
    file << "}\n";
    
    return true;
}

std::string ExperimentRunner::getCurrentTimeString() const {
    // Get current time as string
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}
