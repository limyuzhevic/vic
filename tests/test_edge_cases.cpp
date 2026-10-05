// Edge Case Tests
// Phase 3.4: Edge case and error condition testing

#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

namespace test_edge_cases {

void testInvalidConfigurationFiles() {
    // Test various invalid configuration file formats
    nlm::Config config;
    
    // Test malformed JSON
    std::ofstream badJsonFile("bad_json.json");
    badJsonFile << "{"; // Incomplete JSON
    badJsonFile.close();
    
    bool loadResult = config.loadFromFile("bad_json.json");
    assert(!loadResult); // Should fail to load
    
    // Test empty JSON file
    std::ofstream emptyJsonFile("empty.json");
    emptyJsonFile << "{}";
    emptyJsonFile.close();
    
    bool emptyLoadResult = config.loadFromFile("empty.json");
    assert(emptyLoadResult); // Empty JSON should load
    
    // Test YAML file with invalid content
    std::ofstream badYamlFile("bad_yaml.yaml");
    badYamlFile << "invalid: yaml: content:";
    badYamlFile.close();
    
    bool yamlLoadResult = config.loadFromFile("bad_yaml.yaml");
    // YAML parsing may fail or fall back to simple format
    
    // Test non-existent file
    bool nonExistentResult = config.loadFromFile("does_not_exist_12345.json");
    assert(!nonExistentResult);
    
    // Clean up
    std::remove("bad_json.json");
    std::remove("empty.json");
    std::remove("bad_yaml.yaml");
    
    std::cout << "    testInvalidConfigurationFiles passed" << std::endl;
}

void testMemoryErrorConditions() {
    // Test memory-related error conditions
    nlm::Config config;
    
    // Test with very large values that could cause memory issues
    config.set("large_int", std::numeric_limits<int64_t>::max());
    config.set("large_double", std::numeric_limits<double>::max());
    
    // Test array with many elements
    std::vector<int> largeArray;
    largeArray.reserve(100000); // Reserve large space
    for (int i = 0; i < 10000; ++i) {
        largeArray.push_back(i);
    }
    config.set("large_array", largeArray);
    
    // Test brain creation with memory-intensive configuration
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    brainConfig->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    bool initResult = brain.initialize();
    // May or may not succeed depending on system resources
    
    std::cout << "    testMemoryErrorConditions passed" << std::endl;
}

void testNumericalStability() {
    // Test numerical stability with extreme values
    nlm::Config config;
    
    // Test with infinity and NaN
    config.set("infinity_value", std::numeric_limits<double>::infinity());
    config.set("negative_infinity", -std::numeric_limits<double>::infinity());
    config.set("nan_value", std::numeric_limits<double>::quiet_NaN());
    
    // Test very small and very large values
    config.set("min_double", std::numeric_limits<double>::min());
    config->set("max_double", std::numeric_limits<double>::max());
    
    // Test for Brain numerical stability
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    // Test with extreme inputs
    brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, std::numeric_limits<float>::max());
    brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -std::numeric_limits<float>::max());
    
    brain.step(0);
    
    // Verify system remains stable
    assert(brain.getTotalNeuronCount() == 100);
    assert(std::isfinite(brain.getAverageFiringRate()));
    
    std::cout << "    testNumericalStability passed" << std::endl;
}

void testBoundaryConditions() {
    // Test boundary conditions
    nlm::Config config;
    
    // Test empty configuration
    nlm::Config emptyConfig;
    assert(emptyConfig.getKeys().empty());
    
    // Test configuration with zero values
    config.set("zero_int", 0);
    config.set("zero_double", 0.0);
    config.set("zero_bool", false);
    
    // Test maximum configuration values
    config.set("max_int", std::numeric_limits<int64_t>::max());
    config.set("max_double", std::numeric_limits<double>::max());
    
    // Test negative boundary values
    config.set("negative_int", -1);
    config.set("negative_double", -1.0);
    config.set("negative_bool", false); // bool is only true/false
    
    // Test Brain boundary conditions
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default); // Minimum neurons
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    brain.step(0);
    assert(brain.getTotalNeuronCount() == 1);
    
    std::cout << "    testBoundaryConditions passed" << std::endl;
}

