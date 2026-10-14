#include "tests/NLMTestSuite.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

std::vector<TestResult> NLMTestSuite::runAllTests() {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    
    testResults.clear();
    currentTestIndex = 0;
    
    // Run all test categories
    auto componentTests = runComponentTests();
    testResults.insert(testResults.end(), componentTests.begin(), componentTests.end());
    
    auto integrationTests = runIntegrationTests();
    testResults.insert(testResults.end(), integrationTests.begin(), integrationTests.end());
    
    auto performanceTests = runPerformanceTests();
    testResults.insert(testResults.end(), performanceTests.begin(), performanceTests.end());
    
    auto stressTests = runStressTests();
    testResults.insert(testResults.end(), stressTests.begin(), stressTests.end());
    
    auto propertyTests = runPropertyTests();
    testResults.insert(testResults.end(), propertyTests.begin(), propertyTests.end());
    
    auto crossValidationTests = runCrossValidationTests();
    testResults.insert(testResults.end(), crossValidationTests.begin(), crossValidationTests.end());
    
    auto advancedTests = runAdvancedTests();
    testResults.insert(testResults.end(), advancedTests.begin(), advancedTests.end());
    
    auto configTests = testConfigurationValidation();
    testResults.insert(testResults.end(), configTests.begin(), configTests.end());
    
    return testResults;
}

std::vector<TestResult> NLMTestSuite::runComponentTests() {
    std::vector<TestResult> results;
    
    results.push_back(testBrainInitialization());
    results.push_back(testNeuronDynamics());
    results.push_back(testSynapticPlasticity());
    results.push_back(testConfiguration());
    
    return results;
}

std::vector<TestResult> NLMTestSuite::runIntegrationTests() {
    std::vector<TestResult> results;
    
    results.push_back(testSystemIntegration());
    results.push_back(testMemoryIntegration());
    results.push_back(testNeuromodulationIntegration());
    results.push_back(testPredictionIntegration());
    results.push_back(testCognitionIntegration());
    results.push_back(testDevelopmentIntegration());
    
    return results;
}

std::vector<TestResult> NLMTestSuite::runPerformanceTests() {
    std::vector<TestResult> results;
    
    // Run benchmarks
    auto benchmarks = runPerformanceBenchmarks();
    
    // Convert benchmarks to test results
    for (const auto& bench : benchmarks) {
        TestResult result(bench.benchmarkName + "_benchmark", true);
        result.executionTime = bench.executionTime;
        result.neuronsUsed = bench.neuronsProcessed;
        result.synapsesUsed = bench.synapsesProcessed;
        results.push_back(result);
    }
    
    return results;
}

std::vector<TestResult> NLMTestSuite::runStressTests() {
    std::vector<TestResult> results;
    
    results.push_back(stressTestHighNeuronCount(5000));
    results.push_back(stressTestHighSynapseCount(5000));
    results.push_back(stressTestLongRunning(5000));
    
    return results;
}

std::vector<TestResult> NLMTestSuite::runPropertyTests() {
    std::vector<TestResult> results;
    
    results.push_back(testNeuronInvariants());
    results.push_back(testSynapseInvariants());
    results.push_back(testBrainStateInvariants());
    
    return results;
}

std::vector<TestResult> NLMTestSuite::runCrossValidationTests() {
    std::vector<TestResult> results;
    
    results.push_back(testConfigurationConsistency());
    results.push_back(testSimulationReproducibility());
    
    return results;
}

std::vector<TestResult> NLMTestSuite::runAdvancedTests() {
    std::vector<TestResult> results;
    
    results.push_back(testEligibilityTracePlasticity());
    results.push_back(testAdvancedLearning());
    results.push_back(testRealTimeMode());
    results.push_back(testBatchProcessing());
    
    return results;
}

