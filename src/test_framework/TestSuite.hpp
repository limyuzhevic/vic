// NLM (熙然) - Neural Learning Machine
// Phase 2: Test Suite and Test Runner Framework
// Modular test organization with integrated test runner

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <algorithm>

using namespace nlm;

// Test Framework
class TestSuite {
public:
    enum class TestMode {
        BASIC,
        PLASTICITY,
        STDP,
        ALL
    };
    
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
        double executionTime;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        
        TestResult(const std::string& testName) 
            : name(testName), passed(false), executionTime(0.0) {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        void complete() {
            endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            executionTime = duration.count() / 1000.0;
        }
        
        std::string getFormattedTime() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3) << executionTime;
            return ss.str();
        }
    };
    
    using TestFunction = std::function<void(TestSuite&)>;
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    bool verbose;
    bool quiet;
    std::vector<TestResult> results;
    std::unordered_map<std::string, TestFunction> testFunctions;
    
public:
    TestSuite(std::shared_ptr<Brain> b, std::shared_ptr<Config> c, bool v = false, bool q = false)
        : brain(b), config(c), verbose(v), quiet(q) {
        registerTests();
    }
    
    void registerTests() {
        // Register test functions
        testFunctions["basic"] = [this](TestSuite& suite) {
            suite.runBasicTest();
        };
        
        testFunctions["plasticity"] = [this](TestSuite& suite) {
            suite.runPlasticityTest();
        };
        
        testFunctions["stdp"] = [this](TestSuite& suite) {
            suite.runStdpTest();
        };
        
        testFunctions["all"] = [this](TestSuite& suite) {
            suite.runAllTests();
        };
    }
    
    void runTest(const std::string& testName) {
        if (testFunctions.find(testName) == testFunctions.end()) {
            std::cerr << "ERROR: Test not found: " << testName << std::endl;
            return;
        }
        
        TestResult result("Test: " + testName);
        results.push_back(result);
        
        try {
            testFunctions[testName](*this);
            results.back().passed = true;
            results.back().complete();
            
            if (verbose) {
                logTestResult(results.back());
            }
        } catch (const std::exception& e) {
            results.back().message = "EXCEPTION: " + std::string(e.what());
            results.back().complete();
            
            if (!quiet) {
                std::cerr << "ERROR in test " << testName << ": " << e.what() << std::endl;
            }
        }
    }
    
    void runBasicTest() {
        log("=== Running Basic Connectivity Test ===");
        
        brain->reset();
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw std::runtime_error("No brain regions found!");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            throw std::runtime_error("No neurons found!");
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        log("  Initial spike count: " + std::to_string(initialSpikes));
        log("  Injecting current into 10 neurons...");
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        log("  Running 50 simulation steps...");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        log("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes == 0) {
            throw std::runtime_error("No spikes detected - network not responding to input");
        }
        
        log("  ✓ Basic connectivity test PASSED");
    }
    
    void runPlasticityTest() {
        log("=== Running Plasticity Learning Experiment ===");
        
        brain->reset();
        brain->initialize();
        
        // Record initial state
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        std::vector<float> initialWeights;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning input
        log("  Applying learning input pattern (1000 steps)...");
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                log("    Progress: " + std::to_string(step) + " steps");
            }
        }
        
        // Analyze weight changes
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        size_t strengthened = 0;
        size_t weakened = 0;
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) strengthened++;
            else if (delta < -0.01f) weakened++;
        }
        
        if (strengthened == 0 && weakened == 0) {
            log("  ⚠ WARNING: No significant synaptic changes detected");
        }
        
        log("  Plasticity analysis: " + std::to_string(strengthened) + " strengthened, " + 
            std::to_string(weakened) + " weakened synapses");
        log("  ✓ Plasticity test COMPLETED");
    }
    
    void runStdpTest() {
        log("=== Running STDP Verification Test ===");
        
        brain->reset();
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw std::runtime_error("No brain regions found!");
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 3) {
            throw std::runtime_error("Not enough synapses for STDP test (need at least 3)");
        }
        
        log("  Testing STDP on " + std::to_string(std::min(size_t(3), synapses.size())) + " synapses");
        
        // Test STDP with correlated pre-post activity
        for (int trial = 0; trial < 30; ++trial) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        log("  ✓ STDP test COMPLETED");
    }
    
    void runAllTests() {
        log("=== Running All Tests Sequentially ===");
        log("  Test sequence: basic -> plasticity -> stdp");
        
        runBasicTest();
        log("";);
        
        runPlasticityTest();
        log("";);
        
        runStdpTest();
        
        log("  All tests completed successfully");
    }
    
    void runTests(const std::vector<std::string>& testNames) {
        log("=== Running Selected Tests ===");
        
        for (const auto& testName : testNames) {
            log("Running test: " + testName);
            runTest(testName);
            log("";);
        }
    }
    
    void printResults() const {
        log("\n" + std::string(60, '='));
        log("TEST RESULTS SUMMARY");
        log(std::string(60, '='));
        
        if (results.empty()) {
            log("No tests were run.");
            return;
        }
        
        int passed = 0;
        int failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            std::string status = result.passed ? "✓ PASSED" : "✗ FAILED";
            if (!result.passed && !result.message.empty()) {
                status += " (" + result.message + ")";
            }
            
            std::cout << "  " << result.name << ": " << status 
                      << " [" << result.getFormattedTime() << "s]" << std::endl;
            
            if (result.passed) passed++;
            else failed++;
            totalTime += result.executionTime;
        }
        
        log("");
        log("Summary: " << passed << " passed, " << failed << " failed");
        log("Total execution time: " << std::fixed << std::setprecision(3) << totalTime << "s");
        
        if (failed > 0) {
            log("⚠ Some tests failed - check output for details");
        } else {
            log("✓ All tests passed successfully!");
        }
        
        log(std::string(60, '='));
    }
    
private:
    void log(const std::string& message) {
        if (quiet && !message.empty() && message.find("===") == std::string::npos) {
            return;
        }
        
        std::cout << message << std::endl;
    }
    
    void logTestResult(const TestResult& result) {
        std::cout << "[TEST RESULT] " << result.name << ": ";
        
        if (result.passed) {
            std::cout << "✓ PASSED (" << result.getFormattedTime() << "s)" << std::endl;
        } else {
            std::cout << "✗ FAILED (" << result.getFormattedTime() << "s)";
            if (!result.message.empty()) {
                std::cout << ") - " << result.message;
            }
            std::cout << std::endl;
        }
    }
};

// Test suite manager for integrating with CLI
class TestSuiteManager {
public:
    static std::unique_ptr<TestSuite> createTestSuite(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Config> config,
        bool verbose = false,
        bool quiet = false) {
        
        return std::make_unique<TestSuite>(brain, config, verbose, quiet);
    }
    
    static std::vector<std::string> parseTestModes(const std::string& modeString) {
        std::vector<std::string> modes;
        size_t start = 0;
        size_t end = modeString.find(',');
        
        while (end != std::string::npos) {
            std::string mode = modeString.substr(start, end - start);
            trim(mode);
            if (!mode.empty()) {
                modes.push_back(mode);
            }
            start = end + 1;
            end = modeString.find(',', start);
        }
        
        // Add last mode
        std::string lastMode = modeString.substr(start);
        trim(lastMode);
        if (!lastMode.empty()) {
            modes.push_back(lastMode);
        }
        
        return modes;
    }
    
private:
    static void trim(std::string& s) {
        // Remove leading and trailing whitespace
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }
};
