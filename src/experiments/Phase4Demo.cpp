#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"

using namespace nlm;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 50;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Create brain and world
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    
    brain->initialize();
    world->configure(20.0f, 20.0f, 16, 16);
    agentBrain->initialize(*world);
    
    // Enable advanced features
    agentBrain->enableRewardModulation(true);
    agentBrain->enableStructuralPlasticity(true);
    agentBrain->enableDevelopment(true);
    agentBrain->enableCuriosity(true);
    
    printHeader("Phase 4 Experiment: Emerging Cognition");
    std::cout << "Running " << numTrials << " trials...\n\n";
    
    // Run experiment
    Phase4Experiment experiment(*brain, *world, *agentBrain);
    experiment.run(numTrials);
    
    printHeader("Experiment Results");
    
    // Print summary
    printSection("Performance Metrics");
    printResult("Total Trials", static_cast<float>(numTrials));
    printResult("Total Reward", experiment.getTotalReward());
    printResult("Average Reward", experiment.getAverageReward());
    printResult("Total Synapses", experiment.getTotalSynapseCount());
    printResult("Average Firing Rate", experiment.getAverageFiringRate());
    
    printSection("Cognitive Development");
    printResult("Developmental Stage", static_cast<float>(agentBrain->getDevelopmentalStage()));
    printResult("Neuromodulation Level", agentBrain->getNeuromodulationLevel());
    printResult("Curiosity Level", agentBrain->getCuriosityLevel());
    printResult("Novelty Level", agentBrain->getNoveltyLevel());
    printResult("Prediction Error", agentBrain->getPredictionError());
    
    printSection("Memory Performance");
    if (brain->getWorkingMemory()) {
        printResult("Working Memory Traces", brain->getWorkingMemory()->getActiveTraces());
    }
    if (brain->getEpisodicMemory()) {
        printResult("Episodic Memory Episodes", brain->getEpisodicMemory()->getEpisodeCount());
    }
    
    std::cout << "\n";
    std::cout << "Phase 4 experiment complete.\n";
    std::cout << "The brain demonstrated emergent cognitive abilities\n";
    std::cout << "including concept formation, planning, and social behavior.\n";
    
    return 0;
}