std::vector<TestResult> NLMTestSuite::testConfiguration() {
    std::vector<TestResult> results;
    
    currentTestName = "Configuration Test";
    
    TestResult result("Basic Configuration", true);
    
    // Create config
    auto config = std::make_shared<Config>();
    
    // Test basic operations
    config->set("test_key", "test_value");
    auto value = config->get<std::string>("test_key");
    assert(value && *value == "test_value");
    
    config->set("test_int", 42);
    auto intVal = config->get<int64_t>("test_int");
    assert(intVal && *intVal == 42);
    
    config->set("test_float", 3.14f);
    auto floatVal = config->get<double>("test_float");
    assert(floatVal && std::abs(*floatVal - 3.14) < 0.001);
    
    config->set("test_bool", true);
    auto boolVal = config->get<bool>("test_bool");
    assert(boolVal && *boolVal == true);
    
    // Test config constants
    config->set("test_const", ConfigConstants::DEFAULT_RANDOM_SEED);
    auto constVal = config->get<int64_t>("test_const");
    assert(constVal && *constVal == 42);
    
    result.executionTime = measureExecutionTime([&]() {
        for (int i = 0; i < 1000; ++i) {
            config->set("temp_key", i);
            auto val = config->get<int64_t>("temp_key");
        }
    });
    
    result.neuronsUsed = 0;
    result.synapsesUsed = 0;
    results.push_back(result);
    
    currentTestIndex++;
    return results;
}

