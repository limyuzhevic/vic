// Comprehensive test file demonstrating the new NLM core improvements
// This file tests the enhanced error handling, configuration management, and testing framework

#include "EnhancedNLMCore.hpp"
#include "NLMTestFramework.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

// Test configuration management
void testEnhancedConfiguration() {
    std::cout << "Testing enhanced configuration management..." << std::endl;
    
    // Test 1: Basic configuration
    nlm::EnhancedConfigManager config;
    assert(config.set<size_t>("neuron_count", 1000));
    assert(config.get<size_t>("neuron_count", 0) == 1000);
    assert(config.set<float>("connection_probability", 0.1f));
    assert(config.get<float>("connection_probability", 0.0f) == 0.1f);
    assert(config.set<double>("simulation_timestep", 0.001));
    assert(config.get<double>("simulation_timestep", 0.0) == 0.001);

    // Test 2: Configuration validation
    auto validation = config.validate();
    assert(validation.valid);
    assert(validation.errors.empty());
    assert(validation.warnings.empty());

    // Test 3: Invalid configuration
    nlm::EnhancedConfigManager invalidConfig;
    invalidConfig.set<size_t>("neuron_count", 0); // Invalid: zero neurons
    validation = invalidConfig.validate();
    assert(!validation.valid);
    assert(!validation.errors.empty());

    // Test 4: File operations
    assert(config.saveToFile("test_config.txt"));
    nlm::EnhancedConfigManager loadedConfig;
    assert(loadedConfig.loadFromFile("test_config.txt"));
    assert(loadedConfig.get<size_t>("neuron_count", 0) == 1000);

    // Clean up
    std::remove("test_config.txt");

    std::cout << "✓ Enhanced configuration test passed" << std::endl;
}

// Test configuration presets
void testConfigurationPresets() {
    std::cout << "Testing configuration presets..." << std::endl;
    
    nlm::EnhancedConfigManager config;

    // Test MINIMAL preset
    assert(config.applyPreset(nlm::EnhancedConfigManager::Preset::MINIMAL));
    assert(config.get<size_t>("neuron_count", 0) == 100);
    assert(config.get<float>("connection_probability", 0.0f) == 0.05f);

    // Test BALANCED preset
    config.applyPreset(nlm::EnhancedConfigManager::Preset::BALANCED);
    assert(config.get<size_t>("neuron_count", 0) == 1000);
    assert(config.get<float>("connection_probability", 0.0f) == 0.1f);

    // Test PERFORMANCE preset
    config.applyPreset(nlm::EnhancedConfigManager::Preset::PERFORMANCE);
    assert(config.get<size_t>("neuron_count", 0) == 500);
    assert(config.get<float>("connection_probability", 0.0f) == 0.2f);

    std::cout << "✓ Configuration presets test passed" << std::endl;
}

// Test error handling
void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;

    try {
        throw nlm::ConfigurationException("Test configuration error");
    } catch (const nlm::ConfigurationException& e) {
        assert(std::string(e.what()).find("Test configuration error") != std::string::npos);
    }

    try {
        throw nlm::InitializationException("Test initialization error");
    } catch (const nlm::InitializationException& e) {
        assert(std::string(e.what()).find("Test initialization error") != std::string::npos);
    }

    try {
        throw nlm::MemoryException("Test memory error");
    } catch (const nlm::MemoryException& e) {
        assert(std::string(e.what()).find("Test memory error") != std::string::npos);
    }

    std::cout << "✓ Error handling test passed" << std::endl;
}

// Test basic brain builder
void testSimpleBrainBuilder() {
    std::cout << "Testing simple brain builder..." << std::endl;

    // Test 1: Default configuration
    nlm::SimpleConfig& config = nlm::SimpleConfig::instance();
    assert(config.getNeuronCount() == 1000);
    assert(config.getConnectionProbability() == 0.1f);

    // Test 2: Modified configuration
    config.setNeuronCount(500);
    config.setConnectionProbability(0.15f);
    assert(config.getNeuronCount() == 500);
    assert(config.getConnectionProbability() == 0.15f);

    // Test 3: Validation
    assert(config.validate());

    // Test 4: Invalid configuration
    config.setNeuronCount(0);
    assert(!config.validate());

    std::cout << "✓ Simple brain builder test passed" << std::endl;
}

