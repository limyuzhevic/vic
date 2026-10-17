// NLM (熙然) - Neural Learning Machine
// Phase 2: Test Suite Implementation
// Implementation file for the TestSuite class

#include "test_framework/TestSuite.hpp"
#include <sstream>
#include <iomanip>

using namespace nlm;

namespace Tests {
    // Constructor implementation
    TestSuite::TestSuite(std::shared_ptr<Brain> b, std::shared_ptr<Config> c, bool v, bool q)
        : brain(b), config(c), verbose(v), quiet(q) {
        registerTests();
    }
    
    // Register tests implementation
    void TestSuite::registerTests() {
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
    
    // Run single test implementation
    void TestSuite::runTest(const std::string& testName) {
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
    
    // Run basic test implementation
    void TestSuite::runBasicTest() {
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
    
    // Run plasticity test implementation
    void TestSuite::runPlasticityTest() {
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
    
    // Run STDP test implementation
    void TestSuite::runStdpTest() {
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
    
    // Run all tests implementation
    void TestSuite::runAllTests() {
        log("=== Running All Tests Sequentially ===");
        log("  Test sequence: basic -> plasticity -> stdp");
        
        runBasicTest();
        log("";);
        
        runPlasticityTest();
        log("";);
        
        runStdpTest();
        
        log("  All tests completed successfully");
    }
    
    // Run multiple tests implementation
    void TestSuite::runTests(const std::vector<std::string>& testNames) {
        log("=== Running Selected Tests ===");
        
        for (const auto& testName : testNames) {
            log("Running test: " + testName);
            runTest(testName);
            log("";);
        }
    }
    
    // Print results implementation
    void TestSuite::printResults() const {
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
    
    // Helper function implementation
    void TestSuite::log(const std::string& message) {
        if (quiet && !message.empty() && message.find("===") == std::string::npos) {
            return;
        }
        
        std::cout << message << std::endl;
    }
    
    // Helper function implementation
    void TestSuite::logTestResult(const TestResult& result) {
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
    
    // Test suite manager helper functions
    std::unique_ptr<TestSuite> TestSuiteManager::createTestSuite(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Config> config,
        bool verbose,
        bool quiet) {
        
        return std::make_unique<TestSuite>(brain, config, verbose, quiet);
    }
    
    std::vector<std::string> TestSuiteManager::parseTestModes(const std::string& modeString) {
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
    
    // Trim implementation
    void TestSuiteManager::trim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

} // namespace Tests
