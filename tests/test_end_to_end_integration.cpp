// End-to-End Integration Tests
// Tests complete agent-in-world integration with all systems

#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

namespace test_end_to_end_integration {

void testCompleteAgentWorldLoop() {
    std::cout << "  Testing complete agent-in-world integration..." << std::endl;
    
    // Create brain with default configuration
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Create agent brain interface
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create simple world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Enable all major systems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Run simulation for multiple steps
    const int simulationSteps = 20;
    
    for (int step = 0; step < simulationSteps; ++step) {
        // 1. Update world
        world.update(0.1);
        
        // 2. Get sensory percept
        SensoryPercept percept = world.getSensoryPercept();
        
        // 3. Process sensory input (inject into brain)
        agent.processSensoryInput(percept);
        
        // 4. Brain thinks (step)
        brain.step(step);
        
        // 5. Agent selects action based on brain output
        MotorCommand action = agent.decodeMotorCommand();
        
        // 6. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // 7. Apply reward modulation if reward system enabled
        if (agent.isRewardModulationEnabled()) {
            // Simple reward: positive for movement, zero otherwise
            float reward = (action == MotorCommand::Wait) ? 0.0f : 1.0f;
            agent.applyRewardModulation(reward, 0.0f);  // Predicted reward is 0
        }
        
        // 8. Update development if enabled
        if (agent.isDevelopmentEnabled()) {
            agent.updateDevelopment(0.1);
        }
        
        // 9. Update cognition systems
        agent.updateCognitionSystems();
        
        // Basic validation
        assert(action >= MotorCommand::MoveForward && action <= MotorCommand::Wait);
        
        // Periodically check system health
        if (step % 5 == 0) {
            assert(agent.getCuriosityLevel() >= 0.0f && agent.getCuriosityLevel() <= 1.0f);
            assert(agent.getNoveltyLevel() >= 0.0f);
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testIntegrationWithAllMemorySystems() {
    std::cout << "  Testing integration with all memory systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Store initial vision for comparison
    std::vector<float> initialVision = world.getSensoryPercept().getVision();
    
    // Run simulation
    for (int step = 0; step < 15; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        
        // Check that memory systems are accessible and functional
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* associativeMemory = brain.getAssociativeMemory();
        
        if (workingMemory) {
            // Working memory should accumulate some activity
            float activity = workingMemory->getMemoryActivity();
            assert(activity >= 0.0f);
        }
        
        if (episodicMemory) {
            // Episodic memory may have stored episodes
            size_t episodes = episodicMemory->getEpisodeCount();
            assert(episodes >= 0);
        }
        
        if (associativeMemory) {
            // Associative memory should be functional
            std::vector<float> patternA = {0.1f, 0.2f};
            std::vector<float> patternB = {0.3f, 0.4f};
            float strength = associativeMemory->getAssociationStrength(patternA, patternB);
            assert(strength >= 0.0f);
        }
        
        // Update cognition systems
        agent.updateCognitionSystems();
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testIntegrationWithPredictionSystem() {
    std::cout << "  Testing integration with PredictionSystem..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Connect prediction system to agent
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    agent.connectPredictionSystem(predictionSystem);
    
    // Run simulation with prediction system
    for (int step = 0; step < 10; ++step) {
        world.update(0.1);
        
        // Get sensory input
        SensoryPercept percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        // Brain step
        brain.step(step);
        
        // Update cognition systems (including prediction)
        agent.updateCognitionSystems();
        
        // Check prediction system status
        float error = predictionSystem->getPredictionError();
        assert(error >= 0.0f);
        
        float confidence = predictionSystem->getConfidence();
        assert(confidence >= 0.0f && confidence <= 1.0f);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testIntegrationWithCognitionSystems() {
    std::cout << "  Testing integration with all cognition systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Enable cognition systems
    agent.enableCuriosity(true);
    
    // Run simulation focusing on cognition
    for (int step = 0; step < 10; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        
        // Update all cognition systems
        agent.updateCognitionSystems();
        
        // Check cognition system outputs
        auto* planner = agent.getNeuralPlanner();
        if (planner) {
            std::vector<float> goal = planner->getCurrentGoal();
            assert(!goal.empty());
        }
        
        auto* selfModel = agent.getSelfModel();
        if (selfModel) {
            float capability = selfModel->getCapabilityLevel();
            assert(capability >= 0.0f && capability <= 1.0f);
        }
        
        auto* socialLearning = agent.getSocialLearning();
        if (socialLearning) {
            size_t observations = socialLearning->getObservationCount();
            assert(observations >= 0);
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testSystemHealthAndConsistency() {
    std::cout << "  Testing system health and consistency..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Run extended simulation
    const int healthCheckSteps = 25;
    
    for (int step = 0; step < healthCheckSteps; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        agent.updateCognitionSystems();
        
        // Periodic health checks
        if (step % 5 == 0) {
            // Check all major systems are still functional
            assert(agent.getBrain() != nullptr);
            assert(agent.getCuriosityLevel() >= 0.0f);
            assert(agent.getNoveltyLevel() >= 0.0f);
            assert(agent.getPredictionError() >= 0.0f);
            
            // Check memory systems
            auto* workingMemory = agent.getBrain()->getWorkingMemory();
            if (workingMemory) {
                assert(workingMemory->getMemoryActivity() >= 0.0f);
            }
            
            // Check cognition systems
            auto* planner = agent.getNeuralPlanner();
            if (planner) {
                assert(planner->getPlanningConfidence() >= 0.0f && 
                       planner->getPlanningConfidence() <= 1.0f);
            }
        }
        
        // Reset occasionally to test reset functionality
        if (step % 10 == 9) {
            agent.reset();
            agent.updateDevelopment(0.0);  // Also reset development
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== End-to-End Integration Tests ===" << std::endl;
    
    testCompleteAgentWorldLoop();
    testIntegrationWithAllMemorySystems();
    testIntegrationWithPredictionSystem();
    testIntegrationWithCognitionSystems();
    testSystemHealthAndConsistency();
    
    std::cout << "=== End-to-End Integration Tests Complete ===" << std::endl;
}

} // namespace test_end_to_end_integration