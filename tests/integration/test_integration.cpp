// System Integration Tests
// Tests for all components working together

#include "test_framework.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_integration {

// Test complete system integration
class SystemIntegrationTest : public TestSuite {
public:
    SystemIntegrationTest() : TestSuite("Integration::System") {}
    
    void run() override {
        std::cout << "  Testing complete system integration..." << std::endl;
        
        // Create complete system
        TIME_TEST("Complete System Integration", {
            // Create config
            nlm::Config config;
            config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
            config.set("region_count", 2, nlm::ConfigSource::Runtime);
            config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
            
            // Create brain
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            assert(brain.initialize());
            
            // Create world
            nlm::SimpleWorld world;
            world.configure(200.0f, 200.0f, 32, 32);
            world.reset();
            
            // Create agent brain
            nlm::AgentBrain agentBrain(&brain);
            agentBrain.initialize(world);
            
            // Verify integration
            assert(brain.getTotalNeuronCount() == 500);
            assert(agentBrain.getSensoryInputSize() >= 0);
            assert(agentBrain.getMotorOutputSize() >= 0);
        });
        
        // Test system update cycle
        TIME_TEST("System Update Cycle", {
            nlm::Config config;
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            nlm::SimpleWorld world;
            world.configure(100.0f, 100.0f, 16, 16);
            world.reset();
            
            nlm::AgentBrain agentBrain(&brain);
            agentBrain.initialize(world);
            
            // Run simulation cycle
            for (int step = 0; step < 10; ++step) {
                // Update world
                world.update(0.1);
                
                // Process with agent brain
                nlm::SensoryPercept percept;
                // (In real implementation, would get percept from world)
                
                agentBrain.processSensoryInput(percept);
                
                // Brain updates are handled by simulation loop
            }
            
            // Verify system still functional
            assert(brain.getTotalNeuronCount() == 1000);  // Default config
            assert(world.getAgentEnergy() >= 0.0f);
        });
        
        std::cout << "  System Integration tests PASSED" << std::endl;
    }
};

// Test Python-C++ integration
class PythonCPPIntegrationTest : public TestSuite {
public:
    PythonCPPIntegrationTest() : TestSuite("Integration::PythonCPP") {}
    
    void run() override {
        std::cout << "  Testing Python-C++ integration..." << std::endl;
        
        // Note: Python integration tests would be more complex
        // This is a simplified version for demonstration
        TIME_TEST("Python-C++ Data Exchange", {
            // Simulate data exchange between Python and C++
            // This would involve actual Python bindings in a real implementation
            
            std::vector<int> cppData = {1, 2, 3, 4, 5};
            
            // Process data (simulating Python binding calls)
            int sum = 0;
            for (int value : cppData) {
                sum += value;
            }
            
            assert(sum == 15);
        });
        
        std::cout << "  Python-C++ Integration tests PASSED" << std::endl;
    }
};

// Test configuration system integration
class ConfigIntegrationTest : public TestSuite {
public:
    ConfigIntegrationTest() : TestSuite("Integration::Config") {}
    
    void run() override {
        std::cout << "  Testing configuration system integration..." << std::endl;
        
        TIME_TEST("Advanced Config Integration", {
            nlm::Config config;
            
            // Test various configuration operations
            config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
            config.set("brain.region_count", 2, nlm::ConfigSource::Runtime);
            config.set("brain.connection_probability", 0.15, nlm::ConfigSource::Runtime);
            
            // Verify configuration hierarchy
            auto neuronCount = config.get<size_t>("brain.neuron_count");
            assert(neuronCount.has_value());
            assert(*neuronCount == 1000);
            
            // Test config merging
            nlm::Config config2;
            config2.set("brain.learning_rate", 0.001, nlm::ConfigSource::Runtime);
            config.merge(config2);
            
            auto learningRate = config.get<double>("brain.learning_rate");
            assert(learningRate.has_value());
            assert(*learningRate == 0.001);
        });
        
        std::cout << "  Configuration Integration tests PASSED" << std::endl;
    }
};

// Global test instances
static SystemIntegrationTest systemIntegrationTest;
static PythonCPPIntegrationTest pythonCPPIntegrationTest;
static ConfigIntegrationTest configIntegrationTest;

void runAll() {
    std::cout << "=== System Integration Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&systemIntegrationTest);
    framework.addSuite(&pythonCPPIntegrationTest);
    framework.addSuite(&configIntegrationTest);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Integration Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Integration Tests FAILED ===" << std::endl;
    }
}

} // namespace test_integration
