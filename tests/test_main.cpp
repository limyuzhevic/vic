// Comprehensive Test Framework for NLM
// This file contains the main test runner and utilities

#include "test_neuron.cpp"
#include "test_stdp.cpp"
#include "test_config.cpp"
#include "test_brain.cpp"
#include "test_synapse.cpp"
#include "test_clock.cpp"
#include "test_random.cpp"
#include "test_types.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <functional>

namespace test_framework {
    
    struct TestResult {
        std::string name;
        std::string category;
        bool passed;
        std::string message;
        double duration;  // in seconds
    };
    
    class TestRunner {
    private:
        std::vector<TestResult> results;
        int totalTests;
        int passedTests;
        int failedTests;
        
    public:
        TestRunner() : totalTests(0), passedTests(0), failedTests(0) {}
        
        void runTest(const std::string& name, const std::string& category, 
                    std::function<void()> testFunc) {
            auto startTime = std::chrono::high_resolution_clock::now();
            bool passed = false;
            std::string message;
            
            try {
                testFunc();
                passed = true;
                message = "PASSED";
            } catch (const std::exception& e) {
                passed = false;
                message = std::string("FAILED: ") + e.what();
            } catch (...) {
                passed = false;
                message = "FAILED: Unknown error";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult result{name, category, passed, message, duration};
            results.push_back(result);
            
            totalTests++;
            if (passed) {
                passedTests++;
                std::cout << "✓ " << name << " " << message << " (" << std::fixed 
                          << std::setprecision(4) << duration << "s)" << std::endl;
            } else {
                failedTests++;
                std::cout << "✗ " << name << " " << message << " (" << std::fixed 
                          << std::setprecision(4) << duration << "s)" << std::endl;
            }
        }
        
        void runAllTests() {
            std::cout << std::endl;
            std::cout << "=" << std::string(60, '=') << std::endl;
            std::cout << "NLM Test Suite - Neural Learning Machine" << std::endl;
            std::cout << "=" << std::string(60, '=') << std::endl;
            std::cout << std::endl;
            
            // Neuron tests
            std::cout << "--- Neuron Tests ---" << std::endl;
            runTest("Neuron Creation", "Neuron", test_neuron::testNeuronCreation);
            runTest("Neuron Type Setting", "Neuron", test_neuron::testNeuronType);
            runTest("Membrane Potential", "Neuron", test_neuron::testMembranePotential);
            runTest("Firing State Management", "Neuron", test_neuron::testFiringState);
            runTest("Refractory Period", "Neuron", test_neuron::testRefractoryDecrement);
            runTest("Current Injection", "Neuron", test_neuron::testCurrentInjection);
            runTest("Synaptic Input Handling", "Neuron", test_neuron::testSynapticInput);
            runTest("Spike History", "Neuron", test_neuron::testSpikeHistory);
            runTest("Random Initialization", "Neuron", test_neuron::testRandomInitialization);
            runTest("Reset Functionality", "Neuron", test_neuron::testReset);
            
            // Synapse tests
            std::cout << std::endl << "--- Synapse Tests ---" << std::endl;
            runTest("Synapse Creation", "Synapse", test_synapse::testSynapseCreation);
            runTest("Synapse Properties", "Synapse", test_synapse::testSynapseProperties);
            runTest("Weight Management", "Synapse", test_synapse::testWeightManagement);
            runTest("Plasticity Flags", "Synapse", test_synapse::testPlasticityFlags);
            runTest("Spike History", "Synapse", test_synapse::testSynapseSpikeHistory);
            runTest("Reset Functionality", "Synapse", test_synapse::testSynapseReset);
            
            // STDP tests
            std::cout << std::endl << "--- STDP Tests ---" << std::endl;
            runTest("STDP Creation", "STDP", test_stdp::testSTDPCreation);
            runTest("STDP Configuration", "STDP", test_stdp::testSTDPConfiguration);
            runTest("STDP Configure Method", "STDP", test_stdp::testSTDPConfigure);
            runTest("STDP Name", "STDP", test_stdp::testSTDPName);
            runTest("STDP Potentiation", "STDP", test_stdp::testSTDPPotentiation);
            runTest("STDP Depression", "STDP", test_stdp::testSTDPDepression);
            runTest("STDP Empty Spikes", "STDP", test_stdp::testSTDPEmptySpikes);
            runTest("STDP Weight Bounds", "STDP", test_stdp::testSTDPWeightBounds);
            
            // Configuration tests
            std::cout << std::endl << "--- Configuration Tests ---" << std::endl;
            runTest("Config Creation", "Config", test_config::testConfigCreation);
            runTest("Config Value Setting", "Config", test_config::testConfigValueSetting);
            runTest("Config Value Getting", "Config", test_config::testConfigValueGetting);
            runTest("Config Has Key", "Config", test_config::testConfigHasKey);
            runTest("Config Load/Save", "Config", test_config::testConfigLoadSave);
            
            // Brain tests
            std::cout << std::endl << "--- Brain Tests ---" << std::endl;
            runTest("Brain Creation", "Brain", test_brain::testBrainCreation);
            runTest("Brain Initialization", "Brain", test_brain::testBrainInitialization);
            runTest("Brain Simulation Steps", "Brain", test_brain::testBrainStep);
            runTest("Brain Reset", "Brain", test_brain::testBrainReset);
            runTest("Brain Regions", "Brain", test_brain::testBrainRegions);
            runTest("Brain Action Production", "Brain", test_brain::testBrainActionProduction);
            runTest("Brain Status", "Brain", test_brain::testBrainStatus);
            
            // Clock tests
            std::cout << std::endl << "--- Clock Tests ---" << std::endl;
            runTest("Clock Creation", "Clock", test_clock::testClockCreation);
            runTest("Clock Timing", "Clock", test_clock::testClockTiming);
            runTest("Clock Simulation Step", "Clock", test_clock::testClockSimulationStep);
            
            // Random tests
            std::cout << std::endl << "--- Random Tests ---" << std::endl;
            runTest("Random Generator", "Random", test_random::testRandomGenerator);
            
            // Type tests
            std::cout << std::endl << "--- Type Tests ---" << std::endl;
            runTest("Type Definitions", "Type", test_types::testTypeDefinitions);
            
            // Print summary
            std::cout << std::endl;
            std::cout << "=" << std::string(60, '=') << std::endl;
            std::cout << "Test Summary:" << std::endl;
            std::cout << "  Total Tests: " << totalTests << std::endl;
            std::cout << "  Passed:      " << passedTests << std::endl;
            std::cout << "  Failed:      " << failedTests << std::endl;
            std::cout << "  Success Rate: " << std::fixed << std::setprecision(1)
                      << (passedTests * 100.0 / totalTests) << "%" << std::endl;
            std::cout << "=" << std::string(60, '=') << std::endl;
            
            if (failedTests > 0) {
                std::cout << std::endl;
                std::cout << "FAILED TESTS:" << std::endl;
                for (const auto& result : results) {
                    if (!result.passed) {
                        std::cout << "  - " << result.name << " (" << result.category << ")";
                        if (!result.message.empty() && result.message != "PASSED") {
                            std::cout << ": " << result.message;
                        }
                        std::cout << std::endl;
                    }
                }
            }
            
            std::cout << std::endl;
        }
        
        bool allPassed() const {
            return failedTests == 0;
        }
        
        int getTotalTests() const { return totalTests; }
        int getPassedTests() const { return passedTests; }
        int getFailedTests() const { return failedTests; }
        
        const std::vector<TestResult>& getResults() const { return results; }
    };
    
} // namespace test_framework

// Main test runner function
int main() {
    std::cout << "Starting NLM test suite..." << std::endl;
    
    test_framework::TestRunner runner;
    runner.runAllTests();
    
    return runner.allPassed() ? 0 : 1;
}
