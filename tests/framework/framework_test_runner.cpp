// Unit test framework test runner
#include <iostream>
#include <string>
#include <map>
#include <functional>

class TestFramework {
private:
    std::map<std::string, std::function<void()>> tests;
    
public:
    TestFramework() {
        // Register all tests
        registerTest("config", test_config_unit::runAllConfigUnitTests);
        registerTest("logger", test_logger_unit::runAllLoggerUnitTests);
        registerTest("random", test_random_unit::runAllRandomUnitTests);
        registerTest("clock", test_clock_unit::runAllClockUnitTests);
        registerTest("types", test_types_unit::runAllTypesUnitTests);
        registerTest("brain", test_brain_unit::runAllBrainUnitTests);
        registerTest("neuron", test_neuron_unit::runAllNeuronUnitTests);
        registerTest("synapse", test_synapse_unit::runAllSynapseUnitTests);
        registerTest("neural_population", test_neural_population_unit::runAllNeuralPopulationUnitTests);
        registerTest("neural_region", test_neural_region_unit::runAllNeuralRegionUnitTests);
        registerTest("stdp", test_stdp_unit::runAllSTDPUnitTests);
        registerTest("hebbian", test_hebbian_unit::runAllHebbianUnitTests);
        registerTest("structural_plasticity", test_structural_plasticity_unit::runAllStructuralPlasticityUnitTests);
    }
    
    void registerTest(const std::string& name, std::function<void()> testFunc) {
        tests[name] = testFunc;
    }
    
    bool runTest(const std::string& testName) {
        auto it = tests.find(testName);
        if (it == tests.end()) {
            std::cout << "Test not found: " << testName << std::endl;
            return false;
        }
        
        std::cout << "Running test: " << testName << std::endl;
        try {
            it->second();
            std::cout << "Test passed: " << testName << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cout << "Test failed: " << testName << " - " << e.what() << std::endl;
            return false;
        }
    }
    
    bool runAllTests() {
        std::cout << "=== Running All Unit Tests ===" << std::endl;
        
        bool allPassed = true;
        for (const auto& pair : tests) {
            bool passed = runTest(pair.first);
            if (!passed) {
                allPassed = false;
            }
        }
        
        if (allPassed) {
            std::cout << "=== All tests passed! ===" << std::endl;
        } else {
            std::cout << "=== Some tests failed ===" << std::endl;
        }
        
        return allPassed;
    }
    
    void listTests() {
        std::cout << "Available tests:" << std::endl;
        for (const auto& pair : tests) {
            std::cout << "  - " << pair.first << std::endl;
        }
    }
};

// Global test framework instance
TestFramework g_testFramework;

// Test harness functions
void runTestByName(const std::string& testName) {
    g_testFramework.runTest(testName);
}

void runAllFrameworkTests() {
    g_testFramework.runAllTests();
}

void listFrameworkTests() {
    g_testFramework.listTests();
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Framework Test Runner ===" << std::endl;
    
    if (argc == 1) {
        // Run all tests
        runAllFrameworkTests();
    } else if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "--list") {
            listFrameworkTests();
        } else {
            runTestByName(arg);
        }
    } else {
        std::cout << "Usage: " << argv[0] << " [--list | test_name]" << std::endl;
        return 1;
    }
    
    return 0;
}