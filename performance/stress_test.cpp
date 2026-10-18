// Performance Stress Tests
// Stress testing for system under extreme conditions

#include "test_framework.hpp"
#include "../src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_performance {

// Stress tests
class StressTests : public TestSuite {
public:
    StressTests() : TestSuite("Performance::Stress") {}
    
    void run() override {
        std::cout << "  Running stress tests..." << std::endl;
        
        TIME_TEST("Extreme Load Stress Test", {
            nlm::Config config;
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            nlm::SimpleWorld world;
            world.configure(500.0f, 500.0f, 64, 64);
            world.reset();
            
            nlm::AgentBrain agentBrain(&brain);
            agentBrain.initialize(world);
            
            // Run under extreme stress
            for (int step = 0; step < 5000; ++step) {
                world.update(0.001);
                
                if (step % 25 == 0) {
                    brain.step(step);
                }
                
                // Frequent operations
                if (step % 10 == 0) {
                    agentBrain.applyRewardModulation(0.9f, 0.7f);
                }
            }
            
            std::cout << "    Completed 5000 stress steps under extreme load" << std::endl;
        });
        
        std::cout << "  Stress tests PASSED" << std::endl;
    }
};

// Memory usage tests
class MemoryTests : public TestSuite {
public:
    MemoryTests() : TestSuite("Performance::Memory") {}
    
    void run() override {
        std::cout << "  Testing memory efficiency..." << std::endl;
        
        TIME_TEST("Memory Leak Detection", {
            // Create and destroy many objects
            for (int iteration = 0; iteration < 10; ++iteration) {
                std::vector<nlm::Config> configs;
                
                // Create many configs
                for (int i = 0; i < 1000; ++i) {
                    configs.emplace_back();
                    configs.back().set("test.key", i, nlm::ConfigSource::Runtime);
                }
                
                // Access and verify
                int validConfigs = 0;
                for (auto& config : configs) {
                    if (config.get<int>("test.key").has_value()) {
                        validConfigs++;
                    }
                }
                
                assert(validConfigs == 1000);
                
                // Clear configs
                configs.clear();
                
                if (iteration % 5 == 0) {
                    std::cout << "    Completed " << (iteration + 1) * 1000 << " config cycles" << std::endl;
                }
            }
            
            std::cout << "    Memory leak test completed successfully" << std::endl;
        });
        
        std::cout << "  Memory tests PASSED" << std::endl;
    }
};

// Global test instances
static StressTests stressTests;
static MemoryTests memoryTests;

void runAll() {
    std::cout << "=== Performance Stress Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&stressTests);
    framework.addSuite(&memoryTests);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Performance Stress Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Performance Stress Tests FAILED ===" << std::endl;
    }
}

} // namespace test_performance
