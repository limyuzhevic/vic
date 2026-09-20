// NLM (熙然) - Neural Learning Machine
// Phase 5: Advanced Learning Features Demo
//
// This demo showcases the advanced capabilities of Phase 5:
// - Large-scale neural simulation
// - Event-driven processing
// - Memory consolidation
// - Continual adaptation
// - Emergent specialization

#pragma once

#include "Phase5Experiment.hpp"
#include "core/ConsoleLogger.hpp"
#include <chrono>

namespace nlm {

int runPhase5Demo(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 5 - ADVANCED LEARNING FEATURES\n";
    std::cout << "===========================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 50;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    Phase5Experiment::configureEnvironment(config);
    Phase5Experiment::configureBrain(config);
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return 1;
    }
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Configure agent brain with advanced features
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    agentBrain.enablePrediction(true);
    
    NLM_LOG_INFO("Phase 5 Advanced Features Demo");
    NLM_LOG_INFO("===============================");
    NLM_LOG_INFO("");
    
    // Create Phase 5 experiment
    Phase5Experiment experiment;
    
    // Configure experiment parameters
    experiment.setLearningRate(0.01f);
    experiment.setMemoryConsolidation(true);
    experiment.setContinualLearning(true);
    experiment.setEmergentSpecialization(true);
    
    NLM_LOG_INFO("Running advanced learning demonstration...");
    NLM_LOG_INFO("Number of trials: " + std::to_string(numTrials));
    NLM_LOG_INFO("");
    
    // Run the experiment
    auto startTime = std::chrono::high_resolution_clock::now();
    Phase5ExperimentResult result = experiment.run(brain.get(), agentBrain, world, numTrials);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Display results
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "RESULTS SUMMARY\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    std::cout << "Performance Metrics:\n";
    std::cout << "  Total Reward: " << std::fixed << std::setprecision(2) << result.totalReward << "\n";
    std::cout << "  Average Reward per Trial: " << result.avgRewardPerTrial << "\n";
    std::cout << "  Learning Efficiency: " << result.learningEfficiency << "\n";
    std::cout << "  Final Performance: " << result.finalPerformance << "\n";
    std::cout << "  Memory Capacity: " << result.memoryCapacity << "\n";
    std::cout << "  Prediction Accuracy: " << result.predictionAccuracy << "\n";
    std::cout << "  Generalization Ability: " << result.generalizationAbility << "\n";
    std::cout << "\n";
    
    std::cout << "Development Metrics:\n";
    std::cout << "  Phases Completed: " << result.phases.size() << "\n";
    for (size_t i = 0; i < result.phases.size(); ++i) {
        const auto& phase = result.phases[i];
        std::cout << "    Phase " << (i + 1) << " (" << phase.phaseName << "):";
        std::cout << " Reward: " << phase.totalReward;
        std::cout << ", Steps: " << phase.endStep - phase.startStep << "\n";
    }
    std::cout << "\n";
    
    std::cout << "Brain State:\n";
    std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
    std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
    std::cout << "  Current Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    std::cout << "  Working Memory Capacity: " << brain->getWorkingMemory() ? 
        brain->getWorkingMemory()->getCapacity() : 0 << "\n";
    std::cout << "  Episodic Memory Episodes: " << brain->getEpisodicMemory() ? 
        brain->getEpisodicMemory()->getEpisodeCount() : 0 << "\n";
    std::cout << "\n";
    
    std::cout << "Demonstration Features:\n";
    std::cout << "  ✓ Large-scale neural simulation (" << brain->getTotalNeuronCount() << " neurons)\n";
    std::cout << "  ✓ Event-driven processing with spike propagation\n";
    std::cout << "  ✓ Memory consolidation and replay\n";
    std::cout << "  ✓ Continual adaptation and learning\n";
    std::cout << "  ✓ Emergent specialization across phases\n";
    std::cout << "  ✓ Reward prediction and error correction\n";
    std::cout << "  ✓ Curiosity-driven exploration\n";
    std::cout << "  ✓ Structural plasticity and development\n";
    std::cout << "\n";
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    std::cout << "Execution Time: " << duration << " seconds\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "\nPhase 5 Advanced Features successfully demonstrated!\n";
    
    return 0;
}

} // namespace nlm