#ifndef NLM_TEST_RUNNER_H
#define NLM_TEST_RUNNER_H

#include "core/TestingFramework.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {
namespace testing {

// Simple command line test runner
class TestRunner {
public:
    TestRunner() : totalTests(0), passedTests(0), failedTests(0) {}
    
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    
    void printUsage() {
        std::cout << "NLM Test Runner" << std::endl;
        std::cout << "================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm_test [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help              Show this help message" << std::endl;
        std::cout << "  --list              List all available tests" << std::endl;
        std::cout << "  --run-all           Run all tests" << std::endl;
        std::cout << "  --test=name         Run specific test by name" << std::endl;
        std::cout << "  --timeout=SECONDS   Set timeout for individual tests" << std::endl;
        std::cout << "  --output=FILE       Save test results to file" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --performance       Run performance benchmarks" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_test --list" << std::endl;
        std::cout << "  nlm_test --run-all" << std::endl;
        std::cout << "  nlm_test --test=" << getTestNameList() << std::endl;
        std::cout << "  nlm_test --performance" << std::endl;
    }
    
    void listTests() {
        std::cout << "Available tests:" << std::endl;
        std::cout << "----------------" << std::endl;
        
        auto& framework = TestFramework::getFramework();
        for (const auto& test : framework.getTests()) {
            std::cout << "  " << test->getName() << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Total tests: " << framework.getTestCount() << std::endl;
    }
    
    bool runTestByName(const std::string& testName) {
        auto& framework = TestFramework::getFramework();
        
        for (auto& test : framework.getTests()) {
            if (test->getName() == testName) {
                std::cout << "Running test: " << testName << std::endl;
                return test->run();
            }
        }
        
        std::cerr << "Error: Test not found: " << testName << std::endl;
        return false;
    }
    
    void runAllTests() {
        auto& framework = TestFramework::getFramework();
        framework.runAllTests();
        framework.printResults();
    }
    
    void runPerformanceTests() {
        std::cout << "Running performance benchmarks..." << std::endl;
        std::cout << "=============================" << std::endl;
        
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Test 1: Small network performance
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t numSteps = 100;
            
            for (SimulationStep step = 0; step < numSteps; ++step) {
                brain->step(step, brain->getTimestep() * (step + 1));
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            std::cout << "Small network (100 steps): " << duration << "s" << std::endl;
            std::cout << "  Steps per second: " << (numSteps / duration) << std::endl;
        }
        
        // Reset brain
        NeuralTestUtils::resetBrain(brain);
        
        // Test 2: Larger network performance
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t numSteps = 1000;
            
            for (SimulationStep step = 0; step < numSteps; ++step) {
                brain->step(step, brain->getTimestep() * (step + 1));
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            std::cout << "Medium network (1000 steps): " << duration << "s" << std::endl;
            std::cout << "  Steps per second: " << (numSteps / duration) << std::endl;
        }
        
        // Test 3: Memory usage (simplified)
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t numSteps = 500;
            
            // Create and destroy multiple brains to test memory allocation
            for (int i = 0; i < 10; ++i) {
                auto testBrain = NeuralTestUtils::createTestBrain();
                for (SimulationStep step = 0; step < 50; ++step) {
                    testBrain->step(step, testBrain->getTimestep() * (step + 1));
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            std::cout << "Memory allocation test (10x50 steps): " << duration << "s" << std::endl;
        }
        
        std::cout << "Performance benchmarks completed." << std::endl;
    }
    
private:
    std::string getTestNameList() {
        auto& framework = TestFramework::getFramework();
        std::vector<std::string> testNames;
        
        for (const auto& test : framework.getTests()) {
            testNames.push_back(test->getName());
        }
        
        if (testNames.empty()) return "";
        
        std::string result = testNames[0];
        for (size_t i = 1; i < testNames.size(); ++i) {
            result += "," + testNames[i];
        }
        
        return result;
    }
};

} // namespace testing
} // namespace nlm

#endif // NLM_TEST_RUNNER_H
