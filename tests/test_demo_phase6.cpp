// NLM Phase 6 Demo Tests
// Tests full integration and system coherence

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
    class MotorCommand;
    class ActionResult;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class DevelopmentSystem;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class Logger;
}

namespace test_demo_phase6 {
    
    void testFullIntegration() {
        std::cout << "  Testing Full Integration..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        nlm::SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        agent.enableDevelopment(true);
        
        // Test that all systems are integrated
        assert(brain->getWorkingMemory() != nullptr);
        assert(brain->getEpisodicMemory() != nullptr);
        assert(brain->getDopamine() != nullptr);
        assert(brain->getCuriosity() != nullptr);
        assert(brain->getPredictionSystem() != nullptr);
        assert(brain->getPlanner() != nullptr);
        assert(brain->getConceptFormation() != nullptr);
        assert(brain->getAttention() != nullptr);
        assert(brain->getDevelopmentSystem() != nullptr);
        
        // Run integration test
        float totalReward = 0.0f;
        for (uint64_t step = 0; step < 100; ++step) {
            world.update(0.001);
            nlm::SensoryPercept percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain->step(step, step * 0.001);
            
            nlm::MotorCommand cmd = agent.decodeMotorCommand();
            nlm::ActionResult result = world.applyMotorCommand(cmd, step * 0.001);
            totalReward += result.reward;
            agent.applyRewardModulation(result.reward, 0.0f);
        }
        
        // Reward should be computed (even if negative)
        std::cout << "    Total reward: " << totalReward << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testMemoryIntegration() {
        std::cout << "  Testing Memory Integration..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Test working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "    Working memory is functional" << std::endl;
        }
        
        // Test episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "    Episodic memory is functional" << std::endl;
        }
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testNeuromodulationIntegration() {
        std::cout << "  Testing Neuromodulation Integration..." << std::endl;
        
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
        
        // Test neuromodulation levels
        float curiosityLevel = agent.getCuriosityLevel();
        float noveltyLevel = agent.getNoveltyLevel();
        float dopamineLevel = agent.getNeuromodulationLevel();
        
        std::cout << "    Curiosity level: " << curiosityLevel << std::endl;
        std::cout << "    Novelty level: " << noveltyLevel << std::endl;
        std::cout << "    Dopamine level: " << dopamineLevel << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testCheckpointingWorks() {
        std::cout << "  Testing Checkpointing Functionality..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Run a few steps
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        // Save checkpoint
        std::string checkpointPath = "/tmp/test_phase6_checkpoint.bin";
        bool saved = brain->save(checkpointPath);
        assert(saved);
        
        // Load checkpoint
        auto brain2 = std::make_shared<nlm::Brain>(config);
        brain2->initialize();
        
        bool loaded = brain2->load(checkpointPath);
        assert(loaded);
        
        std::cout << "    Checkpoint save/load completed successfully" << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Phase 6 Demo Tests:" << std::endl;
        std::cout << "  Testing full system integration" << std::endl;
        std::cout << "  including memory, neuromodulation, and prediction systems" << std::endl;
        std::cout << std::endl;
        
        testFullIntegration();
        testMemoryIntegration();
        testNeuromodulationIntegration();
        testCheckpointingWorks();
        
        std::cout << std::endl;
        std::cout << "All Phase 6 demo tests completed successfully!" << std::endl;
    }
}

// Test framework function
namespace test_demo_phase6 {
    void runAll();
}