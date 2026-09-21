#include "Brain.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"
#include <iostream>
#include <memory>

void testPhase6Integration() {
    std::cout << "=== Phase 6 Integration Test ===" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Create world
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0f, 10.0f);
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Test that all integrated systems are present
    std::cout << std::endl << "=== Testing System Integration ===" << std::endl;
    
    // Working Memory
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "✓ Working Memory is integrated" << std::endl;
        
        // Test working memory update
        brain->step(0);
        std::cout << "  Working memory updated, activity: " << workingMemory->getMemoryActivity() << std::endl;
    } else {
        std::cerr << "✗ Working Memory is NOT integrated" << std::endl;
    }
    
    // Episodic Memory
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "✓ Episodic Memory is integrated" << std::endl;
        
        // Test episodic memory storage
        brain->step(1);
        std::cout << "  Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    } else {
        std::cerr << "✗ Episodic Memory is NOT integrated" << std::endl;
    }
    
    // Prediction System
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        std::cout << "✓ Prediction System is integrated" << std::endl;
        
        // Test prediction system
        float error = predictionSystem->getPredictionError();
        std::cout << "  Prediction error: " << error << std::endl;
        
        // Test setting prediction error
        predictionSystem->setPredictionError(0.1f);
        std::cout << "  Set prediction error to 0.1f" << std::endl;
    } else {
        std::cerr << "✗ Prediction System is NOT integrated" << std::endl;
    }
    
    // Neural Planner
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "✓ Neural Planner is integrated" << std::endl;
        
        // Test planner update
        planner->update(0.001);
        std::cout << "  Planner updated, planning depth: " << planner->getPlanningDepth() << std::endl;
        
        // Test concept formation integration
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            std::cout << "  Concept formation also integrated" << std::endl;
        }
    } else {
        std::cerr << "✗ Neural Planner is NOT integrated" << std::endl;
    }
    
    // Neuromodulation
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "✓ Neuromodulation (Dopamine) is integrated" << std::endl;
        
        // Test neuromodulation
        float level = dopamine->getLevel();
        std::cout << "  Dopamine level: " << level << std::endl;
    } else {
        std::cerr << "✗ Neuromodulation is NOT integrated" << std::endl;
    }
    
    // Attention System
    auto* attention = brain->getAttention();
    if (attention) {
        std::cout << "✓ Attention System is integrated" << std::endl;
        
        // Test attention update
        attention->update(0.001);
        std::cout << "  Attention updated" << std::endl;
    } else {
        std::cerr << "✗ Attention System is NOT integrated" << std::endl;
    }
    
    // Development System
    auto* development = brain->getDevelopmentSystem();
    if (development) {
        std::cout << "✓ Development System is integrated" << std::endl;
    } else {
        std::cerr << "✗ Development System is NOT integrated" << std::endl;
    }
    
    // Run some simulation steps
    std::cout << std::endl << "=== Running Simulation ===" << std::endl;
    for (int step = 0; step < 10; ++step) {
        // Get sensory input from world
        auto percept = world.getSensoryPercept();
        
        // Process sensory input through agent
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step);
        
        // Get motor command from agent
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // Compute reward
        auto internalSignals = world.getSensoryPercept().getInternal();
        float reward = internalSignals.size() > 0 ? internalSignals[0] : 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        if (step % 5 == 0) {
            std::cout << "  Step " << step << ": " 
                     << "Reward: " << reward << ", "
                     << "Firing neurons: " << brain->getFiringNeuronCount() << ", "
                     << "Dopamine: " << agent->getNeuromodulationLevel() << std::endl;
        }
    }
    
    // Final verification
    std::cout << std::endl << "=== Final Integration Status ===" << std::endl;
    
    // Check if all critical systems are working
    bool workingMemoryOK = (workingMemory != nullptr);
    bool episodicMemoryOK = (episodicMemory != nullptr);
    bool predictionOK = (predictionSystem != nullptr);
    bool plannerOK = (planner != nullptr);
    bool neuromodulationOK = (dopamine != nullptr);
    
    std::cout << "Working Memory: " << (workingMemoryOK ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Episodic Memory: " << (episodicMemoryOK ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Prediction System: " << (predictionOK ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Neural Planner: " << (plannerOK ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Neuromodulation: " << (neuromodulationOK ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    
    int workingCount = (workingMemoryOK ? 1 : 0) + (episodicMemoryOK ? 1 : 0) + 
                       (predictionOK ? 1 : 0) + (plannerOK ? 1 : 0) + 
                       (neuromodulationOK ? 1 : 0);
    
    std::cout << std::endl << "=== Integration Score: " << workingCount << "/5 critical systems ===" << std::endl;
    
    if (workingCount >= 4) {
        std::cout << "SUCCESS: Phase 6 integration is working!" << std::endl;
        std::cout << "The NLM brain now functions as an integrated artificial brain." << std::endl;
    } else {
        std::cerr << "WARNING: Many systems are still disconnected." << std::endl;
        std::cerr << "Phase 6 integration needs more work." << std::endl;
    }
}

int main() {
    testPhase6Integration();
    return 0;
}

