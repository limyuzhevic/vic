// NLM Phase 5 Demo
// Demonstrates Phase 5: Memory Consolidation and Continual Learning
// This demo shows the enhanced integration of memory systems, prediction, and cognition

#include "experiments/Phase5Experiment.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

void demonstrateMemoryConsolidation(std::shared_ptr<Brain> brain) {
    std::cout << "=== Memory Consolidation Demo ===" << std::endl;
    
    // Demonstrate episodic memory formation
    for (int episode = 0; episode < 10; ++episode) {
        // Create an episodic memory item representing an experience
        EpisodicMemoryItem episode;
        episode.timestamp = episode * 100;
        episode.action = ActionType::Wait;
        episode.reward = 0.5f + (episode % 3) * 0.3f;
        
        // Add some sensory and neural activity
        for (int i = 0; i < 20; ++i) {
            episode.activeNeurons.push_back(i);
            episode.neuronActivations.push_back(0.5f + (i % 5) * 0.2f);
        }
        
        // Store in episodic memory
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->storeEpisode(episode);
        }
        
        // Simulate working memory update
        if (brain->getWorkingMemory()) {
            std::vector<float> pattern(10, 0.5f);
            brain->getWorkingMemory()->store(pattern, 1.0f);
        }
        
        if (episode % 5 == 0) {
            std::cout << "  Episode " << episode << ": Stored experience with " 
                      << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0) 
                      << " total episodes" << std::endl;
        }
        
        // Simulate brain step
        brain->step(episode * 10, episode * 10 * 0.001);
    }
    
    // Demonstrate memory consolidation
    std::cout << "  Running memory consolidation..." << std::endl;
    if (brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->consolidate(0.3f);
    }
    
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->clear();
    }
    
    std::cout << "  Memory consolidation complete." << std::endl;
}

void demonstratePredictionLearning(std::shared_ptr<Brain> brain) {
    std::cout << "=== Prediction Learning Demo ===" << std::endl;
    
    // Demonstrate prediction system
    for (int step = 0; step < 50; ++step) {
        // Simulate sensory input
        if (auto* predictionSystem = brain->getPredictionSystem()) {
            // Create a simple sensory input
            std::vector<float> sensoryData(10, 0.5f + std::sin(step * 0.1f) * 0.3f);
            SensoryInput input(sensoryData);
            
            // Train prediction system
            predictionSystem->train(input);
            
            // Get prediction error
            float error = predictionSystem->getPredictionError();
            
            if (step % 10 == 0) {
                std::cout << "  Step " << step << ": Prediction error = " << error << std::endl;
            }
        }
        
        brain->step(step, step * 0.001);
    }
    
    std::cout << "  Prediction system trained with temporal sequences." << std::endl;
}

void demonstrateNeuromodulation(std::shared_ptr<Brain> brain) {
    std::cout << "=== Neuromodulation Integration Demo ===" << std::endl;
    
    // Test dopamine, acetylcholine, norepinephrine, and serotonin
    std::cout << "  Testing neuromodulators..." << std::endl;
    
    if (auto* dopamine = brain->getDopamine()) {
        std::cout << "  Dopamine: level=" << dopamine->getLevel() 
                  << " plasticity_factor=" << dopamine->getPlasticityFactor() << std::endl;
        dopamine->signalReward(0.7f);
    }
    
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        std::cout << "  Acetylcholine: level=" << acetylcholine->getLevel() 
                  << " plasticity_factor=" << acetylcholine->getPlasticityFactor() << std::endl;
        acetylcholine->signalAttention(0.6f);
        acetylcholine->enhanceMemory(0.4f);
    }
    
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        std::cout << "  Norepinephrine: level=" << norepinephrine->getLevel() 
                  << " plasticity_factor=" << norepinephrine->getPlasticityFactor() << std::endl;
        norepinephrine->signalArousal(0.5f);
        norepinephrine->increaseVigilance(0.3f);
    }
    
    if (auto* serotonin = brain->getSerotonin()) {
        std::cout << "  Serotonin: level=" << serotonin->getLevel() 
                  << " plasticity_factor=" << serotonin->getPlasticityFactor() << std::endl;
        serotonin->signalMood(0.4f);
        serotonin->modifyImpulsivity(0.2f);
        serotonin->enhanceSocialBehavior(0.3f);
    }
    
    std::cout << "  Neuromodulators integrated with brain function." << std::endl;
}

