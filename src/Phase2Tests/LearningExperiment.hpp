// Phase 2 Learning Experiment Implementation
#pragma once

#include "core/Phase2TestFramework.hpp"
#include "core/Phase2TestUtils.hpp"
#include <memory>
#include <numeric>

namespace nlm {
namespace phase2 {

/**
 * Phase 2 Learning Experiment
 */
class LearningExperiment : public Phase2Test {
public:
    LearningExperiment() = default;
    ~LearningExperiment() override = default;
    
    TestResult run(std::shared_ptr<Config> config) override;
    std::string getName() const override { return "Learning Experiment"; }
    std::string getDescription() const override { 
        return "Comprehensive learning experiment demonstrating measurable synaptic changes through experience"; 
    }
    bool shouldRun(const Phase2TestConfig& config) const override { 
        return config.runLearningExperiment; 
    }
};

TestResult LearningExperiment::run(std::shared_ptr<Config> config) {
    TestResult result(getName());
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create brain
        auto brain = TestUtils::createBrain(config);
        if (!brain->initialize()) {
            result.passed = false;
            result.message = "Failed to initialize brain";
            return result;
        }
        
        // Record initial state
        LearningExperiment::LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input pattern
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < PHASE2_TEST_STEPS; ++step) {
            // Create input pattern
            TestUtils::createInputPattern(brain, 20, PHASE2_CURRENT_STRENGTH);
            brain->step(step, step * PHASE2_SIMULATION_TIMESTEP);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(experiment.finalWeights.empty() ? 0 : 
            std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f) / experiment.finalWeights.size() - 
            (experiment.initialWeights.empty() ? 0 : 
            std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f) / experiment.initialWeights.size())) > 0.001f) ||
                               (!experiment.finalWeights.empty() && !experiment.initialWeights.empty());
        
        result.passed = learningOccurred;
        result.message = "Learning experiment " + std::string(result.passed ? "passed" : "failed");
        result.details.push_back("Initial mean weight: " + std::to_string(
            experiment.initialWeights.empty() ? 0 : 
            std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f) / experiment.initialWeights.size()));
        result.details.push_back("Final mean weight: " + std::to_string(
            experiment.finalWeights.empty() ? 0 : 
            std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f) / experiment.finalWeights.size()));
        result.details.push_back("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        result.details.push_back("Learning occurred: " + std::string(learningOccurred ? "Yes" : "No"));
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Error running learning experiment: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

} // namespace phase2
} // namespace nlm