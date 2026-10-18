// Performance Memory Usage Tests
// Memory efficiency tests

#include "test_framework.hpp"
#include "../src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_performance {

// Memory usage tests
class MemoryTests : public TestSuite {
public:
    MemoryTests() : TestSuite("Performance::Memory") {}
    
    void run() override {
        std::cout << "  Testing memory efficiency..." << std::endl;
        
        TIME_TEST("Memory Allocation Patterns", {
            std::cout << "    Testing various memory allocation patterns..." << std::endl;
            
            // Test 1: Many small allocations
            std::vector<nlm::Config> smallConfigs;
            smallConfigs.reserve(1000);
            
            for (int i = 0; i < 1000; ++i) {
                smallConfigs.emplace_back();
                smallConfigs.back().set("small.key", i, nlm::ConfigSource::Runtime);
            }
            
            // Verify all small configs
            int smallValid = 0;
            for (auto& config : smallConfigs) {
                if (config.get<int>("small.key").has_value()) {
                    smallValid++;
                }
            }
            
            assert(smallValid == 1000);
            
            // Clear small configs
            smallConfigs.clear();
            
            // Test 2: Large config with many entries
            nlm::Config largeConfig;
            for (int i = 0; i < 1000; ++i) {
                largeConfig.set(std::string("large.key.") + std::to_string(i), i, nlm::ConfigSource::Runtime);
            }
            
            // Verify large config
            int largeValid = 0;
            for (int i = 0; i < 1000; ++i) {
                if (largeConfig.get<int>("large.key." + std::to_string(i)).has_value()) {
                    largeValid++;
                }
            }
            
            assert(largeValid == 1000);
            
            std::cout << "    Memory allocation patterns test completed" << std::endl;
        });
        
        TIME_TEST("Memory Cleanup Test", {
            // Create and destroy configs to test cleanup
            for (int cycle = 0; cycle < 20; ++cycle) {
                std::vector<nlm::Config> tempConfigs;
                
                // Create temporary configs
                for (int i = 0; i < 100; ++i) {
                    tempConfigs.emplace_back();
                    tempConfigs.back().set("temp.key", i, nlm::ConfigSource::Runtime);
                }
                
                // Use the configs
                for (auto& config : tempConfigs) {
                    auto value = config.get<int>("temp.key");
                    assert(value.has_value());
                }
                
                // Explicit cleanup
                tempConfigs.clear();
                std::vector<nlm::Config>().swap(tempConfigs);
                
                if (cycle % 5 == 0) {
                    std::cout << "    Completed " << (cycle + 1) * 100 << " cleanup cycles" << std::endl;
                }
            }
            
            std::cout << "    Memory cleanup test completed successfully" << std::endl;
        });
        
        std::cout << "  Memory tests PASSED" << std::endl;
    }
};

// Global test instances
static MemoryTests memoryTests;

void runAll() {
    std::cout << "=== Performance Memory Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&memoryTests);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Performance Memory Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Performance Memory Tests FAILED ===" << std::endl;
    }
}

} // namespace test_performance
