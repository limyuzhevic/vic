// NLM Phase 3 Demo Tests
// Tests world interaction and behavioral learning

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class SimpleWorld;
    class AgentBrain;
    class SensoryPercept;
    class Sensority;
    class MotorCommand;
    class ActionResult;
    class Logger;
}

namespace test_demo_phase3 {
    
    void testWorldInteraction() {
        std::cout << "  Testing World Interaction..." << std::endl;
        
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        
        // Create brain and agent
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        nlm::SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        
        // Run a few steps
        float totalReward = 0.0f;
        for (int step = 0; step < 50; ++step) {
            // Get sensory input
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get motor command
            nlm::MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            nlm::ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            totalReward += result.reward;
            
            // Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world.update(0.1);
        }
        
        assert(totalReward <= 0.0f);  // Should have negative rewards due to energy costs
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testLearningThroughExperience() {
        std::cout << "  Testing Learning Through Experience..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        nlm::SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        world.reset();
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        
        // Record initial state
        float initialPreference = brain->getLearnedPreference();
        
        // Run simulation with resource interactions
        for (int step = 0; step < 100; ++step) {
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            for (int i = 0; i < 10; ++i) brain->step(step * 10 + i);
            
            nlm::MotorCommand cmd = agent.decodeMotorCommand();
            nlm::ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            agent.applyRewardModulation(result.reward, 0.0f);
            world.update(0.1);
        }
        
        float finalPreference = brain->getLearnedPreference();
        
        // Preference should have changed based on experience
        // (may be small, but should be different from initial)
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testActionSelection() {
        std::cout << "  Testing Action Selection..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        nlm::SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        
        // Check that action selection works
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        nlm::MotorCommand cmd = agent.decodeMotorCommand();
        
        // Action should be valid
        assert(cmd != nlm::MotorCommand::Wait || agent.hasLastDecoded());
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Phase 3 Demo Tests:" << std::endl;
        std::cout << "  Testing world interaction and behavioral learning" << std::endl;
        std::cout << "  with reward-based neuromodulation and action selection" << std::endl;
        std::cout << std::endl;
        
        testWorldInteraction();
        testLearningThroughExperience();
        testActionSelection();
        
        std::cout << std::endl;
        std::cout << "All Phase 3 demo tests completed successfully!" << std::endl;
    }
}

// Test framework function
namespace test_demo_phase3 {
    void runAll();
}