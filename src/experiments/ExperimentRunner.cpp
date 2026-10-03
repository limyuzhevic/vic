#include "ExperimentRunner.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../world/SimpleWorld.hpp"
#include <fstream>
#include <sstream>

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
        // Clone brain from template configuration
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Clone environment from template
        auto environment = std::make_shared<SimpleWorld>();
        
        // Configure environment with same parameters as template if possible
        // Get configuration from brain template
        auto config = brainTemplate->getConfig();
        
        // SimpleWorld uses fixed defaults (20x20), but we can configure it
        // Based on typical environment settings (environment from Phase 6 demo uses 16x16)
        environment->configure(16.0f, 16.0f, 16, 16);
        
        // Set random seed from experiment for reproducibility
        auto seedIt = config->get<uint64_t>("random_seed");
        if (seedIt.has_value()) {
            environment->setRandomSeed(seedIt.value());
        }
        
        environment->reset();

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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== NLM Experiment Results ===" << std::endl;
    file << "Global Seed: " << pImpl->globalSeed << std::endl;
    file << "Total Experiments: " << pImpl->experiments.size() << std::endl;
    file << std::endl;
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& exp = pImpl->experiments[i];
        file << "--- Experiment " << i + 1 << ": " << exp->getName() << " ---" << std::endl;
        file << "Seed: " << exp->getSeed() << std::endl;
        file << std::endl;
        
        // Save metrics
        file << "Metrics:" << std::endl;
        const auto& metrics = exp->getMetrics();
        for (const auto& metric : metrics) {
            file << "  " << metric.first << ": " << metric.second << std::endl;
        }
        file << std::endl;
        
        // Save step data
        file << "Step Data:" << std::endl;
        const auto& steps = exp->getStepData();
        for (const auto& step : steps) {
            file << "  Step " << step.step << ": neurons=" << step.neuronCount 
                 << ", synapses=" << step.synapseCount << 
                 << ", firing=" << step.firingCount << 
                 << ", avg_rate=" << step.avgFiringRate << std::endl;
        }
        file << std::endl;
        
        // Save notes
        const auto& notes = exp->getNotes();
        if (!notes.empty()) {
            file << "Notes:" << std::endl;
            for (const auto& note : notes) {
                file << "  - " << note << std::endl;
            }
            file << std::endl;
        }
    }
    
    file << "=== End of Results ===" << std::endl;
    return true;
}

} // namespace nlm