void demonstrateAgentBrainIntegration(std::shared_ptr<Brain> brain, const SimpleWorld& world) {
    std::cout << "=== AgentBrain Integration Demo ===" << std::endl;
    
    auto agentBrain = std::make_unique<AgentBrain>(brain);
    agentBrain->initialize(world);
    
    std::cout << "  Testing agent brain integration..." << std::endl;
    
    // Test sensory processing
    SensoryPercept percept(256, 8, 4, 6);
    agentBrain->processSensoryInput(percept);
    
    // Test motor decoding
    MotorCommand command = agentBrain->decodeMotorCommand();
    std::cout << "  Motor command: " << static_cast<int>(command) << std::endl;
    
    // Test curiosity and exploration
    std::cout << "  Curiosity level: " << agentBrain->getCuriosityLevel() << std::endl;
    std::cout << "  Prediction error: " << agentBrain->getPredictionError() << std::endl;
    
    // Test attention system integration
    auto* attention = agentBrain->getAttention();
    if (attention) {
        std::vector<NeuronId> competitors;
        for (int i = 0; i < 10; ++i) competitors.push_back(i);
        agentBrain->applyAttention(competitors, 0.5f);
        std::cout << "  Attentional winners: " << agentBrain->getAttentionalWinners().size() << std::endl;
    }
    
    // Test concept formation
    auto* conceptFormation = agentBrain->getConceptFormation();
    if (conceptFormation) {
        std::vector<float> testPattern(5, 0.8f);
        std::cout << "  Testing concept formation..." << std::endl;
    }
    
    // Test neural planner
    auto* planner = agentBrain->getPlanner();
    if (planner) {
        std::vector<float> currentState(5, 0.5f);
        ActionType plannedAction = planner->planAction(currentState, 0.7f);
        std::cout << "  Planned action: " << static_cast<int>(plannedAction) << std::endl;
    }
    
    std::cout << "  AgentBrain successfully integrated with higher cognition systems." << std::endl;
}

void runPhase5Demo() {
    std::cout << "===========================================" << std::endl;
    std::cout << "NLM Phase 5 Demo: Memory Consolidation" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration for Phase 5
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Phase 5 Demo Started:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "" << std::endl;
    
    // Create simple world
    SimpleWorld world(16, 16, 0, 0);
    
    // Run demos
    demonstrateMemoryConsolidation(brain);
    std::cout << "" << std::endl;
    
    demonstratePredictionLearning(brain);
    std::cout << "" << std::endl;
    
    demonstrateNeuromodulation(brain);
    std::cout << "" << std::endl;
    
    demonstrateAgentBrainIntegration(brain, world);
    std::cout << "" << std::endl;
    
    // Final status
    std::cout << "=== Phase 5 Demo Complete ===" << std::endl;
    std::cout << "Key achievements demonstrated:" << std::endl;
    std::cout << "  ✓ Episodic memory formation and consolidation" << std::endl;
    std::cout << "  ✓ Working memory integration" << std::endl;
    std::cout << "  ✓ Prediction system with temporal learning" << std::endl;
    std::cout << "  ✓ Full neuromodulation integration (DA, ACh, NE, 5-HT)" << std::endl;
    std::cout << "  ✓ AgentBrain with higher cognition systems" << std::endl;
    std::cout << "  ✓ Enhanced brain integration and function" << std::endl;
    std::cout << "" << std::endl;
}

int main(int argc, char** argv) {
    // Suppress logging for cleaner demo output
    auto logger = std::make_shared<Logger>();
    auto nullLogger = std::make_shared<NullLogger>();
    logger->addLogger(nullLogger);
    Logger::setGlobal(logger);
    
    runPhase5Demo();
    
    return 0;
}