// Comprehensive Test Suite for NLM Brain Simulation
// Unit tests for plasticity rules and neural dynamics

#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace test {

// Forward declarations for test classes
class STDPTest;
class HebbianTest;
class StructuralPlasticityTest;
class PlasticityRuleTest;
class NeuronTest;
class SpikeSystemTest;
class NeuralPopulationTest;
class NeuralRegionTest;
class SynapseTest;
class BrainTest;
class IntegrationPlasticityNeuralTest;
class IntegrationMemoryPlasticityTest;
class IntegrationNeuromodulationTest;
class PerformanceTest;

// Test Suite Manager
class TestSuite {
public:
    TestSuite() {
        std::cout << "=== NLM Brain Simulation Comprehensive Test Suite ===" << std::endl;
    }
    
    void runAllTests() {
        runPlasticityRuleTests();
        runNeuralDynamicsTests();
        runIntegrationTests();
        runPerformanceTests();
        
        std::cout << std::endl;
        std::cout << "=== Test Suite Summary ===" << std::endl;
        std::cout << "✓ All plasticity rule tests completed" << std::endl;
        std::cout << "✓ All neural dynamics tests completed" << std::endl;
        std::cout << "✓ All integration tests completed" << std::endl;
        std::cout << "✓ All performance tests completed" << std::endl;
        std::cout << std::endl;
        std::cout << "All test suites completed successfully!" << std::endl;
    }
    
private:
    void runPlasticityRuleTests() {
        std::cout << "\n=== Plasticity Rule Tests ===" << std::endl;
        
        // Create test instances
        STDPTest stdpTest;
        HebbianTest hebbianTest;
        StructuralPlasticityTest structuralTest;
        PlasticityRuleTest plasticityRuleTest;
        
        stdpTest.run();
        hebbianTest.run();
        structuralTest.run();
        plasticityRuleTest.run();
    }
    
    void runNeuralDynamicsTests() {
        std::cout << "\n=== Neural Dynamics Tests ===" << std::endl;
        
        NeuronTest neuronTest;
        SynapseTest synapseTest;
        NeuralPopulationTest populationTest;
        NeuralRegionTest regionTest;
        BrainTest brainTest;
        
        neuronTest.run();
        synapseTest.run();
        populationTest.run();
        regionTest.run();
        brainTest.run();
    }
    
    void runIntegrationTests() {
        std::cout << "\n=== Integration Tests ===" << std::endl;
        
        IntegrationPlasticityNeuralTest integration1;
        IntegrationMemoryPlasticityTest integration2;
        IntegrationNeuromodulationTest integration3;
        
        integration1.run();
        integration2.run();
        integration3.run();
    }
    
    void runPerformanceTests() {
        std::cout << "\n=== Performance Tests ===" << std::endl;
        
        PerformanceTest performanceTest;
        performanceTest.run();
    }
};

// Base Test Class for all tests
class BaseTest {
protected:
    std::string testName;
    
public:
    virtual ~BaseTest() = default;
    
    virtual void run() = 0;
    
protected:
    template<typename T, typename U>
    void assertEqual(const T& expected, const U& actual, const std::string& message = "") {
        if (expected != actual) {
            std::cerr << "FAIL: " << message 
                      << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    template<typename T>
    void assertFloatEqual(T expected, T actual, T tolerance, const std::string& message = "") {
        if (std::abs(expected - actual) > tolerance) {
            std::cerr << "FAIL: " << message 
                      << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            std::cerr << "FAIL: " << message << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            std::cerr << "FAIL: " << message << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
};

// Test macros for easy test creation
#define TEST_CASE(testName, testClass) \
    class testName : public testClass { \
    public: void run() override { testBody(); } \
    protected: virtual void testBody()

#define RUN_ALL_TESTS(suite) suite.runAllTests()

}}} // namespace nlm::test
