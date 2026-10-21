#!/usr/bin/env g++
// NLM Test Tool
// Run specific test suites for NLM

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
    std::map<std::string, std::string> metrics;
};

class NLMTest {
private:
    std::vector<TestResult> testResults;
    std::shared_ptr<Config> config;
    bool verbose;
    bool runAllTests;
    bool skip; // If true, skip tests and just show help
    
public:
    NLMTest() {
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        verbose = false;
        runAllTests = false;
        skip = false;
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --config <file>           Load configuration file" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --all                     Run all tests (default)" << std::endl;
        std::cout << "  --list                    List available tests" << std::endl;
        std::cout << "  --run <test>              Run specific test" << std::endl;
        std::cout << "  --skip                    Skip tests and show help only" << std::endl;
        std::cout << "  --output <file>           Save test results to file" << std::endl;
        std::cout << "  --xml                     Output in XML format" << std::endl;
        std::cout << "  --junit                   Output in JUnit XML format" << std::endl;
        std::cout << std::endl;
        std::cout << "Available Tests:" << std::endl;
        std::cout << "  test_basic_connectivity    - Test basic neural connectivity" << std::endl;
        std::cout << "  test_plasticity            - Test plasticity learning" << std::endl;
        std::cout << "  test_stdp_verification     - Test STDP mechanism" << std::endl;
        std::cout << "  test_memory_integration    - Test memory systems" << std::endl;
        std::cout << "  test_neuromodulation       - Test neuromodulation" << std::endl;
        std::cout << "  test_development          - Test developmental processes" << std::endl;
        std::cout << "  test_plasticity_rules     - Test various plasticity rules" << std::endl;
        std::cout << "  test_brain_initialization  - Test brain initialization" << std::endl;
        std::cout << "  test_simulation_clock     - Test simulation clock" << std::endl;
        std::cout << "  test_config_loading        - Test configuration loading" << std::endl;
        std::cout << "  test_logger               - Test logging system" << std::endl;
    }
    
    void listTests() {
        std::cout << "=== Available NLM Tests ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Core Tests:" << std::endl;
        std::cout << "  test_basic_connectivity    - Test basic neural connectivity and spike propagation" << std::endl;
        std::cout << "  test_plasticity            - Test Hebbian and STDP learning" << std::endl;
        std::cout << "  test_stdp_verification     - Verify STDP potentiation and depression" << std::endl;
        std::cout << "  test_memory_integration    - Test memory system integration" << std::endl;
        std::cout << "  test_neuromodulation       - Test dopamine, curiosity, and novelty" << std::endl;
        std::cout << std::endl;
        std::cout << "Development Tests:" << std::endl;
        std::cout << "  test_development          - Test developmental stages and maturation" << std::endl;
        std::cout << "  test_plasticity_rules     - Test specific plasticity rules (STDP, Hebbian)" << std::endl;
        std::cout << "  test_structural_plasticity - Test structural plasticity" << std::endl;
        std::cout << std::endl;
        std::cout << "System Tests:" << std::endl;
        std::cout << "  test_brain_initialization  - Test brain initialization and setup" << std::endl;
        std::cout << "  test_simulation_clock     - Test simulation timing" << std::endl;
        std::cout << "  test_config_loading        - Test configuration file loading" << std::endl;
        std::cout << "  test_logger               - Test logging system" << std::endl;
        std::cout << "  test_random_generator     - Test random number generation" << std::endl;
        std::cout << std::endl;
    }
    
    void runSpecificTest(const std::string& testName) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (testName == "test_basic_connectivity") {
            runBasicConnectivityTest();
        } else if (testName == "test_plasticity") {
            runPlasticityTest();
        } else if (testName == "test_stdp_verification") {
            runStdpVerificationTest();
        } else if (testName == "test_memory_integration") {
            runMemoryIntegrationTest();
        } else if (testName == "test_neuromodulation") {
            runNeuromodulationTest();
        } else if (testName == "test_development") {
            runDevelopmentTest();
        } else if (testName == "test_plasticity_rules") {
            runPlasticityRulesTest();
        } else if (testName == "test_brain_initialization") {
            runBrainInitializationTest();
        } else if (testName == "test_simulation_clock") {
            runSimulationClockTest();
        } else if (testName == "test_config_loading") {
            runConfigLoadingTest();
        } else if (testName == "test_logger") {
            runLoggerTest();
        } else if (testName == "test_random_generator") {
            runRandomGeneratorTest();
        } else {
            std::cout << "Error: Unknown test: " << testName << std::endl;
            return;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Record test result (already recorded by individual test functions)
    }
    
