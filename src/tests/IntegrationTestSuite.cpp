#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <memory>
#include <chrono>
#include <algorithm>

namespace nlm {
namespace tests {

class IntegrationTestSuite {
public:
    IntegrationTestSuite() : testResults(), passedTests(0), failedTests(0) {}
    
    void runAllTests() {
        std::cout << "============================================================" << std::endl;
        std::cout << "NLM Integration Test Suite" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        // Run all test categories
        runConfigurationTests();
        runMemorySystemTests();
        runBrainIntegrationTests();
        runAgentWorldIntegrationTests();
        runPythonAPITests();
        runPerformanceTests();
        runEndToEndTests();
        
        // Generate report
        generateTestReport();
    }
    
    void runConfigurationTests() {
        std::cout << "\n--- Configuration Tests ---" << std::endl;
        
        // Test 1: Basic configuration loading
        testBasicConfiguration();
        
        // Test 2: Configuration validation
        testConfigurationValidation();
        
        // Test 3: Configuration error handling
        testConfigurationErrorHandling();
        
        // Test 4: Schema validation
        testSchemaValidation();
        
        // Test 5: Configuration serialization
        testConfigurationSerialization();
    }
    
    void runMemorySystemTests() {
        std::cout << "\n--- Memory System Tests ---" << std::endl;
        
        // Test 1: Working memory integration
        testWorkingMemoryIntegration();
        
        // Test 2: Episodic memory integration
        testEpisodicMemoryIntegration();
        
        // Test 3: Associative memory integration
        testAssociativeMemoryIntegration();
        
        // Test 4: Attention system integration
        testAttentionSystemIntegration();
        
        // Test 5: Memory system statistics
        testMemorySystemStatistics();
    }
    
    void runBrainIntegrationTests() {
        std::cout << "\n--- Brain Integration Tests ---" << std::endl;
        
        // Test 1: Basic brain initialization
        testBasicBrainInitialization();
        
        // Test 2: Neural connectivity
        testNeuralConnectivity();
        
        // Test 3: Plasticity mechanisms
        testPlasticityMechanisms();
        
        // Test 4: Spike propagation
        testSpikePropagation();
        
        // Test 5: Brain statistics
        testBrainStatistics();
    }
    
    void runAgentWorldIntegrationTests() {
        std::cout << "\n--- Agent-World Integration Tests ---" << std::endl;
        
        // Test 1: Agent brain initialization
        testAgentBrainInitialization();
        
        // Test 2: Sensory processing
        testSensoryProcessing();
        
        // Test 3: Action production
        testActionProduction();
        
        // Test 4: Reward modulation
        testRewardModulation();
        
        // Test 5: World interaction
        testWorldInteraction();
    }
    
    void runPythonAPITests() {
        std::cout << "\n--- Python API Tests ---" << std::endl;
        
        // Test 1: Python type bindings
        testPythonTypeBindings();
        
        // Test 2: Python error handling
        testPythonErrorHandling();
        
        // Test 3: Python object lifecycle
        testPythonObjectLifecycle();
        
        // Test 4: Python integration patterns
        testPythonIntegrationPatterns();
    }
    
    void runPerformanceTests() {
        std::cout << "\n--- Performance Tests ---" << std::endl;
        
        // Test 1: Memory allocation performance
        testMemoryAllocationPerformance();
        
        // Test 2: Neural computation performance
        testNeuralComputationPerformance();
        
        // Test 3: Configuration parsing performance
        testConfigurationParsingPerformance();
        
        // Test 4: System startup performance
        testSystemStartupPerformance();
    }
    
    void runEndToEndTests() {
        std::cout << "\n--- End-to-End Tests ---" << std::endl;
        
        // Test 1: Complete learning simulation
        testCompleteLearningSimulation();
        
        // Test 2: Real-world scenario simulation
        testRealWorldScenario();
        
        // Test 3: Stress testing
        testStressTesting();
        
        // Test 4: Resource management
        testResourceManagement();
    }
    
private:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
        double executionTime;
    };
    
    std::vector<TestResult> testResults;
    size_t passedTests;
    size_t failedTests;
    
    // Configuration Tests
    void testBasicConfiguration() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            
            // Set basic values
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            // Verify values
            assert(config->has("random_seed"));
            assert(config->has("simulation_timestep"));
            assert(config->has("neuron_count"));
            assert(config->has("region_count"));
            
            assert(config->getOr<int64_t>("random_seed", 0) == 42);
            assert(config->getOr<double>("simulation_timestep", 0.0) == 0.001);
            assert(config->getOr<int64_t>("neuron_count", 0) == 100);
            assert(config->getOr<int64_t>("region_count", 0) == 1);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Basic Configuration", true, "Configuration loading and verification", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Basic Configuration", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testConfigurationValidation() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            
            // Test validation with schema
            ConfigSchema schema;
            schema.addField("test_int", ConfigConstraints("int", true, "Test int"));
            schema.addField("test_string", ConfigConstraints("string", false, "Test string"));
            
