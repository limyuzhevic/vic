// Performance Baseline Tests
// Baseline performance tests for comparison

#include "test_framework.hpp"
#include "../src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_performance {

// Baseline tests
class BaselineTests : public TestSuite {
public:
    BaselineTests() : TestSuite("Performance::Baseline") {}
    
    void run() override {
        std::cout << "  Running baseline performance tests..." << std::endl;
        
        TIME_TEST("Baseline Config Performance", {
            // Measure baseline config performance
            nlm::Config config;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < 1000; ++i) {
                config.set(std::string("key_") + std::to_string(i), i, nlm::ConfigSource::Runtime);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            std::cout << "    Baseline config performance: " << duration.count() << " us for 1000 operations" << std::endl;
        });
        
        TIME_TEST("Baseline Brain Performance", {
            nlm::Config config;
            config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
            config.set("region_count", 2, nlm::ConfigSource::Runtime);
            
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            
            auto start = std::chrono::high_resolution_clock::now();
            brain.initialize();
            
            for (int step = 0; step < 50; ++step) {
                brain.step(step);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "    Baseline brain performance: " << duration.count() << " ms for 50 steps" << std::endl;
        });
        
        std::cout << "  Baseline tests PASSED" << std::endl;
    }
};

// Global test instances
static BaselineTests baselineTests;

void runAll() {
    std::cout << "=== Performance Baseline Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&baselineTests);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Baseline Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Baseline Tests FAILED ===" << std::endl;
    }
}

} // namespace test_performance