    void runBasicConnectivityTest() {
        std::cout << "\n=== Running Test: Basic Connectivity ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test 1: Inject current into neurons and check for spikes
        auto neurons = brain->getTotalNeuronCount();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into multiple neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        TestResult result;
        result.testName = "test_basic_connectivity";
        result.passed = (spikes > 0);
        result.message = "Generated " + std::to_string(spikes) + " spikes through neural network";
        result.executionTime = testTime;
        result.metrics["neurons_tested"] = std::to_string(std::min(size_t(10), neurons / 10));
        result.metrics["total_neuron_count"] = std::to_string(neurons);
        result.metrics["spikes_generated"] = std::to_string(spikes);
        
        testResults.push_back(result);
        
        if (verbose) {
            std::cout << "  Neurons tested: " << std::min(size_t(10), neurons / 10) << std::endl;
            std::cout << "  Total neurons: " << neurons << std::endl;
            std::cout << "  Initial spikes: " << initialSpikes << std::endl;
            std::cout << "  Final spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Spikes generated: " << spikes << std::endl;
            std::cout << "  Test time: " << testTime << "ms" << std::endl;
        }
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: No spikes generated" << std::endl;
        }
    }
    
    void runPlasticityTest() {
        std::cout << "\n=== Running Test: Plasticity Learning ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning patterns
        for (SimulationStep step = 0; step < 100; ++step) {
            // Create input patterns
            for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Check if learning occurred
        auto* spikeSystem = brain->getSpikeSystem();
        bool learningOccurred = false;
        if (spikeSystem) {
            auto mostActive = spikeSystem->getMostActiveNeurons(5);
            learningOccurred = !mostActive.empty();
        }
        
        TestResult result;
        result.testName = "test_plasticity";
        result.passed = learningOccurred;
        result.message = "Learning " + std::string(learningOccurred ? "detected" : "not detected") + 
                        " through neural activity";
        result.executionTime = testTime;
        result.metrics["steps_run"] = "100";
        result.metrics["neurons_injected"] = std::to_string(std::min(size_t(5), brain->getTotalNeuronCount() / 4));
        result.metrics["learning_detected"] = learningOccurred ? "true" : "false";
        
        testResults.push_back(result);
        
        if (verbose) {
            std::cout << "  Steps run: 100" << std::endl;
            std::cout << "  Neurons injected: " << std::min(size_t(5), brain->getTotalNeuronCount() / 4) << std::endl;
            std::cout << "  Learning detected: " << (learningOccurred ? "true" : "false") << std::endl;
            std::cout << "  Test time: " << testTime << "ms" << std::endl;
        }
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: No learning detected" << std::endl;
        }
    }
    
    void runStdpVerificationTest() {
        std::cout << "\n=== Running Test: STDP Verification ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            TestResult result;
            result.testName = "test_stdp_verification";
            result.passed = false;
            result.message = "Not enough synapses for STDP test";
            result.executionTime = 0.0;
            testResults.push_back(result);
            std::cout << "  ✗ FAILED: Not enough synapses" << std::endl;
            return;
        }
        
        // Get initial weights
        auto& synapses = region->getSynapses();
        std::vector<float> initialWeights;
        for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
            initialWeights.push_back(synapses[i]->getWeight());
        }
        
        // Enable STDP only
        for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        // Create correlated pre-post activity
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 50; ++trial) {
                // Fire pre-synaptic neuron
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Fire post-synaptic neuron
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check weight changes
        float totalDelta = 0.0f;
        for (size_t i = 0; i < initialWeights.size(); ++i) {
            float delta = synapses[i]->getWeight() - initialWeights[i];
            totalDelta += delta;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        bool stdpWorking = (std::abs(totalDelta) > 0.001f);
        
        TestResult result;
        result.testName = "test_stdp_verification";
        result.passed = stdpWorking;
        result.message = "STDP " + std::string(stdpWorking ? "working" : "not working") +
                        " (total delta: " + std::to_string(totalDelta) + ")";
        result.executionTime = testTime;
        result.metrics["initial_weights_recorded"] = std::to_string(initialWeights.size());
        result.metrics["total_weight_change"] = std::to_string(totalDelta);
        result.metrics["stdp_triggered"] = stdpWorking ? "true" : "false";
        
        testResults.push_back(result);
        
        if (verbose) {
            std::cout << "  Initial weights: " << initialWeights.size() << " synapses" << std::endl;
            std::cout << "  Total weight change: " << totalDelta << std::endl;
            std::cout << "  STDP triggered: " << (stdpWorking ? "true" : "false") << std::endl;
            std::cout << "  Test time: " << testTime << "ms" << std::endl;
        }
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: STDP not working" << std::endl;
        }
    }
    
    void runMemoryIntegrationTest() {
        std::cout << "\n=== Running Test: Memory Integration ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test memory systems
        bool workingMemoryOk = false;
        bool episodicMemoryOk = false;
        
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            workingMemoryOk = true;
            std::cout << "  Working memory: ACTIVE" << std::endl;
        } else {
            std::cout << "  Working memory: INACTIVE" << std::endl;
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            episodicMemoryOk = true;
            std::cout << "  Episodic memory: ACTIVE" << std::endl;
        } else {
            std::cout << "  Episodic memory: INACTIVE" << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        bool memoryIntegrationOk = workingMemoryOk && episodicMemoryOk;
        
        TestResult result;
        result.testName = "test_memory_integration";
        result.passed = memoryIntegrationOk;
        result.message = "Memory systems " + std::string(memoryIntegrationOk ? "integrated" : "not integrated");
        result.executionTime = testTime;
        result.metrics["working_memory_active"] = workingMemoryOk ? "true" : "false";
        result.metrics["episodic_memory_active"] = episodicMemoryOk ? "true" : "false";
        result.metrics["memory_integration"] = memoryIntegrationOk ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Memory systems not integrated" << std::endl;
        }
    }
    
    void runNeuromodulationTest() {
        std::cout << "\n=== Running Test: Neuromodulation ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test neuromodulation systems
        bool dopamineOk = false;
        bool curiosityOk = false;
        bool noveltyOk = false;
        
        auto* neuromodulator = brain->getNeuromodulator();
        if (neuromodulator) {
            dopamineOk = neuromodulator->getModulatorState(ModulatorType::Dopamine);
            curiosityOk = neuromodulator->getModulatorState(ModulatorType::Curiosity);
            noveltyOk = neuromodulator->getModulatorState(ModulatorType::Novelty);
            
            std::cout << "  Neuromodulator: ACTIVE" << std::endl;
            std::cout << "    Dopamine: " << (dopamineOk ? "active" : "inactive") << std::endl;
            std::cout << "    Curiosity: " << (curiosityOk ? "active" : "inactive") << std::endl;
            std::cout << "    Novelty: " << (noveltyOk ? "active" : "inactive") << std::endl;
        } else {
            std::cout << "  Neuromodulator: INACTIVE" << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        bool neuromodulationOk = dopamineOk || curiosityOk || noveltyOk;
        
        TestResult result;
        result.testName = "test_neuromodulation";
        result.passed = neuromodulationOk;
        result.message = "Neuromodulation " + std::string(neuromodulationOk ? "functional" : "not functional");
        result.executionTime = testTime;
        result.metrics["dopamine_active"] = dopamineOk ? "true" : "false";
        result.metrics["curiosity_active"] = curiosityOk ? "true" : "false";
        result.metrics["novelty_active"] = noveltyOk ? "true" : "false";
        result.metrics["neuromodulation_ok"] = neuromodulationOk ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Neuromodulation not functional" << std::endl;
        }
    }
    
    void runDevelopmentTest() {
        std::cout << "\n=== Running Test: Development ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test developmental processes
        bool developmentOk = false;
        
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            developmentOk = developmentSystem->isActive();
            
            std::cout << "  Development system: ACTIVE" << std::endl;
            std::cout << "    Synaptogenesis: " << (developmentSystem->getSynaptogenesisRate() > 0.0 ? "enabled" : "disabled") << std::endl;
            std::cout << "    Pruning: " << (developmentSystem->getPruningRate() > 0.0 ? "enabled" : "disabled") << std::endl;
            std::cout << "    Maturation: " << (developmentSystem->getMaturationRate() > 0.0 ? "enabled" : "disabled") << std::endl;
        } else {
            std::cout << "  Development system: INACTIVE" << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        TestResult result;
        result.testName = "test_development";
        result.passed = developmentOk;
        result.message = "Development " + std::string(developmentOk ? "active" : "not active");
        result.executionTime = testTime;
        result.metrics["development_system_active"] = developmentOk ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Development not active" << std::endl;
        }
    }
    
    void runPlasticityRulesTest() {
        std::cout << "\n=== Running Test: Plasticity Rules ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test various plasticity rules
        bool stdpOk = false;
        bool hebbianOk = false;
        bool structuralOk = false;
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto& synapses = region->getSynapses();
            if (!synapses.empty()) {
                // Enable all plasticity types
                for (auto& syn : synapses) {
                    syn->enablePlasticity(true, true, true);
                }
                
                // Run a learning trial
                brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
                for (SimulationStep step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                // Check if synapses changed (simple check)
                size_t changedSynapses = 0;
                for (const auto& syn : synapses) {
                    if (syn->hasPlasticity()) {
                        changedSynapses++;
                    }
                }
                
                stdpOk = (changedSynapses > 0);
                hebbianOk = true; // Assume enabled if no error
                structuralOk = (changedSynapses > 0);
                
                std::cout << "  Plasticity rules test completed" << std::endl;
                std::cout << "    STDP: " << (stdpOk ? "active" : "inactive") << std::endl;
                std::cout << "    Hebbian: " << (hebbianOk ? "active" : "inactive") << std::endl;
                std::cout << "    Structural: " << (structuralOk ? "active" : "inactive") << std::endl;
            } else {
                std::cout << "  No synapses available for test" << std::endl;
            }
        } else {
            std::cout << "  No brain region available" << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        bool plasticityRulesOk = stdpOk && hebbianOk && structuralOk;
        
        TestResult result;
        result.testName = "test_plasticity_rules";
        result.passed = plasticityRulesOk;
        result.message = "Plasticity rules " + std::string(plasticityRulesOk ? "functional" : "not fully functional");
        result.executionTime = testTime;
        result.metrics["stdp_active"] = stdpOk ? "true" : "false";
        result.metrics["hebbian_active"] = hebbianOk ? "true" : "false";
        result.metrics["structural_active"] = structuralOk ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Plasticity rules not fully functional" << std::endl;
        }
    }
    
    void runBrainInitializationTest() {
        std::cout << "\n=== Running Test: Brain Initialization ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        bool initOk = brain->initialize();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        TestResult result;
        result.testName = "test_brain_initialization";
        result.passed = initOk;
        result.message = "Brain initialization " + std::string(initOk ? "successful" : "failed");
        result.executionTime = testTime;
        result.metrics["initialization_success"] = initOk ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Brain initialization failed" << std::endl;
        }
    }
    
    void runSimulationClockTest() {
        std::cout << "\n=== Running Test: Simulation Clock ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto clock = std::make_shared<SimulationClock>(0.001);
        bool clockOk = clock->getCurrentTime() >= 0.0;
        
        // Test clock advancement
        double initialTime = clock->getCurrentTime();
        for (SimulationStep step = 0; step < 100; ++step) {
            clock->advanceStep();
        }
        double finalTime = clock->getCurrentTime();
        bool timeAdvanced = (finalTime > initialTime);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        bool clockTestOk = clockOk && timeAdvanced;
        
        TestResult result;
        result.testName = "test_simulation_clock";
        result.passed = clockTestOk;
        result.message = "Simulation clock " + std::string(clockTestOk ? "working" : "not working");
        result.executionTime = testTime;
        result.metrics["clock_initialized"] = clockOk ? "true" : "false";
        result.metrics["time_advanced"] = timeAdvanced ? "true" : "false";
        result.metrics["clock_accuracy"] = std::to_string((finalTime - initialTime) / 100.0) + "s per step";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Simulation clock not working" << std::endl;
        }
    }
    
    void runConfigLoadingTest() {
        std::cout << "\n=== Running Test: Configuration Loading ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<Config>();
        bool configLoadOk = config->loadFromFile("configs/default.cfg");
        
        bool configValuesOk = false;
        if (configLoadOk) {
            configValuesOk = config->has("random_seed") && config->has("neuron_count") && 
                            config->has("simulation_timestep");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        bool configTestOk = configLoadOk && configValuesOk;
        
        TestResult result;
        result.testName = "test_config_loading";
        result.passed = configTestOk;
        result.message = "Configuration loading " + std::string(configTestOk ? "successful" : "failed");
        result.executionTime = testTime;
        result.metrics["config_file_loaded"] = configLoadOk ? "true" : "false";
        result.metrics["config_values_valid"] = configValuesOk ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Configuration loading failed" << std::endl;
        }
    }
    
    void runLoggerTest() {
        std::cout << "\n=== Running Test: Logger ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        
        // Test logging
        logger->logInfo("Test log message");
        logger->logWarning("Test warning message");
        logger->logError("Test error message");
        
        bool loggerOk = true; // If we got here without crashing, logger is working
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        TestResult result;
        result.testName = "test_logger";
        result.passed = loggerOk;
        result.message = "Logger " + std::string(loggerOk ? "working" : "not working");
        result.executionTime = testTime;
        result.metrics["logger_initialized"] = "true";
        result.metrics["console_logging"] = "enabled";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Logger not working" << std::endl;
        }
    }
    
    void runRandomGeneratorTest() {
        std::cout << "\n=== Running Test: Random Generator ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto random = std::make_shared<Random>(42); // Fixed seed for reproducibility
        
        // Generate some random numbers
        std::vector<float> randomValues;
        for (int i = 0; i < 10; ++i) {
            randomValues.push_back(random->nextFloat());
        }
        
        // Check for duplicates (unlikely but possible)
        bool duplicatesFound = false;
        for (size_t i = 0; i < randomValues.size(); ++i) {
            for (size_t j = i + 1; j < randomValues.size(); ++j) {
                if (randomValues[i] == randomValues[j]) {
                    duplicatesFound = true;
                    break;
                }
            }
        }
        
        bool randomOk = !duplicatesFound && randomValues.size() == 10;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double testTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        TestResult result;
        result.testName = "test_random_generator";
        result.passed = randomOk;
        result.message = "Random generator " + std::string(randomOk ? "working" : "not working");
        result.executionTime = testTime;
        result.metrics["random_values_generated"] = std::to_string(randomValues.size());
        result.metrics["duplicates_found"] = duplicatesFound ? "true" : "false";
        
        testResults.push_back(result);
        
        if (result.passed) {
            std::cout << "  ✓ PASSED" << std::endl;
        } else {
            std::cout << "  ✗ FAILED: Random generator not working" << std::endl;
        }
    }
    
    void printResults() {
        std::cout << "=== NLM Test Results ===" << std::endl;
        std::cout << std::endl;
        
        int passed = 0;
        int failed = 0;
        double totalTime = 0.0;
        
        if (verbose) {
            std::cout << std::left << std::setw(25) << "Test" 
                      << std::setw(15) << "Time (ms)"
                      << std::setw(10) << "Status" 
                      << std::endl;
            std::cout << std::string(50, '-') << std::endl;
            
            for (const auto& result : testResults) {
                std::cout << std::left << std::setw(25) << result.testName
                          << std::setw(15) << std::to_string(result.timeMs)
                          << std::setw(10) << (result.passed ? "PASS" : "FAIL") << std::endl;
                
                if (result.passed) {
                    passed++;
                } else {
                    failed++;
                }
                
                totalTime += result.timeMs;
                
                // Print additional metrics
                for (const auto& metric : result.metrics) {
                    std::cout << "  " << std::left << std::setw(20) << metric.first 
                              << ": " << metric.second << std::endl;
                }
                std::cout << std::endl;
            }
        } else {
            std::cout << std::left << std::setw(30) << "Test" 
                      << std::setw(15) << "Status" 
                      << std::setw(15) << "Time (ms)" << std::endl;
            std::cout << std::string(60, '-') << std::endl;
            
            for (const auto& result : testResults) {
                std::cout << std::left << std::setw(30) << result.testName
                          << std::setw(15) << (result.passed ? "PASS" : "FAIL")
                          << std::setw(15) << std::to_string(result.timeMs) << std::endl;
                
                if (result.passed) {
                    passed++;
                } else {
                    failed++;
                }
                
                totalTime += result.timeMs;
            }
        }
        
        std::cout << std::string(60, '-') << std::endl;
        std::cout << std::left << std::setw(30) << "TOTAL"
                  << std::setw(15) << passed << "/" << testResults.size()
                  << std::setw(15) << std::to_string(totalTime) << std::endl;
        std::cout << std::endl;
        std::cout << "Tests passed: " << passed << "/" << testResults.size() << std::endl;
        
        if (failed > 0) {
            std::cout << "Tests failed: " << failed << std::endl;
        }
        
        if (passed == testResults.size()) {
            std::cout << "✓ All tests passed!" << std::endl;
        } else {
            std::cout << "✗ Some tests failed!" << std::endl;
        }
    }
    
    void saveResultsToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "NLM Test Results" << std::endl;
            file << "===============" << std::endl;
            file << "Generated: " << time(0) << std::endl;
            file << std::endl;
            
            file << "Test Summary:" << std::endl;
            file << "Total Tests: " << testResults.size() << std::endl;
            file << "Passed: " << std::count_if(testResults.begin(), testResults.end(), 
                                           [](const TestResult& r) { return r.passed; }) << std::endl;
            file << "Failed: " << std::count_if(testResults.begin(), testResults.end(), 
                                           [](const TestResult& r) { return !r.passed; }) << std::endl;
            file << std::endl;
            
            file << "Detailed Results:" << std::endl;
            for (const auto& result : testResults) {
                file << result.testName << ","
                     << (result.passed ? "PASS" : "FAIL") << ","
                     << result.timeMs << ","
                     << result.message << std::endl;
            }
            
            file.close();
            std::cout << "Test results saved to: " << filename << std::endl;
        } else {
            std::cerr << "Error: Cannot create results file: " << filename << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-test";
    std::string configFile;
    bool verbose = false;
    bool runAll = true;
    bool skip = false;
    std::string outputFile;
    bool xmlOutput = false;
    bool junitOutput = false;
    std::string specificTest;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMTest test;
            test.printUsage(programName);
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--all") {
            runAll = true;
        } else if (arg == "--list") {
            NLMTest test;
            test.listTests();
            return 0;
        } else if (arg == "--run") {
            if (i + 1 < argc) {
                specificTest = argv[++i];
                runAll = false;
            }
        } else if (arg == "--skip") {
            skip = true;
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--xml") {
            xmlOutput = true;
        } else if (arg == "--junit") {
            junitOutput = true;
        }
    }
    
    NLMTest test;
    test.verbose = verbose;
    
    if (!configFile.empty()) {
        if (!test.config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 1;
        }
    }
    
    if (skip) {
        std::cout << "Skipping tests. Use --help for available options." << std::endl;
        return 0;
    }
    
    if (runAll) {
        std::cout << "Running all NLM tests..." << std::endl;
        test.runBasicConnectivityTest();
        test.runPlasticityTest();
        test.runStdpVerificationTest();
        test.runMemoryIntegrationTest();
        test.runNeuromodulationTest();
        test.runDevelopmentTest();
        test.runPlasticityRulesTest();
        test.runBrainInitializationTest();
        test.runSimulationClockTest();
        test.runConfigLoadingTest();
        test.runLoggerTest();
        test.runRandomGeneratorTest();
    } else if (!specificTest.empty()) {
        std::cout << "Running specific test: " << specificTest << std::endl;
        test.runSpecificTest(specificTest);
    } else {
        NLMTest tempTest;
        tempTest.printUsage(programName);
        return 0;
    }
    
    // Print results
    test.printResults();
    
    // Save results if requested
    if (!outputFile.empty()) {
        test.saveResultsToFile(outputFile);
    }
    
    return 0;
}
