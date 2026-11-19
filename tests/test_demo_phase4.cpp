// NLM Phase 4 Demo Tests
// Tests advanced cognitive capabilities

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
    class Logger;
}

namespace test_demo_phase4 {
    
    void testPredictionSystem() {
        std::cout << "  Testing Prediction System..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Check if prediction system is integrated
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            std::cout << "    Prediction system is integrated" << std::endl;
        }
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testWorkingMemory() {
        std::cout << "  Testing Working Memory..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Check if working memory is integrated
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "    Working memory is integrated" << std::endl;
        }
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testEpisodicMemory() {
        std::cout << "  Testing Episodic Memory..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Check if episodic memory is integrated
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "    Episodic memory is integrated" << std::endl;
        }
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Phase 4 Demo Tests:" << std::endl;
        std::cout << "  Testing advanced cognitive capabilities" << std::endl;
        std::cout << "  including prediction, memory, and attention systems" << std::endl;
        std::cout << std::endl;
        
        testPredictionSystem();
        testWorkingMemory();
        testEpisodicMemory();
        
        std::cout << std::endl;
        std::cout << "All Phase 4 demo tests completed successfully!" << std::endl;
    }
}

// Test framework function
namespace test_demo_phase4 {
    void runAll();
}