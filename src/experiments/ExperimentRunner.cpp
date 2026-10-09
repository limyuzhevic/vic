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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Experiment Results" << "\n";
    file << "# Generated on " << getCurrentTimeString() << "\n";
    file << "# " << pImpl->experiments.size() << " experiments saved\n\n";
    
    for (const auto& experiment : pImpl->experiments) {
        file << "[EXPERIMENT] " << experiment->getName() << "\n";
        file << "  Seed: " << experiment->getSeed() << "\n";
        file << "  Steps: " << experiment->getStartStep() << " - " << experiment->getEndStep() << "\n";
        file << "  Duration: " << (experiment->getEndStep() - experiment->getStartStep()) << " steps\n\n";
        
        // Write metrics
        file << "  [METRICS]\n";
        const auto& metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            file << "    " << metric.first << " = " << metric.second << "\n";
        }
        file << "\n";
        
        // Write step data summary
        file << "  [STEP_DATA]\n";
        const auto& stepData = experiment->getStepData();
        file << "    Steps recorded: " << stepData.size() << "\n";
        if (!stepData.empty()) {
            file << "    First step neurons: " << stepData.front().neuronCount << "\n";
            file << "    Last step neurons: " << stepData.back().neuronCount << "\n";
            file << "    Peak firing rate: " << getMaxFiringRate(stepData) << " Hz\n";
        }
        file << "\n";
        
        // Write notes
        const auto& notes = experiment->getNotes();
        if (!notes.empty()) {
            file << "  [NOTES]\n";
            for (const auto& note : notes) {
                file << "    " << note << "\n";
            }
            file << "\n";
        }
        
        file << "=== END EXPERIMENT ===\n\n";
    }
    
    return true;
}

// Helper function to get current time as string
std::string ExperimentRunner::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to find max firing rate in step data
float ExperimentRunner::getMaxFiringRate(const std::vector<Experiment::StepData>& stepData) const {
    if (stepData.empty()) return 0.0f;
    
    float maxRate = 0.0f;
    for (const auto& step : stepData) {
        maxRate = std::max(maxRate, step.avgFiringRate);
    }
    return maxRate;
}

} // namespace nlm