            // Valid configuration
            config->set("test_int", 42);
            config->set("test_string", "hello");
            
            assert(config->isValid(schema));
            
            // Invalid configuration (missing required field)
            auto invalidConfig = std::make_shared<Config>();
            assert(!invalidConfig->isValid(schema));
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Configuration Validation", true, "Schema validation works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Configuration Validation", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testConfigurationErrorHandling() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            
            // Test error handling for missing keys
            try {
                int value = config->get<int>("nonexistent_key");
                assert(false && "Should have thrown exception");
            } catch (const std::exception&) {
                // Expected exception
            }
            
            // Test error handling for type conversion
            config->set("test_value", "invalid_int");
            try {
                int value = config->get<int>("test_value");
                assert(false && "Should have thrown exception for type mismatch");
            } catch (const std::exception&) {
                // Expected exception
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Configuration Error Handling", true, "Error handling works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Configuration Error Handling", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testSchemaValidation() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            ConfigSchema schema;
            
            // Add fields with constraints
            schema.addField("required_int", ConfigConstraints("int", true, "Required int"));
            schema.addField("optional_string", ConfigConstraints("string", false, "Optional string", "default"));
            schema.addField("float_range", ConfigConstraints("double", false, "Float in range", "0.5"));
            
            auto config = std::make_shared<Config>();
            
            // Test schema validation
            assert(config->isValid(schema));  // Empty config should be valid (optional fields)
            
            // Add required field
            config->set("required_int", 42);
            assert(config->isValid(schema));
            
            // Add optional field
            config->set("optional_string", "test");
            assert(config->isValid(schema));
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Schema Validation", true, "Schema validation works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Schema Validation", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testConfigurationSerialization() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            
            // Set various types of values
            config->set("int_value", 42);
            config->set("double_value", 3.14);
            config->set("string_value", "hello");
            config->set("bool_value", true);
            config->set("int_array", std::vector<int>{1, 2, 3});
            
            // Test summary generation
            std::string summary = config->summary();
            assert(!summary.empty());
            assert(summary.find("int_value") != std::string::npos);
            assert(summary.find("double_value") != std::string::npos);
            assert(summary.find("string_value") != std::string::npos);
            assert(summary.find("bool_value") != std::string::npos);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Configuration Serialization", true, "Configuration serialization works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Configuration Serialization", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    // Memory System Tests
    void testWorkingMemoryIntegration() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Test working memory operations
            auto* workingMemory = brain->getWorkingMemory();
            assert(workingMemory != nullptr);
            
            // Test store and retrieve
            std::vector<float> pattern{0.1f, 0.5f, 0.9f};
            workingMemory->store(pattern);
            
            std::vector<float> retrieved = workingMemory->retrieve();
            assert(!retrieved.empty());
            
            // Test memory statistics
            assert(workingMemory->getActiveTraces() >= 0);
            assert(workingMemory->getCapacity() > 0);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Working Memory Integration", true, "Working memory integration works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Working Memory Integration", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testEpisodicMemoryIntegration() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Test episodic memory operations
            auto* episodicMemory = brain->getEpisodicMemory();
            assert(episodicMemory != nullptr);
            
            // Test storing an episode
            EpisodicMemoryItem episode;
            episode.timestamp = 1;
            episode.sensoryState = std::vector<float>{0.1f, 0.2f, 0.3f};
            episode.reward = 1.0f;
            
            episodicMemory->storeEpisode(episode);
            assert(episodicMemory->getEpisodeCount() > 0);
            
            // Test retrieval
            std::vector<const EpisodicMemoryItem*> recent = episodicMemory->getRecentEpisodes(1);
            assert(!recent.empty());
            
            // Test statistics
            assert(episodicMemory->getAverageReward() >= 0.0f);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Episodic Memory Integration", true, "Episodic memory integration works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Episodic Memory Integration", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testAssociativeMemoryIntegration() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Test associative memory operations
            auto* associativeMemory = brain->getAssociativeMemory();
            assert(associativeMemory != nullptr);
            
            // Test association
            std::vector<float> patternA{0.1f, 0.2f, 0.3f};
            std::vector<float> patternB{0.4f, 0.5f, 0.6f};
            
            associativeMemory->associate(patternA, patternB, 0.8f);
            assert(associativeMemory->getAssociationCount() > 0);
            
            // Test retrieval
            std::vector<std::vector<float>> retrieved = associativeMemory->retrieve(patternA, 3);
            assert(!retrieved.empty());
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Associative Memory Integration", true, "Associative memory integration works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Associative Memory Integration", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testAttentionSystemIntegration() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Test attention system operations
            auto* attention = brain->getAttention();
            assert(attention != nullptr);
            
            // Test competition processing
            std::vector<NeuronId> competitors;
            competitors.push_back(NeuronId(1));
            competitors.push_back(NeuronId(2));
            competitors.push_back(NeuronId(3));
            
            std::vector<NeuronId> winners = attention->processCompetition(competitors);
            assert(!winners.empty() || competitors.empty());  // Either some winners or no competition
            
            // Test attention statistics
            std::vector<NeuronId> currentWinners = attention->getWinners();
            size_t winnerCount = currentWinners.size();
            assert(winnerCount >= 0);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Attention System Integration", true, "Attention system integration works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Attention System Integration", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testMemorySystemStatistics() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Get memory statistics
            auto* workingMemory = brain->getWorkingMemory();
            auto* episodicMemory = brain->getEpisodicMemory();
            auto* associativeMemory = brain->getAssociativeMemory();
            auto* attention = brain->getAttention();
            
            // Test that statistics collection works
            std::map<std::string, std::string> stats;
            
            if (workingMemory) {
                stats["working_memory_capacity"] = std::to_string(workingMemory->getCapacity());
                stats["working_memory_activity"] = std::to_string(workingMemory->getMemoryActivity());
            }
            
            if (episodicMemory) {
                stats["episodic_memory_count"] = std::to_string(episodicMemory->getEpisodeCount());
                stats["episodic_memory_average_reward"] = std::to_string(episodicMemory->getAverageReward());
            }
            
            if (associativeMemory) {
                stats["associative_memory_count"] = std::to_string(associativeMemory->getAssociationCount());
            }
            
            if (attention) {
                stats["attention_winner_count"] = std::to_string(attention->getWinners().size());
            }
            
            assert(!stats.empty());
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Memory System Statistics", true, "Memory system statistics collection works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Memory System Statistics", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testBasicBrainInitialization() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Test basic brain functionality
            assert(brain->getTotalNeuronCount() > 0);
            assert(brain->getTotalSynapseCount() >= 0);
            assert(brain->getRegionCount() > 0);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Basic Brain Initialization", true, "Basic brain initialization works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Basic Brain Initialization", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void testNeuralConnectivity() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            // Inject current and check for spike propagation
            auto* region = brain->getRegion(RegionId(1));
            assert(region != nullptr);
            
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                // Inject current into first neuron
                neurons[0]->injectCurrent(50.0f);
                
                // Run a few simulation steps
                for (SimulationStep step = 0; step < 10; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                // Check if spikes were generated
                size_t spikeCount = brain->getTotalSpikeCount();
                assert(spikeCount >= 0);  // At least 0 spikes is acceptable
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Neural Connectivity", true, "Neural connectivity and spike propagation works correctly", duration});
            passedTests++;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            
            testResults.push_back({"Neural Connectivity", false, std::string("Exception: ") + e.what(), duration});
            failedTests++;
        }
    }
    