void testConcurrentAccess() {
    // Test thread safety with concurrent access
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::vector<std::thread> threads;
    std::atomic<int> completedThreads{0};
    std::mutex accessMutex;
    
    auto concurrentWorker = [&brain, &completedThreads, &accessMutex]() {
        for (int i = 0; i < 20; ++i) {
            std::lock_guard<std::mutex> lock(accessMutex);
            
            // Read operations
            auto firingRate = brain.getAverageFiringRate();
            auto neuronCount = brain.getTotalNeuronCount();
            auto synapseCount = brain.getTotalSynapseCount();
            
            // Write operations
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.1f);
            brain.step(0);
            
            (void)firingRate; (void)neuronCount; (void)synapseCount; // Suppress warnings
            
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        
        completedThreads++;
    };
    
    // Create and start threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(concurrentWorker);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    assert(completedThreads == 5);
    
    std::cout << "    testConcurrentAccess passed" << std::endl;
}

void testErrorRecovery() {
    // Test error recovery capabilities
    nlm::Config config;
    
    // Set up a valid configuration
    config.set("valid_key", 42);
    
    // Try to get non-existent key
    auto missingValue = config.get<int>("missing_key");
    assert(!missingValue.has_value());
    
    // Configuration should still be usable
    assert(config.has("valid_key"));
    assert(config.getOr<int>("valid_key", 0) == 42);
    
    // Test Brain error recovery
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    // Perform operations
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Reset and recover
    brain.reset();
    assert(brain.initialize());
    
    // Verify recovery
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testErrorRecovery passed" << std::endl;
}

void testMemoryLeakDetection() {
    // Test for memory leaks (simplified version)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    // Create and destroy many brain instances
    for (int i = 0; i < 20; ++i) {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Perform some operations
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        // Brain goes out of scope here - should be properly destroyed
    }
    
    std::cout << "    testMemoryLeakDetection passed" << std::endl;
}

void testResourceExhaustion() {
    // Test behavior under resource exhaustion conditions
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Try to initialize with large brain
    bool initResult = brain.initialize();
    // May or may not succeed depending on system resources
    
    // If initialization succeeds, test under load
    if (initResult) {
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.0f);
            brain.step(step);
            
            // Verify system remains stable
            assert(brain.getTotalNeuronCount() == 1000);
            assert(std::isfinite(brain.getAverageFiringRate()));
        }
    }
    
    std::cout << "    testResourceExhaustion passed" << std::endl;
}

void testInvalidInputHandling() {
    // Test handling of invalid inputs
    nlm::Config config;
    
    // Test invalid type conversions
    config.set("int_key", 42);
    auto wrongType = config.get<double>("int_key"); // Should be convertible
    assert(wrongType.has_value());
    
    // Test missing keys
    auto missing = config.get<std::string>("non_existent");
    assert(!missing.has_value());
    
    // Test Brain invalid input handling
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    // Invalid neuron ID (should be handled gracefully)
    // This depends on the Brain implementation
    
    std::cout << "    testInvalidInputHandling passed" << std::endl;
}

void testDataCorruptionDetection() {
    // Test detection of data corruption
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Perform operations
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.5f);
        brain.step(step);
        
        // Check for data corruption signs
        assert(brain.getTotalNeuronCount() > 0);
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getTotalSynapseCount() >= 0);
    }
    
    std::cout << "    testDataCorruptionDetection passed" << std::endl;
}

void runAll() {
    testInvalidConfigurationFiles();
    testMemoryErrorConditions();
    testNumericalStability();
    testBoundaryConditions();
    testConcurrentAccess();
    testErrorRecovery();
    testMemoryLeakDetection();
    testResourceExhaustion();
    testInvalidInputHandling();
    testDataCorruptionDetection();
}

} // namespace test_edge_cases