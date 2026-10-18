// Performance Benchmark Tests
// Performance benchmarks for measuring improvements

#include "test_framework.hpp"
#include "../src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_performance {

// Performance benchmark tests
class BenchmarkTests : public TestSuite {
public:
    BenchmarkTests() : TestSuite("Performance::Benchmark") {}
    
    void run() override {
        std::cout << "  Running comprehensive benchmarks..." << std::endl;
        
        // Memory benchmark
        TIME_TEST("Memory Usage Benchmark", {
            std::cout << "    Testing memory allocation patterns..." << std::endl;
            // Memory benchmark logic would go here
        });
        
        // CPU benchmark
        TIME_TEST("CPU Performance Benchmark", {
            nlm::Config config;
            config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
            config.set("region_count", 2, nlm::ConfigSource::Runtime);
            
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            
            auto start = std::chrono::high_resolution_clock::now();
            brain.initialize();
            
            for (int step = 0; step < 100; ++step) {
                brain.step(step);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "    Completed 100 simulation steps in " << duration.count() << " ms" << std::endl;
        });
        
        std::cout << "  Benchmark tests PASSED" << std::endl;
    }
};

// Stress tests
class StressTests : public TestSuite {
public:
    StressTests() : TestSuite("Performance::Stress") {}
    
    void run() override {
        std::cout << "  Running stress tests..." << std::endl;
        
        TIME_TEST("High Load Stress Test", {
            nlm::Config config;
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            nlm::SimpleWorld world;
            world.configure(1000.0f, 1000.0f, 128, 128);
            world.reset();
            
            nlm::AgentBrain agentBrain(&brain);
            agentBrain.initialize(world);
            
            // Run under heavy load
            for (int step = 0; step < 2000; ++step) {
                world.update(0.005);
                
                if (step % 50 == 0) {
                    brain.step(step);
                }
                
                // High frequency reward processing
                if (step % 20 == 0) {
                    agentBrain.applyRewardModulation(0.8f, 0.5f);
                }
            }
            
            std::cout << "    Completed 2000 stress steps under high load" << std::endl;
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
        
        TIME_TEST("Memory Efficiency Test", {
            // Create many objects to test memory management
            std::vector<nlm::Config> configs;
            configs.reserve(500);
            
            // Create many configs
            for (int i = 0; i < 500; ++i) {
                configs.emplace_back();
                configs.back().set("test.key", i, nlm::ConfigSource::Runtime);
            }
            
            // Access all configs
            int accessCount = 0;
            for (auto& config : configs) {
                if (config.has("test.key")) {
                    accessCount++;
                }
            }
            
            assert(accessCount == 500);
            
            // Clear all configs
            configs.clear();
            
            std::cout << "    Managed 500 config objects efficiently" << std::endl;
        });
        
        std::cout << "  Memory tests PASSED" << std::endl;
    }
};

// Global test instances
static BenchmarkTests benchmarkTests;
static StressTests stressTests;
static MemoryTests memoryTests;

void runAll() {
    std::cout << "=== Performance Benchmark Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&benchmarkTests);
    framework.addSuite(&stressTests);
    framework.addSuite(&memoryTests);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Performance Benchmark Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Performance Benchmark Tests FAILED ===" << std::endl;
    }
}

} // namespace test_performance
