// Phase 5 Demo - Performance and Advanced Features
// 
// This demo demonstrates the performance optimizations and advanced features
// added in Phase 5, including memory pools, SIMD vectorization, and parallel processing.

#include "Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Performance Demo ===" << std::endl;
    std::cout << "Demonstrating performance optimizations and advanced features..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase5IntegratedExperiment experiment;
    
    // Configure experiment for demo (using LifetimeExperimentConfig from Phase5Experiment)
    nlm::LifetimeExperimentConfig config;
    config.initialNeurons = 1000;
    config.maxNeurons = 10000;
    config.connectionProbability = 0.1f;
    config.synapseFormationSteps = 10000;
    config.sensoryDevelopmentSteps = 20000;
    config.motorDevelopmentSteps = 10000;
    config.associativeDevelopmentSteps = 20000;
    config.memoryConsolidationSteps = 20000;
    config.socialDevelopmentSteps = 20000;
    config.trialsPerPhase = 2;
    config.stepsPerTrial = 5000;
    config.enableMultithreading = true;
    config.enableSIMD = true;
    config.enableSparseConnectivity = true;
    config.enableEventDriven = true;
    config.threadCount = 0;  // auto
    config.saveCheckpoints = true;
    config.generateVisualizations = false;  // Disable for demo
    config.runAblations = false;  // Disable for demo
    
    std::cout << "Running Phase 5 performance demo with " << config.initialNeurons 
              << " neurons (max " << config.maxNeurons << ") for " << config.stepsPerTrial << " steps per trial" << std::endl;
    
    // Create a simple brain function for the demo
    auto createBrain = [](const nlm::LifetimeExperimentConfig& config) -> std::shared_ptr<nlm::Brain> {
        // For demo purposes, create a basic brain
        auto cfg = std::make_shared<nlm::Config>();
        cfg->set("neuron_count", static_cast<int64_t>(config.initialNeurons));
        cfg->set("region_count", static_cast<int64_t>(1));
        cfg->set("connection_probability", config.connectionProbability);
        return std::make_shared<nlm::Brain>(cfg);
    };
    
    // Create a simple simulation function for the demo
    auto runSimulation = [](nlm::Brain* brain, uint64_t steps) -> bool {
        // Simple simulation for demo
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step);
            if (step % 1000 == 0) {
                std::cout << "  Simulation step " << step << "/" << steps << std::endl;
            }
        }
        return true;
    };
    
    auto result = experiment.run(config, createBrain, runSimulation);
    
    std::cout << std::endl;
    std::cout << "=== PHASE 5 PERFORMANCE RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg reward per step: " << result.avgRewardPerStep << std::endl;
    std::cout << "Learning efficiency: " << result.overallLearningEfficiency << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== PERFORMANCE STATUS ===" << std::endl;
    if (!result.failures.empty()) {
        std::cout << "Failures: " << result.failures.size() << std::endl;
        for (const auto& failure : result.failures) {
            std::cout << "  - " << failure << std::endl;
        }
    }
    if (!result.warnings.empty()) {
        std::cout << "Warnings: " << result.warnings.size() << std::endl;
        for (const auto& warning : result.warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    
    std::cout << "Phase 5 performance demo complete." << std::endl;
    
    return 0;
}

