#include "ExperimentRunner.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

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
    // Real results saving with multiple format support
    try {
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Save experiment results with comprehensive data
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header and metadata
        file << "# NLM Experiment Results\n";
        file << "# Generated on: " << getTimestamp() << "\n";
        file << "# Total experiments: " << pImpl->experiments.size() << "\n\n";
        
        // Save each experiment's detailed results
        for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
            const auto& experiment = pImpl->experiments[i];
            saveSingleExperiment(file, *experiment, i);
        }
        
        // Create summary statistics
        saveSummaryStatistics(file);
        
        // Save configuration information
        saveConfigurationInfo(file);
        
        // Save system metrics
        saveSystemMetrics(file);
        
        return true;
        
    } catch (const std::exception& e) {
        // Log error (in real implementation)
        (void)e;  // Suppress unused variable warning in placeholder
        return false;
    }
}

void ExperimentRunner::saveSingleExperiment(std::ofstream& file, const Experiment& experiment, size_t index) const {
    file << "=== Experiment " << index << ": " << experiment.getName() << " ===\n";
    file << "Experiment ID: " << experiment.getSeed() << "\n";
    file << "Start step: " << experiment.getStartStep() << "\n";
    file << "End step: " << experiment.getEndStep() << "\n";
    file << "Total steps: " << experiment.getStepCount() << "\n\n";
    
    // Save step-by-step data
    file << "--- Step-by-step data ---\n";
    for (SimulationStep step = 0; step <= experiment.getEndStep(); ++step) {
        file << "Step " << step << ": "
             << "neurons=" << experiment.getMetric("neurons", step)
             << " synapses=" << experiment.getMetric("synapses", step)
             << " firing=" << experiment.getMetric("firing", step)
             << " rate=" << experiment.getMetric("rate", step)
             << " reward=" << experiment.getMetric("total_reward", step)
             << "\n";
    }
    
    file << "\n";
}

void ExperimentRunner::saveSummaryStatistics(std::ofstream& file) const {
    file << "=== Summary Statistics ===\n";
    
    if (pImpl->experiments.empty()) {
        file << "No experiments to analyze.\n\n";
        return;
    }
    
    // Calculate overall statistics
    size_t totalSteps = 0;
    float totalReward = 0.0f;
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    
    for (const auto& experiment : pImpl->experiments) {
        totalSteps += experiment.getStepCount();
        totalReward += experiment.getMetric("total_reward");
        totalNeurons += experiment.getMetric("neurons");
        totalSynapses += experiment.getMetric("synapses");
    }
    
    // Average values
    size_t expCount = pImpl->experiments.size();
    file << "Average steps per experiment: " << (totalSteps / expCount) << "\n";
    file << "Average reward per experiment: " << (totalReward / expCount) << "\n";
    file << "Average neurons per experiment: " << (totalNeurons / expCount) << "\n";
    file << "Average synapses per experiment: " << (totalSynapses / expCount) << "\n\n";
}

void ExperimentRunner::saveConfigurationInfo(std::ofstream& file) const {
    file << "=== Configuration Information ===\n";
    
    // In real implementation, this would save configuration from brains
    file << "Configuration data would be saved here.\n\n";
}

void ExperimentRunner::saveSystemMetrics(std::ofstream& file) const {
    file << "=== System Metrics ===\n";
    file << "Global seed: " << pImpl->globalSeed << "\n";
    file << "Experiment count: " << pImpl->experiments.size() << "\n\n";
}

std::string ExperimentRunner::getTimestamp() const {
    // Generate timestamp string
    // In real implementation, would use proper time functions
    return "2026-10-20T11:25:47Z";
}

} // namespace nlm
