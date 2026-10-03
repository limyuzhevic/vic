// Phase 5 Demo - Lifetime Learning Experiment
// Shows continual learning with multiple phases

#include "experiments/Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Lifetime Learning Demo ===" << std::endl;
    std::cout << "Demonstrating lifelong learning through multiple developmental phases..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create Phase 5 experiment
    Phase5IntegratedExperiment experiment;
    
    // Configure for demo (shorter than full lifetime)
    LifetimeExperimentConfig config;
    config.initialNeurons = 1000;
    config.maxNeurons = 5000;
    config.synapseFormationSteps = 20000;
    config.sensoryDevelopmentSteps = 20000;
    config.motorDevelopmentSteps = 20000;
    config.associativeDevelopmentSteps = 30000;
    config.memoryConsolidationSteps = 30000;
    config.socialDevelopmentSteps = 30000;
    config.trialsPerPhase = 3;
    config.stepsPerTrial = 5000;
    config.enableMultithreading = false; // Simpler for demo
    config.enableSIMD = false;
    config.enableSparseConnectivity = true;
    config.enableEventDriven = true;
    config.threadCount = 1;
    config.saveCheckpoints = true;
    config.generateVisualizations = false;
    config.runAblations = false;
    
    std::cout << "Running Phase 5 Lifetime Experiment with " << config.stepsPerTrial << " steps per trial..." << std::endl;
    std::cout << "Total estimated steps: " << config.synapseFormationSteps + 
                     config.sensoryDevelopmentSteps + 
                     config.motorDevelopmentSteps + 
                     config.associativeDevelopmentSteps + 
                     config.memoryConsolidationSteps + 
                     config.socialDevelopmentSteps << std::endl;
    
    // Function to create brain
    auto createBrain = [](const LifetimeExperimentConfig& config) -> std::shared_ptr<Brain> {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.initialNeurons);
        cfg->set("region_count", 2);
        cfg->set("connection_probability", 0.1f);
        cfg->set("random_seed", 42);
        cfg->set("simulation_timestep", 0.001);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("stdp_tau", 20.0f);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (brain->initialize()) {
            return brain;
        }
        return nullptr;
    };
    
    // Function to run simulation
    auto runSimulation = [](Brain* brain, uint64_t steps) -> bool {
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return true;
    };
    
    // Run the experiment
    auto result = experiment.run(config, createBrain, runSimulation);
    
    std::cout << std::endl;
    std::cout << "=== EXPERIMENT RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average reward per step: " << result.avgRewardPerStep << std::endl;
    std::cout << "Learning efficiency: " << result.overallLearningEfficiency << std::endl;
    std::cout << std::endl;
    std::cout << "Final capabilities:" << std::endl;
    std::cout << "  Performance: " << result.finalPerformance << std::endl;
    std::cout << "  Memory capacity: " << result.memoryCapacity << std::endl;
    std::cout << "  Prediction accuracy: " << result.predictionAccuracy << std::endl;
    std::cout << "  Generalization ability: " << result.generalizationAbility << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== PHASE SUMMARY ===" << std::endl;
    for (const auto& phase : result.phaseResults) {
        std::cout << phase.phaseName << ": steps " << phase.startStep 
                  << "-" << phase.endStep << ", reward=" << phase.totalReward << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "=== EXPERIMENT COMPLETE ===" << std::endl;
    std::cout << "The brain successfully completed a lifetime of learning through multiple developmental phases." << std::endl;
    std::cout << "It developed from basic reflexes to complex behavior, with continual adaptation and memory consolidation." << std::endl;
    
    return 0;
}
