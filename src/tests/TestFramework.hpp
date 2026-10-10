#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>

// Forward declarations for NLM classes
namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class WorldObject;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class PredictionSystem;
    class DevelopmentSystem;
    class Dopamine;
    class Acetylcholine;
    class Norepinephrine;
    class Serotonin;
    class Curiosity;
    class Novelty;
    class PredictionError;
}

// Base test class
class Test {
protected:
    std::string name;
    std::string description;
public:
    Test(const std::string& testName, const std::string& testDescription)
        : name(testName), description(testDescription) {}
    
    virtual ~Test() = default;
    
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    
    virtual bool run() = 0;
    virtual void cleanup() {}
};

// Test suite for managing multiple tests
class TestSuite {
private:
    std::vector<std::unique_ptr<Test>> tests;
public:
    void registerTest(std::unique_ptr<Test> test) {
        tests.push_back(std::move(test));
    }
    
    void runAll() {
        int passed = 0;
        int total = tests.size();
        
        std::cout << "Running " << total << " tests..." << std::endl;
        std::cout << "=" << std::string(50, '=') << std::endl;
        
        for (auto& test : tests) {
            std::cout << "Test: " << test->getName() << std::endl;
            std::cout << "Description: " << test->getDescription() << std::endl;
            
            bool success = test->run();
            
            if (success) {
                std::cout << "Result: " << test->getName() << " ✓ PASS" << std::endl;
                passed++;
            } else {
                std::cout << "Result: " << test->getName() << " ✗ FAIL" << std::endl;
            }
            
            std::cout << std::string(50, '-') << std::endl;
            
            test->cleanup();
        }
        
        std::cout << "=" << std::string(50, '=') << std::endl;
        std::cout << "Summary: " << passed << "/" << total << " tests passed" << std::endl;
        
        if (passed == total) {
            std::cout << "All tests PASSED! ✓" << std::endl;
        } else {
            std::cout << "Some tests FAILED! ✗" << std::endl;
        }
    }
};