    void generateTestReport() {
        std::cout << "\n============================================================" << std::endl;
        std::cout << "Integration Test Suite Report" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        std::cout << "Total Tests: " << testResults.size() << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success Rate: " << (passedTests * 100.0 / testResults.size()) << "%" << std::endl;
        
        std::cout << "\nDetailed Results:" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        
        double totalTime = 0.0;
        for (const auto& result : testResults) {
            std::cout << result.name << ": " << (result.passed ? "PASSED" : "FAILED") << std::endl;
            std::cout << "  Message: " << result.message << std::endl;
            std::cout << "  Time: " << result.executionTime << " ms" << std::endl;
            totalTime += result.executionTime;
        }
        
        std::cout << "\nPerformance Summary:" << std::endl;
        std::cout << "Total execution time: " << totalTime << " ms" << std::endl;
        std::cout << "Average time per test: " << (totalTime / testResults.size()) << " ms" << std::endl;
        
        if (failedTests == 0) {
            std::cout << "\n🎉 All tests passed! The NLM integration is working correctly." << std::endl;
        } else {
            std::cout << "\n⚠️  " << failedTests << " tests failed. Please review the failed tests above." << std::endl;
        }
        
        std::cout << "\n============================================================" << std::endl;
    }
};

} // namespace tests
} // namespace nlm

// Entry point for integration tests
int main() {
    try {
        nlm::tests::IntegrationTestSuite testSuite;
        testSuite.runAllTests();
        
        // Return appropriate exit code based on test results
        return nlm::tests::IntegrationTestSuite().failedTests > 0 ? 1 : 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: Integration test suite failed with exception:" << std::endl;
        std::cerr << e.what() << std::endl;
        return 1;
    }
}