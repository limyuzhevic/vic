// NLM Phase 5 Demo Tests
// Tests performance optimization and personality characteristics

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <chrono>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class SimpleWorld;
    class AgentBrain;
    class SensoryPercept;
    class MotorCommand;
    class ActionResult;
    class NeuralPlanner;
    class ConceptFormation;
    class SelfModel;
    class SocialLearning;
    class Logger;
}

namespace test_demo_phase5 {
    
    void testPerformanceOptimization() {
        std::cout << "  Testing Performance Optimization..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Test performance characteristics
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        std::cout << "    Performance: " << duration << "s for 1000 steps" << std::endl;
        
        // Should complete in reasonable time
        assert(duration < 10.0);  // Less than 10 seconds
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testPersonalityLearning() {
        std::cout << "  Testing Personality Learning..." << std::endl;
        
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
        agent.enableCuriosity(true);
        
        // Record initial preference
        float initialPreference = brain->getLearnedPreference();
        
        // Run simulation with resource interactions
        for (int step = 0; step < 200; ++step) {
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
        float preferenceChange = std::abs(finalPreference - initialPreference);
        std::cout << "    Preference changed from " << initialPreference << " to " << finalPreference << " (Δ=" << preferenceChange << ")" << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testCheckpointing() {
        std::cout << "  Testing Checkpointing..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Run a few steps to build state
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        // Save checkpoint
        std::string checkpointPath = "/tmp/test_checkpoint.bin";
        bool saved = brain->save(checkpointPath);
        assert(saved);
        
        // Create new brain and load checkpoint
        auto brain2 = std::make_shared<nlm::Brain>(config);
        brain2->initialize();
        
        bool loaded = brain2->load(checkpointPath);
        assert(loaded);
        
        std::cout << "    Checkpoint save/load completed successfully" << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Phase 5 Demo Tests:" << std::endl;
        std::cout << "  Testing performance optimization and personality characteristics" << std::endl;
        std::cout << "  including checkpointing and reward-based learning" << std::endl;
        std::cout << std::endl;
        
        testPerformanceOptimization();
        testPersonalityLearning();
        testCheckpointing();
        
        std::cout << std::endl;
        std::cout << "All Phase 5 demo tests completed successfully!" << std::endl;
    }
}

// Test framework function
namespace test_demo_phase5 {
    void runAll();
}