#include "ExperimentRunner.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    // Results cache for batch processing
    std::vector<std::shared_ptr<Experiment>> batchResults;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    exp->setCreationTime(std::chrono::system_clock::now());
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
    experiment->setStartTime(std::chrono::system_clock::now());
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Record initial state
    experiment->recordStep(
        0,
        brain->getTotalNeuronCount(),
        brain->getTotalSynapseCount(),
        brain->getFiringNeuronCount(),
        brain->getAverageFiringRate()
    );
    
    // Run simulation loop with performance monitoring
    auto startTime = std::chrono::system_clock::now();
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        auto stepStartTime = std::chrono::system_clock::now();
        
        // Brain step with timing
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
        
        // Performance monitoring
        auto stepEndTime = std::chrono::system_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEndTime - stepStartTime);
        if (stepDuration.count() > 100) {  // Log slow steps
            experiment->recordWarning(std::string("Slow step " + std::to_string(step) + ": " +
                std::to_string(stepDuration.count()) + "ms"));
        }
    }
    
    experiment->setEndStep(maxSteps);
    auto endTime = std::chrono::system_clock::now();
    experiment->setEndTime(endTime);
    
    // Calculate experiment statistics
    experiment->finalize();
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    pImpl->batchResults.clear();
    
    // Create fresh brain and environment for each experiment
    for (auto& exp : experiments) {
        // Create fresh brain from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Run experiment
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
            pImpl->batchResults.push_back(exp);
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
    pImpl->batchResults.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // Save results to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "Experiment Results" << std::endl;
    file << "=================" << std::endl << std::endl;
    
    // Write experiment information
    for (auto& exp : pImpl->experiments) {
        file << "Experiment: " << exp->getName() << std::endl;
        file << "  Seed: " << exp->getSeed() << std::endl;
        file << "  Start Step: " << exp->getStartStep() << std::endl;
        file << "  End Step: " << exp->getEndStep() << std::endl;
        file << "  Total Steps: " << exp->getSteps().size() << std::endl;
        file << "  Total Reward: " << exp->getMetric("total_reward") << std::endl;
        
        // Write final metrics
        const auto& metrics = exp->getMetrics();
        for (auto& metric : metrics) {
            file << "  " << metric.first << ": " << metric.second << std::endl;
        }
        
        file << std::endl;
    }
    
    // Write batch results if available
    if (!pImpl->batchResults.empty()) {
        file << "Batch Results Summary" << std::endl;
        file << "====================" << std::endl;
        file << "Total Experiments: " << pImpl->batchResults.size() << std::endl;
        
        // Calculate batch statistics
        double totalReward = 0.0;
        double totalDuration = 0.0;
        
        for (auto& exp : pImpl->batchResults) {
            totalReward += exp->getMetric("total_reward");
            // In real implementation, would calculate duration from start/end times
        }
        
        file << "Average Reward: " << (pImpl->batchResults.empty() ? 0.0 : totalReward / pImpl->batchResults.size()) << std::endl;
        
        file << std::endl;
    }
    
    file.close();
    return true;
}

} // namespace nlm