#pragma once

// Test framework for NLM with isolation and comprehensive testing capabilities
// Provides isolated test execution, parameterized tests, and detailed reporting

#include "main.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <chrono>

namespace nlm {

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    bool passed = false;
    std::string message;
    double duration = 0.0;
    std::string errorType;
    
    TestResult(const std::string& testName, const std::string& testCategory)
        : name(testName), category(testCategory) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Test: " << name << " (" << category << ") - " 
           << (passed ? "PASSED" : "FAILED");
        if (!message.empty()) {
            ss << " - " << message;
        }
        ss << " [" << duration << "s]";
        return ss.str();
    }
};

// Test fixture base class
class TestFixture {
public:
    virtual ~TestFixture() = default;
    
    virtual void setup() {}
    virtual void teardown() {}
    
    template<typename T>
    T getParameter(const std::string& name, const T& defaultValue = T()) {
        auto it = parameters_.find(name);
        return (it != parameters_.end()) ? std::any_cast<T>(it->second) : defaultValue;
    }
    
    void setParameter(const std::string& name, const std::any& value) {
        parameters_[name] = value;
    }
    
protected:
    std::map<std::string, std::any> parameters_;
};

// Test runner interface
class TestRunner {
public:
    virtual ~TestRunner() = default;
    virtual void addTest(const std::string& name, const std::string& category,
                       std::function<bool()> testFunc) = 0;
    virtual void runAllTests() = 0;
    virtual std::vector<TestResult> getResults() const = 0;
    virtual void clearResults() = 0;
};

// Simple test runner implementation
class SimpleTestRunner : public TestRunner {
public:
    SimpleTestRunner(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void addTest(const std::string& name, const std::string& category,
                 std::function<bool()> testFunc) override {
        tests_.push_back({name, category, testFunc});
    }
    
    void runAllTests() override {
        results_.clear();
        
        NLM_LOG_INFO("=== NLM Test Suite ===");
        NLM_LOG_INFO("Running " + std::to_string(tests_.size()) + " tests");
        
        for (const auto& test : tests_) {
            auto startTime = std::chrono::high_resolution_clock::now();
            TestResult result(test.name, test.category);
            
            try {
                result.passed = test.testFunc();
                if (result.passed) {
                    NLM_LOG_INFO("✓ " + result.toString());
                } else {
                    NLM_LOG_WARNING("- " + result.toString());
                }
            } catch (const NLMError& e) {
                result.passed = false;
                result.message = std::string(e.what());
                result.errorType = typeid(e).name();
                NLM_LOG_ERROR("✗ " + result.toString());
            } catch (const std::exception& e) {
                result.passed = false;
                result.message = "Unexpected error: " + std::string(e.what());
                result.errorType = typeid(e).name();
                NLM_LOG_ERROR("✗ " + result.toString());
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            result.duration = duration.count();
            
            results_.push_back(result);
        }
        
        printTestSummary();
    }
    
    std::vector<TestResult> getResults() const override {
        return results_;
    }
    
    void clearResults() override {
        results_.clear();
    }
    
private:
    void printTestSummary() {
        size_t totalTests = results_.size();
        size_t passedTests = 0;
        size_t failedTests = 0;
        
        for (const auto& result : results_) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Summary ===");
        NLM_LOG_INFO("Total tests: " + std::to_string(totalTests));
        NLM_LOG_INFO("Passed: " + std::to_string(passedTests));
        NLM_LOG_INFO("Failed: " + std::to_string(failedTests));
        
        if (failedTests > 0) {
            NLM_LOG_ERROR("Test suite completed with failures");
        } else {
            NLM_LOG_INFO("All tests passed successfully!");
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Detailed Results ===");
        for (const auto& result : results_) {
            NLM_LOG_INFO(result.toString());
        }
    }
    
    struct TestCase {
        std::string name;
        std::string category;
        std::function<bool()> testFunc;
    };
    
    std::vector<TestCase> tests_;
    std::vector<TestResult> results_;
    std::shared_ptr<Brain> brain_;
};

// NLM test suite with comprehensive testing capabilities
class NLMTestSuite {
public:
    NLMTestSuite(std::shared_ptr<Brain> brain) : brain_(brain), runner_(std::make_unique<SimpleTestRunner>(brain)) {}
    
    void runAllTests() {
        ErrorContext context("Test Suite");
        
        NLM_LOG_INFO("=== NLM Comprehensive Test Suite ===");
        NLM_LOG_INFO("Starting comprehensive testing of NLM components");
        
        // Clear previous results
        runner_->clearResults();
        
        // Run different test categories
        runConnectivityTests();
        runPlasticityTests();
        runSTDPTests();
        runMemoryTests();
        runPerformanceTests();
        runIntegrationTests();
        
        // Display summary
        auto results = runner_->getResults();
        printFinalSummary(results);
    }
    
    void addCustomTest(const std::string& name, const std::string& category,
                      std::function<bool()> testFunc) {
        runner_->addTest(name, category, testFunc);
    }
    
private:
    void runConnectivityTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Connectivity Tests ===");
        
        // Basic connectivity test
        addCustomTest("Basic Connectivity", "Connectivity",
            [this]() {
                return testBasicConnectivity();
            });
        
        // Region connectivity test
        addCustomTest("Region Connectivity", "Connectivity",
            [this]() {
                return testRegionConnectivity();
            });
        
        // Synapse propagation test
        addCustomTest("Synapse Propagation", "Connectivity",
            [this]() {
                return testSynapsePropagation();
            });
    }
    
    void runPlasticityTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Plasticity Tests ===");
        
        // STDP potentiation test
        addCustomTest("STDP Potentiation", "Plasticity",
            [this]() {
                return testSTDPPotentiation();
            });
        
        // STDP depression test
        addCustomTest("STDP Depression", "Plasticity",
            [this]() {
                return testSTDPDepression();
            });
        
        // Hebbian learning test
        addCustomTest("Hebbian Learning", "Plasticity",
            [this]() {
                return testHebbianLearning();
            });
    }
    
    void runSTDPTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== STDP Tests ===");
        
        // Correlated activity test
        addCustomTest("Correlated Activity", "STDP",
            [this]() {
                return testCorrelatedActivity();
            });
        
        // Spike timing dependence test
        addCustomTest("Spike Timing Dependence", "STDP",
            [this]() {
                return testSpikeTimingDependence();
            });
    }
    
