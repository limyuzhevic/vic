// TestRunner.cpp
#include "CLI/TestRunner.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <map>

struct TestRunner::Impl {
    struct Test {
        std::string name;
        std::function<void(std::shared_ptr<nlm::Config>)> testFunc;
        std::string description;
    };
    
    std::vector<Test> tests;
};

TestRunner::TestRunner() : pImpl(std::make_unique<Impl>()), verbose_(false) {
    // Register built-in tests
    registerTest("connectivity", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) return;
        
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region) return;
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run 50 steps
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        if (spikes == 0) {
            throw std::runtime_error("No spikes generated - connectivity test failed");
        }
    }, "Test basic neural connectivity and spike propagation");
    
    registerTest("plasticity", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) return;
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input pattern (1000 steps)
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            // Inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        // Check if learning occurred
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            bool learningOccurred = false;
            for (const auto& syn : region->getSynapses()) {
                if (syn->getWeight() > 0.0f) {
                    learningOccurred = true;
                    break;
                }
            }
            if (!learningOccurred) {
                throw std::runtime_error("Plasticity test failed - no weight changes");
            }
        }
    }, "Test STDP and Hebbian plasticity with learning experiment");
    
    registerTest("stdp", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) return;
        
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            return; // Skip if not enough synapses
        }
        
        auto& synapses = region->getSynapses();
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
        }
        
        // Create correlated pre->post activity (LTP)
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 50; ++trial) {
                // Pre fires first
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check for potentiation
        float totalDelta = 0.0f;
        for (size_t i = 0; i < beforeWeights.size(); ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        if (totalDelta <= 0.001f) {
            throw std::runtime_error("STDP test failed - no potentiation detected");
        }
    }, "Test spike-timing dependent plasticity (STDP) rule");
}

void TestRunner::registerTest(const std::string& name,
                             std::function<void(std::shared_ptr<nlm::Config>)> testFunc,
                             const std::string& description) {
    pImpl->tests.push_back({name, testFunc, description});
}

TestResult TestRunner::runTest(const std::string& name, std::shared_ptr<nlm::Config> config) {
    TestResult result;
    result.name = name;
    result.passed = false;
    
    auto it = std::find_if(pImpl->tests.begin(), pImpl->tests.end(),
                           [&name](const Impl::Test& t) { return t.name == name; });
        result.message = "Test not found";
        return result;
    }
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        it->testFunc(config);
        result.passed = true;
        result.message = "Test passed";
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = std::string("Test failed: ") + e.what();
    } catch (...) {
        result.passed = false;
        result.message = "Test failed with unknown error";
    }
    
    // Calculate execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.executionTime = duration.count() / 1000.0;
    
    return result;
}

std::vector<TestResult> TestRunner::runAllTests(std::shared_ptr<nlm::Config> config) {
    std::vector<TestResult> results;
    
    for (const auto& test : pImpl->tests) {
        if (verbose_) {
            std::cout << "Running test: " << test.name << " - " << test.description << std::endl;
        }
        
        TestResult result = runTest(test.name, config);
        results.push_back(result);
        
        if (verbose_) {
            std::cout << "  " << (result.passed ? "PASS" : "FAIL") 
                      << " (" << result.executionTime << "s): " << result.message << std::endl;
        }
    }
    
    return results;
}

void TestRunner::listTests() const {
    std::cout << "Available tests:" << std::endl;
    for (const auto& test : pImpl->tests) {
        std::cout << "  " << std::left << std::setw(20) << test.name 
                  << "  " << test.description << std::endl;
    }
}
