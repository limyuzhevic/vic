// NLM Phase 6 Benchmark Tests
// Tests performance and scaling characteristics

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
    class Logger;
}

namespace test_demo_phase6_benchmark {
    
    void testBrainPerformance() {
        std::cout << "  Testing Brain Performance..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.05f);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Measure performance
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = 1000.0 / duration;
        
        std::cout << "    Performance: " << stepsPerSecond << " steps/second" << std::endl;
        
        // Should complete in reasonable time
        assert(stepsPerSecond > 100.0);  // At least 100 steps per second
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testMemoryUsage() {
        std::cout << "  Testing Memory Usage..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Check memory usage metrics
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        
        std::cout << "    Neurons: " << neuronCount << std::endl;
        std::cout << "    Synapses: " << synapseCount << std::endl;
        
        // Should be reasonable
        assert(neuronCount == 500);
        assert(synapseCount > 0);
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void testScalability() {
        std::cout << "  Testing Scalability..." << std::endl;
        
        // Test with different brain sizes
        std::vector<size_t> neuronCounts = {100, 500, 1000};
        
        for (size_t neuronCount : neuronCounts) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", neuronCount);
            config->set("region_count", 2);
            config->set("connection_probability", 0.05f);
            config->set("random_seed", 42);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            assert(brain->initialize());
            
            std::cout << "    " << neuronCount << " neurons: " << brain->getTotalSynapseCount() << " synapses" << std::endl;
            
            // Brain should initialize successfully
            assert(brain->getTotalNeuronCount() == neuronCount);
        }
        
        std::cout << "    PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Phase 6 Benchmark Tests:" << std::endl;
        std::cout << "  Testing performance, memory usage, and scalability characteristics" << std::endl;
        std::cout << std::endl;
        
        testBrainPerformance();
        testMemoryUsage();
        testScalability();
        
        std::cout << std::endl;
        std::cout << "All Phase 6 benchmark tests completed successfully!" << std::endl;
    }
}

// Test framework function
namespace test_demo_phase6_benchmark {
    void runAll();
}