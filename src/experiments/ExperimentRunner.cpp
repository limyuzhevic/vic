#include "ExperimentRunner.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>

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
    
    if (!brainTemplate || !environmentTemplate) {
        return results; // Empty results on invalid templates
    }
    
    for (auto& exp : experiments) {
        if (!exp) {
            continue; // Skip null experiments
        }
        
        // Create fresh brain and environment for each experiment
        // Clone from template to ensure independent runs
        auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(brainTemplate->getConfig()));
        brain->initialize();
        
        // Create fresh environment for each experiment
        // TODO: In real implementation, would clone environment from template
        auto environment = std::make_shared<SimpleWorld>();
        environment->configure(16, 16, 8, 8);
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
    
    // Write header
    file << "# NLM Experiment Results\n";
    file << "# Generated: " << time(nullptr) << "\n\n";
    
    file << "Experiment Results Summary:\n";
    file << "Total Experiments: " << pImpl->experiments.size() << "\n\n";
    
    // Write experiment details
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& exp = pImpl->experiments[i];
        
        file << "=== Experiment " << (i + 1) << ": " << exp->getName() << " ===\n";
        file << "Seed: " << exp->getSeed() << "\n";
        file << "Start Step: " << exp->getStartStep() << "\n";
        file << "End Step: " << exp->getEndStep() << "\n";
        file << "Duration: " << (exp->getEndStep() - exp->getStartStep()) << " steps\n\n";
        
        // Write configuration
        file << "Configuration:\n";
        for (const auto& configPair : exp->pImpl->config) {
            file << "  " << configPair.first << " = " << configPair.second << "\n";
        }
        
        // Write metrics
        file << "\nMetrics:\n";
        for (const auto& metricPair : exp->pImpl->metrics) {
            file << "  " << metricPair.first << " = " << metricPair.second << "\n";
        }
        
        // Write step data summary
        file << "\nStep Data Summary (first 100 steps):\n";
        file << "  Step | Neurons | Synapses | Firing | Avg Firing Rate\n";
        file << "  -----|---------|----------|--------|----------------\n";
        
        const auto& stepData = exp->getStepData();
        size_t stepsToShow = std::min(stepData.size(), static_cast<size_t>(100));
        for (size_t j = 0; j < stepsToShow; ++j) {
            const auto& step = stepData[j];
            file << "  " << std::setw(4) << step.step 
                 << " | " << std::setw(8) << step.neuronCount
                 << " | " << std::setw(10) << step.synapseCount
                 << " | " << std::setw(6) << step.firingCount
                 << " | " << std::fixed << std::setprecision(3) << std::setw(14) << step.avgFiringRate << "\n";
        }
        
        // Write notes if any
        if (!exp->getNotes().empty()) {
            file << "\nNotes:\n";
            for (const auto& note : exp->getNotes()) {
                file << "  - " << note << "\n";
            }
        }
        
        file << "\n"; // Empty line between experiments
    }
    
    return true;
}

} // namespace nlm