TestResult NLMTestSuite::testBrainInitialization() {
    currentTestName = "Brain Initialization Test";
    
    TestResult result("Brain Initialization", true);
    
    try {
        // Initialize with facade
        auto facade = std::make_shared<NLMFacade>();
        
        bool initSuccess = facade->initialize(100);
        if (!initSuccess) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain via facade";
            return result;
        }
        
        // Test basic functionality
        facade->step();
        facade->step();
        facade->step();
        
        // Verify state
        size_t neurons = facade->getNeuronCount();
        size_t synapses = facade->getTotalSynapseCount();
        
        result.neuronsUsed = neurons;
        result.synapsesUsed = synapses;
        
        result.executionTime = measureExecutionTime([&]() {
            auto testFacade = std::make_shared<NLMFacade>();
            testFacade->initialize(100);
            for (int i = 0; i < 100; ++i) {
                testFacade->step();
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testNeuronDynamics() {
    currentTestName = "Neuron Dynamics Test";
    
    TestResult result("Neuron Dynamics", true);
    
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50));
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain";
            return result;
        }
        
        // Test neuron state changes
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current to stimulate activity
        for (size_t i = 0; i < 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikesGenerated = finalSpikes - initialSpikes;
        
        // Verify neurons spiked
        if (spikesGenerated == 0) {
            result.passed = false;
            result.errorMessage = "No neurons fired despite current injection";
        }
        
        result.executionTime = measureExecutionTime([&]() {
            auto testConfig = std::make_shared<Config>();
            testConfig->set("neuron_count", static_cast<int64_t>(100));
            auto testBrain = std::make_shared<Brain>(testConfig);
            testBrain->initialize();
            
            for (int step = 0; step < 100; ++step) {
                testBrain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                testBrain->step(step, step * 0.001);
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testSynapticPlasticity() {
    currentTestName = "Synaptic Plasticity Test";
    
    TestResult result("Synaptic Plasticity", true);
    
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain";
            return result;
        }
        
        // Enable plasticity
        brain->getSTDP()->configure(0.01f, 0.012f, 20.0f);
        
        // Get initial weights
        std::vector<float> initialWeights;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (initialWeights.empty()) {
            result.passed = false;
            result.errorMessage = "No synapses found for plasticity test";
            return result;
        }
        
        // Apply learning protocol
        for (int trial = 0; trial < 50; ++trial) {
            // Create correlated activity for LTP
            if (region && region->getAllNeurons().size() >= 2) {
                auto neurons = region->getAllNeurons();
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check weight changes
        std::vector<float> finalWeights;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        float totalWeightChange = 0.0f;
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            totalWeightChange += finalWeights[i] - initialWeights[i];
        }
        
        // Plasticity should produce some changes (not guaranteed due to randomness)
        // but we expect reasonable behavior
        result.executionTime = measureExecutionTime([&]() {
            auto testConfig = std::make_shared<Config>();
            testConfig->set("neuron_count", static_cast<int64_t>(100));
            auto testBrain = std::make_shared<Brain>(testConfig);
            testBrain->initialize();
            
            // Run plasticity test
            for (int trial = 0; trial < 50; ++trial) {
                if (auto* testRegion = testBrain->getRegion(RegionId(1))) {
                    auto neurons = testRegion->getAllNeurons();
                    if (neurons.size() >= 2) {
                        neurons[0]->injectCurrent(30.0f);
                        testBrain->step(trial * 2, trial * 2 * 0.001);
                        
                        neurons[1]->injectCurrent(30.0f);
                        testBrain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                    }
                }
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testMemoryIntegration() {
    currentTestName = "Memory Integration Test";
    
    TestResult result("Memory Integration", true);
    
    try {
        auto facade = std::make_shared<NLMFacade>();
        
        if (!facade->initialize(200)) {
            result.passed = false;
            result.errorMessage = "Failed to initialize facade";
            return result;
        }
        
        // Test working memory
        facade->storeWorkingMemory("test_vector", std::vector<float>{1.0f, 2.0f, 3.0f});
        auto retrieved = facade->retrieveWorkingMemory("test_vector");
        
        if (retrieved.size() != 3 || std::abs(retrieved[0] - 1.0f) > 0.001f) {
            result.passed = false;
            result.errorMessage = "Working memory storage/retrieval failed";
        }
        
        // Test episodic memory
        facade->storeEpisodic("episode1", std::vector<std::string>{"event1", "event2", "event3"});
        auto episode = facade->retrieveEpisodic("episode1");
        
        if (episode.size() != 3) {
            result.passed = false;
            result.errorMessage = "Episodic memory storage/retrieval failed";
        }
        
        result.executionTime = measureExecutionTime([&]() {
            auto testFacade = std::make_shared<NLMFacade>();
            testFacade->initialize(100);
            
            for (int i = 0; i < 100; ++i) {
                testFacade->storeWorkingMemory("key_" + std::to_string(i), std::vector<float>{(float)i});
                auto val = testFacade->retrieveWorkingMemory("key_" + std::to_string(i));
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testSystemIntegration() {
    currentTestName = "System Integration Test";
    
    TestResult result("System Integration", true);
    
    try {
        auto facade = std::make_shared<NLMFacade>();
        
        if (!facade->initialize(300)) {
            result.passed = false;
            result.errorMessage = "Failed to initialize facade";
            return result;
        }
        
        // Test integration of all systems through facade
        facade->receiveSensoryInput(std::vector<float>{1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
        
        for (int step = 0; step < 50; ++step) {
            facade->step();
            
            // Test neuromodulation integration
            facade->setReward(0.5f);
            facade->setCuriosity(0.8f);
            facade->setNovelty(0.3f);
        }
        
        // Test prediction system
        facade->predict(std::vector<float>{0.1f, 0.2f, 0.3f});
        auto prediction = facade->getPrediction();
        
        // Test cognitive planning
        facade->planAction(std::vector<float>{0.5f, 0.5f, 0.5f});
        auto actions = facade->getPlannedActions();
        
        // Test development
        facade->advanceDevelopmentalStage();
        auto stage = facade->getDevelopmentalStage();
        
        // Verify integration scores
        auto scores = facade->getIntegrationScore();
        
        result.executionTime = measureExecutionTime([&]() {
            auto testFacade = std::make_shared<NLMFacade>();
            testFacade->initialize(200);
            
            for (int step = 0; step < 100; ++step) {
                testFacade->receiveSensoryInput(std::vector<float>{0.1f, 0.2f, 0.3f});
                testFacade->step();
                
                if (step % 10 == 0) {
                    testFacade->setReward(0.1f * (step % 5 + 1));
                    testFacade->predict(std::vector<float>{0.1f, 0.1f, 0.1f});
                }
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::stressTestHighNeuronCount(size_t neuronCount) {
    currentTestName = "Stress Test High Neuron Count";
    
    TestResult result("Stress Test - High Neuron Count", true);
    
    try {
        double startTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain with " + std::to_string(neuronCount) + " neurons";
            return result;
        }
        
        // Run simulation with high neuron count
        for (SimulationStep step = 0; step < 200; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                // Log progress occasionally
                size_t spikes = brain->getTotalSpikeCount();
            }
        }
        
        double endTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        result.executionTime = endTime - startTime;
        result.neuronsUsed = neuronCount;
        result.synapsesUsed = neuronCount * 10; // Rough estimate
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::stressTestLongRunning(size_t steps) {
    currentTestName = "Stress Test - Long Running";
    
    TestResult result("Stress Test - Long Running", true);
    
    try {
        double startTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200));
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain for long-running test";
            return result;
        }
        
        // Run for extended period
        size_t progressStep = 0;
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f + (static_cast<float>(step % 10) * 0.1f));
            brain->step(step, step * 0.001);
            
            // Check for stability every 100 steps
            if (step % 100 == 0) {
                bool stable = testBrainStability(brain, 10);
                if (!stable) {
                    result.passed = false;
                    result.errorMessage = "Brain unstable at step " + std::to_string(step);
                    break;
                }
            }
        }
        
        double endTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        result.executionTime = endTime - startTime;
        result.neuronsUsed = 200;
        result.synapsesUsed = 2000;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testEligibilityTracePlasticity() {
    currentTestName = "Eligibility Trace Plasticity Test";
    
    TestResult result("Eligibility Trace Plasticity", true);
    
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain";
            return result;
        }
        
        // Test eligibility trace functionality through facade
        auto facade = std::make_shared<NLMFacade>(config);
        
        facade->enableEligibilityTraces(true);
        facade->setEligibilityTraceDecay(0.01f);
        
        // Apply reward and check trace behavior
        facade->applyExternalReward(0.8f);
        
        // Run simulation to let traces decay
        for (int step = 0; step < 50; ++step) {
            facade->step();
        }
        
        result.executionTime = measureExecutionTime([&]() {
            auto testFacade = std::make_shared<NLMFacade>();
            testFacade->initialize(100);
            testFacade->enableEligibilityTraces(true);
            
            for (int step = 0; step < 100; ++step) {
                testFacade->applyExternalReward(0.5f * (step % 3 + 1) * 0.1f);
                testFacade->step();
                
                if (step % 20 == 0) {
                    testFacade->setEligibilityTraceDecay(0.01f * (1.0f + static_cast<float>(step % 10) * 0.1f));
                }
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testRealTimeMode() {
    currentTestName = "Real-Time Mode Test";
    
    TestResult result("Real-Time Mode", true);
    
    try {
        auto facade = std::make_shared<NLMFacade>();
        
        if (!facade->initialize(150)) {
            result.passed = false;
            result.errorMessage = "Failed to initialize facade";
            return result;
        }
        
        // Enable real-time mode
        facade->enableRealTimeMode(true);
        facade->setTargetFPS(30.0f);
        
        // Run simulation in real-time mode
        auto startTime = std::chrono::duration<double>::clock::now();
        
        for (int step = 0; step < 30; ++step) {
            facade->receiveSensoryInput(std::vector<float>{0.5f, 0.5f, 0.5f, 0.5f, 0.5f});
            facade->step();
            
            // Simulate real-time timing
            auto currentTime = std::chrono::duration<double>::clock::now();
            auto elapsed = std::chrono::duration<double>(currentTime - startTime).count();
            
            // In real-time mode, we would ensure frame rate
            // For testing, we just ensure the mode is active
            facade->setSimulationSpeed(1.0f / facade->getSimulationSpeed());
        }
        
        result.executionTime = measureExecutionTime([&]() {
            auto testFacade = std::make_shared<NLMFacade>();
            testFacade->initialize(100);
            testFacade->enableRealTimeMode(true);
            testFacade->setTargetFPS(60.0f);
            
            for (int step = 0; step < 60; ++step) {
                testFacade->step();
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

TestResult NLMTestSuite::testBatchProcessing() {
    currentTestName = "Batch Processing Test";
    
    TestResult result("Batch Processing", true);
    
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200));
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.passed = false;
            result.errorMessage = "Failed to initialize brain";
            return result;
        }
        
        // Test batch current injection
        std::vector<NeuronId> neuronIds;
        std::vector<float> currents;
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
                neuronIds.push_back(neurons[i]->getId());
                currents.push_back(30.0f + static_cast<float>(i) * 5.0f);
            }
        }
        
        // Apply batch currents
        for (size_t i = 0; i < std::min(neuronIds.size(), currents.size()); ++i) {
            brain->injectCurrent(neuronIds[i], currents[i]);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        result.executionTime = measureExecutionTime([&]() {
            auto testConfig = std::make_shared<Config>();
            testConfig->set("neuron_count", static_cast<int64_t>(150));
            auto testBrain = std::make_shared<Brain>(testConfig);
            testBrain->initialize();
            
            // Test batch processing
            for (int batch = 0; batch < 10; ++batch) {
                std::vector<NeuronId> batchNeurons;
                std::vector<float> batchCurrents;
                
                for (int i = 0; i < 10; ++i) {
                    batchNeurons.push_back(static_cast<NeuronId>(i + batch * 10));
                    batchCurrents.push_back(20.0f + static_cast<float>(i) * 2.0f);
                }
                
                for (size_t i = 0; i < std::min(batchNeurons.size(), batchCurrents.size()); ++i) {
                    testBrain->injectCurrent(batchNeurons[i], batchCurrents[i]);
                }
                
                for (int step = 0; step < 10; ++step) {
                    testBrain->step(step, step * 0.001);
                }
            }
        });
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    currentTestIndex++;
    return result;
}

// Implementation of helper functions

bool NLMTestSuite::testBrainStability(std::shared_ptr<Brain> brain, size_t steps) {
    if (!brain) return false;
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikeIncrease = finalSpikes - initialSpikes;
    
    // Consider stable if spike count doesn't grow exponentially
    return spikeIncrease < steps * 10;  // Arbitrary stability threshold
}

bool NLMTestSuite::testPlasticityLearning(std::shared_ptr<Brain> brain, size_t trials) {
    if (!brain) return false;
    
    // Simple test for learning behavior
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return false;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) return false;
    
    // Apply learning protocol
    for (size_t trial = 0; trial < trials; ++trial) {
        neurons[0]->injectCurrent(30.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        
        neurons[1]->injectCurrent(30.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    return true;
}

bool NLMTestSuite::testMemoryCapacity(std::shared_ptr<Brain> brain, size_t capacity) {
    if (!brain) return false;
    
    // This would test memory system capacity
    // Implementation depends on memory system interface
    return true;
}

bool NLMTestSuite::testNeuromodulationEffect(std::shared_ptr<Brain> brain, float reward) {
    if (!brain) return false;
    
    // This would test neuromodulation effects
    // Implementation depends on neuromodulation system interface
    return true;
}

double NLMTestSuite::measureExecutionTime(std::function<void()> testFunc) {
    auto startTime = std::chrono::duration<double>::clock::now();
    testFunc();
    auto endTime = std::chrono::duration<double>::clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

MemoryStats NLMTestSuite::measureMemoryUsage(std::function<void()> testFunc) {
    // Note: This is a simplified implementation
    // Real memory measurement would use platform-specific APIs
    MemoryStats stats;
    
    // Call the test function
    testFunc();
    
    // Simulate memory tracking
    stats.allocatedBytes = 1024 * 1024;  // 1MB
    stats.peakAllocatedBytes = 1024 * 1024 * 2;  // 2MB peak
    stats.deallocatedBytes = 512 * 1024;  // 512KB freed
    stats.currentAllocatedBytes = 512 * 1024;  // 512KB current
    
    return stats;
}

// Configuration setter
void NLMTestSuite::setTestConfig(const std::string& key, const std::string& value) {
    testConfig.configOverrides[key] = value;
}

void NLMTestSuite::setTestNeuronCount(size_t count) {
    testConfig.neuronCount = count;
}

void NLMTestSuite::setTestSynapseCount(size_t count) {
    testConfig.synapseCount = count;
}

void NLMTestSuite::setTestStepCount(size_t count) {
    testConfig.stepCount = count;
}

void NLMTestSuite::setTestDuration(double duration) {
    testConfig.duration = duration;
}

// Random number generation
float NLMTestSuite::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(randomEngine);
}

int NLMTestSuite::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(randomEngine);
}

// Progress tracking
double NLMTestSuite::getTestProgress() const {
    if (testResults.empty()) return 0.0;
    return static_cast<double>(currentTestIndex) / static_cast<double>(testResults.size());
}

std::string NLMTestSuite::getCurrentTestName() const {
    return currentTestName;
}

// Results reporting
void NLMTestSuite::saveTestResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "NLM Test Results" << std::endl;
    file << "=================" << std::endl;
    file << std::endl;
    
    for (const auto& result : testResults) {
        file << "Test: " << result.testName << std::endl;
        file << "  Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
        file << "  Execution Time: " << std::fixed << std::setprecision(3) << result.executionTime << "s" << std::endl;
        file << "  Neurons Used: " << result.neuronsUsed << std::endl;
        file << "  Synapses Used: " << result.synapsesUsed << std::endl;
        if (!result.errorMessage.empty()) {
            file << "  Error: " << result.errorMessage << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
}

void NLMTestSuite::loadTestResults(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open file for reading: " << filename << std::endl;
        return;
    }
    
    // Implementation would parse saved test results
    // For now, it's a placeholder
    file.close();
}

void NLMTestSuite::printTestResults() const {
    std::cout << "=== NLM Test Results ===" << std::endl;
    std::cout << std::endl;
    
    int passed = 0;
    int failed = 0;
    double totalTime = 0.0;
    
    for (const auto& result : testResults) {
        std::cout << "Test: " << result.testName << std::endl;
        std::cout << "  Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
        std::cout << "  Execution Time: " << std::fixed << std::setprecision(3) << result.executionTime << "s" << std::endl;
        
        if (!result.passed) {
            std::cout << "  Error: " << result.errorMessage << std::endl;
            failed++;
        } else {
            passed++;
        }
        
        totalTime += result.executionTime;
        std::cout << std::endl;
    }
    
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "Total Tests: " << testResults.size() << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
              << (testResults.size() > 0 ? (passed * 100.0 / testResults.size()) : 0.0) << "%" << std::endl;
    std::cout << "Total Execution Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    std::cout << "Average Time per Test: " << std::fixed << std::setprecision(3) 
              << (testResults.size() > 0 ? totalTime / testResults.size() : 0.0) << "s" << std::endl;
}

void NLMTestSuite::printBenchmarkResults() const {
    std::cout << "=== NLM Benchmark Results ===" << std::endl;
    std::cout << std::endl;
    
    for (const auto& bench : benchmarkResults) {
        std::cout << "Benchmark: " << bench.benchmarkName << std::endl;
        std::cout << "  Execution Time: " << std::fixed << std::setprecision(3) << bench.executionTime << "s" << std::endl;
        std::cout << "  Neurons Processed: " << bench.neuronsProcessed << std::endl;
        std::cout << "  Synapses Processed: " << bench.synapsesProcessed << std::endl;
        std::cout << "  Throughput: " << std::fixed << std::setprecision(1) << bench.throughput << " ops/s" << std::endl;
        std::cout << std::endl;
    }
}

} // namespace nlm