// Test brain builder
void testBrainBuilder() {
    std::cout << "Testing brain builder..." << std::endl;

    // Test 1: Excitatory brain
    auto brain = nlm::SimpleBrainBuilder::createExcitatoryBrain(1000);
    assert(brain != nullptr);

    // Test 2: Balanced brain
    auto balancedBrain = nlm::SimpleBrainBuilder::createBalancedBrain(1500);
    assert(balancedBrain != nullptr);

    // Test 3: Learning brain
    auto learningBrain = nlm::SimpleBrainBuilder::createForLearning(2000);
    assert(learningBrain != nullptr);

    std::cout << "✓ Brain builder test passed" << std::endl;
}

// Test basic NLM functionality
void testBasicNLM() {
    std::cout << "Testing basic NLM functionality..." << std::endl;

    // Create a simple brain
    auto brain = nlm::SimpleBrainBuilder::createBalancedBrain(100);
    assert(brain != nullptr);

    // Initialize brain
    assert(brain->initialize());

    // Test basic operations
    size_t initialSpikes = brain->getTotalSpikeCount();
    brain->step(0);
    size_t finalSpikes = brain->getTotalSpikeCount();

    // Should have some activity
    assert(finalSpikes >= initialSpikes);

    // Test action production
    auto action = brain->produceAction();
    assert(action != nullptr);

    // Test reset
    brain->reset();
    assert(brain->getTotalSpikeCount() == 0);

    std::cout << "✓ Basic NLM test passed" << std::endl;
}

// Test test utilities
void testTestUtilities() {
    std::cout << "Testing test utilities..." << std::endl;

    // Test float comparison
    assert(nlm::test_utils::TestScenarios::compareFloat(1.0f, 1.000001f, 1e-5f));
    assert(!nlm::test_utils::TestScenarios::compareFloat(1.0f, 1.1f, 1e-5f));

    // Test double comparison
    assert(nlm::test_utils::TestScenarios::compareDouble(1.0, 1.000000001, 1e-9));
    assert(!nlm::test_utils::TestScenarios::TestScenarios::compareDouble(1.0, 1.1, 1e-9));

    std::cout << "✓ Test utilities test passed" << std::endl;
}

// Test test runner
void testTestRunner() {
    std::cout << "Testing test runner..." << std::endl;

    nlm::test_utils::SimpleTestRunner runner;

    // Test successful test
    auto result1 = runner.runTest("Success Test", []() { return true; });
    assert(result1.passed);
    assert(result1.message == "Test passed");

    // Test failed test
    auto result2 = runner.runTest("Failure Test", []() { return false; });
    assert(!result2.passed);
    assert(result2.message == "Test failed");

    // Test test with exception
    auto result3 = runner.runTest("Exception Test", []() { throw std::runtime_error("Test error"); });
    assert(!result3.passed);
    assert(result3.message.find("Exception") != std::string::npos);

    std::cout << "✓ Test runner test passed" << std::endl;
}

// Main test function
int main() {
    std::cout << "=== NLM Enhanced Core Test Suite ===" << std::endl;
    std::cout << "Testing the new NLM core improvements..." << std::endl << std::endl;

    try {
        // Run all tests
        testEnhancedConfiguration();
        testConfigurationPresets();
        testErrorHandling();
        testSimpleBrainBuilder();
        testBrainBuilder();
        testBasicNLM();
        testTestUtilities();
        testTestRunner();

        std::cout << std::endl;
        std::cout << "=== All Tests Passed ===" << std::endl;
        std::cout << "The NLM enhanced core is working correctly!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "=== Test Failed ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "=== Test Failed ===" << std::endl;
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}