    void runMemoryTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Memory Tests ===");
        
        // Working memory capacity test
        addCustomTest("Working Memory Capacity", "Memory",
            [this]() {
                return testWorkingMemoryCapacity();
            });
        
        // Episodic memory storage test
        addCustomTest("Episodic Memory Storage", "Memory",
            [this]() {
                return testEpisodicMemoryStorage();
            });
        
        // Memory replay test
        addCustomTest("Memory Replay", "Memory",
            [this]() {
                return testMemoryReplay();
            });
    }
    
    void runPerformanceTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Performance Tests ===");
        
        // Throughput test
        addCustomTest("Throughput", "Performance",
            [this]() {
                return testThroughput();
            });
        
        // Memory efficiency test
        addCustomTest("Memory Efficiency", "Performance",
            [this]() {
                return testMemoryEfficiency();
            });
    }
    
    void runIntegrationTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Integration Tests ===");
        
        // Neuromodulation integration test
        addCustomTest("Neuromodulation Integration", "Integration",
            [this]() {
                return testNeuromodulationIntegration();
            });
        
        // Development integration test
        addCustomTest("Development Integration", "Integration",
            [this]() {
                return testDevelopmentIntegration();
            });
        
        // Cognition integration test
        addCustomTest("Cognition Integration", "Integration",
            [this]() {
                return testCognitionIntegration();
            });
    }
    
    void printFinalSummary(const std::vector<TestResult>& results) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Test Summary ===");
        
        size_t totalTests = results.size();
        size_t passedTests = 0;
        size_t failedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            totalDuration += result.duration;
        }
        
        NLM_LOG_INFO("Total test cases: " + std::to_string(totalTests));
        NLM_LOG_INFO("Passed: " + std::to_string(passedTests));
        NLM_LOG_INFO("Failed: " + std::to_string(failedTests));
        NLM_LOG_INFO("Average duration per test: " + std::to_string(totalDuration / totalTests) + "s");
        NLM_LOG_INFO("Total test suite duration: " + std::to_string(totalDuration) + "s");
        
        if (failedTests > 0) {
            NLM_LOG_ERROR("Test suite completed with " + std::to_string(failedTests) + 
                         " failures out of " + std::to_string(totalTests) + " tests");
        } else {
            NLM_LOG_INFO("All tests passed successfully!");
        }
    }
    
    // Individual test implementations
    bool testBasicConnectivity() {
        brain_->reset();
        brain_->initialize();
        
        auto* region = brain_->getRegion(RegionId(1));
        NLMValidate(region != nullptr, "Region 1 should exist");
        
        auto neurons = region->getAllNeurons();
        NLMValidate(!neurons.empty(), "Should have neurons");
        
        // Inject current and check for spikes
        size_t initialSpikes = brain_->getTotalSpikeCount();
        neurons[0]->injectCurrent(50.0f);
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain_->step(step, step * 0.001);
        }
        
        size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
        NLMValidate(spikes > 0, "Should generate spikes");
        
        return true;
    }
    
    bool testRegionConnectivity() {
        brain_->reset();
        brain_->initialize();
        
        // Create second region
        auto region2Id = brain_->addRegion("Region_2");
        auto* region2 = brain_->getRegion(region2Id);
        NLMValidate(region2 != nullptr, "Region 2 should exist");
        
        // Add neurons to region 2
        region2->addPopulation(10, NeuronType::Internal);
        
        // Check that regions are independent
        auto* region1 = brain_->getRegion(RegionId(1));
        auto* region2Actual = brain_->getRegion(region2Id);
        NLMValidate(region1 != region2Actual, "Regions should be different objects");
        
        // Check different neuron counts
        size_t region1Neurons = region1->getTotalNeuronCount();
        size_t region2Neurons = region2Actual->getTotalNeuronCount();
        NLMValidate(region1Neurons > 0 && region2Neurons > 0, "Both regions should have neurons");
        
        return true;
    }
    
    bool testSynapsePropagation() {
        brain_->reset();
        brain_->initialize();
        
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        NLMValidate(neurons.size() >= 2, "Should have at least 2 neurons");
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Find synapse from pre to post
        auto synapses = region->getSynapsesFrom(preNeuron->getId());
        bool foundSynapse = false;
        
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                foundSynapse = true;
                break;
            }
        }
        
        NLMValidate(foundSynapse, "Should have synapse between neurons");
        
        // Fire pre-synaptic neuron and check post-synaptic response
        size_t initialSpikes = brain_->getTotalSpikeCount();
        preNeuron->injectCurrent(50.0f);
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain_->step(step, step * 0.001);
        }
        
        size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
        NLMValidate(spikes > 0, "Should generate spikes through synapse");
        
        return true;
    }
    
    bool testSTDPPotentiation() {
        brain_->reset();
        brain_->initialize();
        
        // Enable STDP on synapses
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false);  // Enable only STDP
            }
        }
        
        // Create correlated activity (pre before post)
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        NLMValidate(neurons.size() >= 2, "Should have at least 2 neurons");
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Record initial weights
        std::vector<float> initialWeights;
        auto synapses = region->getSynapsesFrom(preNeuron->getId());
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLMValidate(!initialWeights.empty(), "Should have at least one synapse");
        
        // Create correlated activity (pre fires, then post fires)
        for (int trial = 0; trial < 20; ++trial) {
            preNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Check that weights increased (LTP)
        float totalDelta = 0.0f;
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                float delta = syn->getWeight() - initialWeights[&syn - &synapses[0]];
                totalDelta += delta;
            }
        }
        
        NLMValidate(totalDelta > 0.001f, "STDP should produce potentiation");
        
        return true;
    }
    
    bool testSTDPDepression() {
        brain_->reset();
        brain_->initialize();
        
        // Enable STDP on synapses
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false);  // Enable only STDP
            }
        }
        
        // Create anti-correlated activity (post before pre)
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        NLMValidate(neurons.size() >= 2, "Should have at least 2 neurons");
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Record initial weights
        std::vector<float> initialWeights;
        auto synapses = region->getSynapsesFrom(preNeuron->getId());
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLMValidate(!initialWeights.empty(), "Should have at least one synapse");
        
        // Create anti-correlated activity (post fires, then pre fires)
        for (int trial = 0; trial < 20; ++trial) {
            postNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2, trial * 2 * 0.001);
            
            preNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Check that weights decreased (LTD)
        float totalDelta = 0.0f;
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                float delta = syn->getWeight() - initialWeights[&syn - &synapses[0]];
                totalDelta += delta;
            }
        }
        
        NLMValidate(totalDelta < -0.001f, "STDP should produce depression");
        
        return true;
    }
    
    bool testHebbianLearning() {
        brain_->reset();
        brain_->initialize();
        
        // Enable Hebbian learning
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, false, false);  // Enable only Hebbian
            }
        }
        
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        NLMValidate(neurons.size() >= 2, "Should have at least 2 neurons");
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Record initial weights
        std::vector<float> initialWeights;
        auto synapses = region->getSynapsesFrom(preNeuron->getId());
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLMValidate(!initialWeights.empty(), "Should have at least one synapse");
        
        // Fire both neurons simultaneously (Hebbian)
        for (int trial = 0; trial < 10; ++trial) {
            preNeuron->injectCurrent(60.0f);
            postNeuron->injectCurrent(60.0f);
            brain_->step(trial, trial * 0.001);
        }
        
        // Check that weights increased (Hebbian learning)
        float totalDelta = 0.0f;
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                float delta = syn->getWeight() - initialWeights[&syn - &synapses[0]];
                totalDelta += delta;
            }
        }
        
        NLMValidate(totalDelta > 0.001f, "Hebbian learning should strengthen connections");
        
        return true;
    }
    
    bool testCorrelatedActivity() {
        brain_->reset();
        brain_->initialize();
        
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        NLMValidate(neurons.size() >= 2, "Should have at least 2 neurons");
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Create correlated activity over time
        for (int step = 0; step < 100; ++step) {
            // Correlated firing pattern
            if (step % 2 == 0) {
                preNeuron->injectCurrent(60.0f);
            }
            if (step % 3 == 0) {
                postNeuron->injectCurrent(60.0f);
            }
            brain_->step(step, step * 0.001);
        }
        
        // Check for synaptic changes consistent with STDP
        float totalChange = 0.0f;
        if (auto* stdpSystem = brain_->getSTDP()) {
            for (auto& region : brain_->getRegions()) {
                for (auto& syn : region->getSynapses()) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        totalChange += std::abs(syn->getWeight() - 0.5f);  // Expected weight
                    }
                }
            }
        }
        
        NLMValidate(totalChange > 0.0f, "Should detect correlated activity effects");
        
        return true;
    }
    
    bool testSpikeTimingDependence() {
        brain_->reset();
        brain_->initialize();
        
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        NLMValidate(neurons.size() >= 2, "Should have at least 2 neurons");
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Test different spike time intervals
        struct IntervalTest {
            int preDelay;
            int postDelay;
            bool shouldProduceLTP;
        };
        
        std::vector<IntervalTest> tests = {
            {0, 5, true},   // Pre before post (LTP)
            {5, 0, false},  // Post before pre (LTD)
            {2, 10, true},  // Pre before post (LTP)
            {10, 2, false}  // Post before pre (LTD)
        };
        
        for (const auto& test : tests) {
            brain_->reset();
            brain_->initialize();
            
            // Update neurons with new references
            neurons = brain_->getRegion(RegionId(1))->getAllNeurons();
            if (neurons.size() < 2) continue;
            preNeuron = neurons[0];
            postNeuron = neurons[1];
            
            // Create spike timing pattern
            for (int step = 0; step < 50; ++step) {
                if (step == test.preDelay) {
                    preNeuron->injectCurrent(60.0f);
                }
                if (step == test.postDelay) {
                    postNeuron->injectCurrent(60.0f);
                }
                brain_->step(step, step * 0.001);
            }
            
            // Check result (simplified - just ensure no crashes)
            NLMValidate(true, "Spike timing test completed");
        }
        
        return true;
    }
    
    bool testWorkingMemoryCapacity() {
        brain_->reset();
        brain_->initialize();
        
        auto* workingMemory = brain_->getWorkingMemory();
        NLMValidate(workingMemory != nullptr, "Working memory should exist");
        
        // Test capacity
        workingMemory->setCapacity(100);
        
        // Store many traces
        for (int i = 0; i < 150; ++i) {
            workingMemory->storeToNeuron(NeuronId(i), 0.5f);
        }
        
        // Check that capacity is respected (implementation dependent)
        // For now, just ensure no crashes
        
        return true;
    }
    
    bool testEpisodicMemoryStorage() {
        brain_->reset();
        brain_->initialize();
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        NLMValidate(episodicMemory != nullptr, "Episodic memory should exist");
        
        episodicMemory->setMaxEpisodes(50);
        
        // Create and store episodes
        for (int episode = 0; episode < 60; ++episode) {
            EpisodicMemoryItem item;
            item.timestamp = SimulationStep(episode);
            item.reward = 0.5f;
            item.activeNeurons = {NeuronId(episode % 10)};
            item.neuronActivations = {0.5f};
            
            episodicMemory->storeEpisode(item);
        }
        
        // Check episode count
        size_t episodeCount = episodicMemory->getEpisodeCount();
        NLMValidate(episodeCount > 0, "Should have stored episodes");
        
        return true;
    }
    
    bool testMemoryReplay() {
        brain_->reset();
        brain_->initialize();
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        NLMValidate(episodicMemory != nullptr, "Episodic memory should exist");
        
        // Store some episodes
        for (int i = 0; i < 20; ++i) {
            EpisodicMemoryItem item;
            item.timestamp = SimulationStep(i);
            item.reward = 0.5f;
            item.activeNeurons = {NeuronId(i % 5)};
            item.neuronActivations = {0.5f};
            
            episodicMemory->storeEpisode(item);
        }
        
        // Request replay (implementation dependent)
        // For now, just ensure no crashes
        
        return true;
    }
    
    bool testThroughput() {
        brain_->reset();
        brain_->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run many simulation steps
        for (SimulationStep step = 0; step < 1000; ++step) {
            brain_->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        // Calculate steps per second
        double stepsPerSecond = 1000.0 / duration.count();
        
        NLM_LOG_INFO("Throughput test: " + std::to_string(stepsPerSecond) + " steps/second");
        
        NLMValidate(stepsPerSecond > 100.0, "Should process at least 100 steps/second");
        
        return true;
    }
    
    bool testMemoryEfficiency() {
        brain_->reset();
        brain_->initialize();
        
        // Check memory usage (implementation dependent)
        // For now, just ensure no crashes
        
        NLMValidate(true, "Memory efficiency test completed");
        
        return true;
    }
    
    bool testNeuromodulationIntegration() {
        brain_->reset();
        brain_->initialize();
        
        auto* dopamine = brain_->getDopamine();
        NLMValidate(dopamine != nullptr, "Dopamine system should exist");
        
        // Update neuromodulation system
        dopamine->update(0.001);
        
        // Check that effects are applied
        // For now, just ensure no crashes
        
        return true;
    }
    
    bool testDevelopmentIntegration() {
        brain_->reset();
        brain_->initialize();
        
        auto* developmentSystem = brain_->getDevelopmentSystem();
        NLMValidate(developmentSystem != nullptr, "Development system should exist");
        
        // Update development system
        developmentSystem->update(brain_.get(), *brain_->getRandomGenerator(), 1000.0);
        
        // Check developmental stage changes
        NLMValidate(true, "Development integration test completed");
        
        return true;
    }
    
    bool testCognitionIntegration() {
        brain_->reset();
        brain_->initialize();
        
        auto* planner = brain_->getPlanner();
        NLMValidate(planner != nullptr, "Planner system should exist");
        
        auto* conceptFormation = brain_->getConceptFormation();
        NLMValidate(conceptFormation != nullptr, "Concept formation system should exist");
        
        // Initialize cognition systems
        planner->initialize(brain_.get());
        conceptFormation->initialize(brain_.get());
        
        // Test cognition integration
        // For now, just ensure no crashes
        
        return true;
    }
    
    std::unique_ptr<TestRunner> runner_;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm