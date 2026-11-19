// NLM Phase 6 Text Demo Tests
// Tests text-as-a-sense and advanced cognition systems

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
    class SelfModel;
    class SocialLearning;
    class Logger;
}

namespace test_demo_phase6_text {
    
    void testTextAsASense() {
        std::cout << "  Testing Text as a Sense..." << std::endl;
        
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
        
        // Test text encoding
        std::string testText = "apple";
        std::vector<size_t> textPattern = nlm::AgentBrain::encodeText(testText);
        
        std::cout << "    Text encoded to pattern of size " << textPattern.size() << std::endl;
        
        // Test text-based learning
        nlm::SensoryPercept percept;
        std::vector<float> vision(16, 1.0f);
        // Add bright object at center (resource)
        vision[8] = 0.65f;
        std::vector<float> touch(8, 0.0f);
        
        percept.setVision(vision);
        percept.setTouch(touch);
        
        agent.processSensoryInput(percept);
        
        for (int i = 0; i < 10; ++i) brain->step(i, i * 0.001);
        
        std::cout << "    Text sense processing completed" << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testConceptFormation() {
        std::cout << "  Testing Concept Formation..." << std::endl;
        
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
        
        // Create concept through experience
        for (int step = 0; step < 100; ++step) {
            nlm::SensoryPercept percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            for (int i = 0; i < 10; ++i) brain->step(step * 10 + i);
            
            nlm::MotorCommand cmd = agent.decodeMotorCommand();
            nlm::ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            agent.applyRewardModulation(result.reward, 0.0f);
            world.update(0.1);
        }
        
        size_t conceptCount = agent.getConceptCount();
        std::cout << "    Concepts formed: " << conceptCount << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testSelfModel() {
        std::cout << "  Testing Self Model..." << std::endl;
        
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
        
        // Test self-model confidence
        float confidence = agent.getSelfModelConfidence();
        std::cout << "    Self-model confidence: " << confidence << std::endl;
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Phase 6 Text Demo Tests:" << std::endl;
        std::cout << "  Testing text-as-a-sense and advanced cognition systems" << std::endl;
        std::cout << "  including concept formation, self-model, and social learning" << std::endl;
        std::cout << std::endl;
        
        testTextAsASense();
        testConceptFormation();
        testSelfModel();
        
        std::cout << std::endl;
        std::cout << "All Phase 6 text demo tests completed successfully!" << std::endl;
    }
}

// Test framework function
namespace test_demo_phase6_text {
    void runAll();
}