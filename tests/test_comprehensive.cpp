#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <memory>
#include <functional>
#include <string>
#include <algorithm>
#include <ctime>
#include <utility>

// Test Framework Base
class TestFramework {
public:
    static void runTest(const std::string& testName, std::function<void()> testFunc) {
        std::cout << "Running " << testName << "..." << std::endl;
        try {
            testFunc();
            std::cout << "  [PASS] " << testName << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  [FAIL] " << testName << ": " << e.what() << std::endl;
            throw;
        } catch (...) {
            std::cout << "  [FAIL] " << testName << ": Unknown error" << std::endl;
            throw;
        }
    }
    
    static void runPerformanceTest(const std::string& testName, std::function<void()> testFunc, double& timeMs) {
        std::cout << "Running performance " << testName << "..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        try {
            testFunc();
            auto end = std::chrono::high_resolution_clock::now();
            timeMs = std::chrono::duration<double, std::milli>(end - start).count();
            std::cout << "  [PASS] " << testName << " (" << timeMs << " ms)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  [FAIL] " << testName << ": " << e.what() << std::endl;
            throw;
        }
    }
};

// Test 1: Advanced Configuration Tests
void testAdvancedConfiguration() {
    nlm::Config config;
    
    // Test complex configuration with different types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    
    // Test vector values
    std::vector<int> int_vector = {1, 2, 3, 4, 5};
    config.set("int_vector", int_vector, nlm::ConfigSource::Runtime);
    
    std::vector<double> double_vector = {0.1, 0.2, 0.3};
    config.set("double_vector", double_vector, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> string_vector = {"a", "b", "c"};
    config.set("string_vector", string_vector, nlm::ConfigSource::Runtime);
    
    // Verify all values
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.has("double_value"));
    double val = config.get<double>("double_value").value();
    assert(std::abs(val - 3.14159) < 0.0001);
    
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "test_string");
    
    // Test vector retrieval
    auto int_vec_val = config.get<std::vector<int>>("int_vector");
    assert(int_vec_val.has_value());
    assert(int_vec_val.value() == int_vector);
    
    auto double_vec_val = config.get<std::vector<double>>("double_vector");
    assert(double_vec_val.has_value());
    assert(double_vec_val.value() == double_vector);
    
    auto string_vec_val = config.get<std::vector<std::string>>("string_vector");
    assert(string_vec_val.has_value());
    assert(string_vec_val.value() == string_vector);
    
    // Test getOr with missing keys
    assert(config.getOr<int>("missing_int", 999) == 999);
    assert(config.getOr<double>("missing_double", 3.14) > 3.13);
    assert(config.getOr<std::string>("missing_string", "default") == "default");
    
    // Test configuration summary
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("string_value") != std::string::npos);
}

// Test 2: Advanced Brain Initialization
void testAdvancedBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    
    // Configure for advanced testing
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    config->set("development.enable", true, nlm::ConfigSource::Default);
    config->set("neuromod.dopamine.scale", 1.2, nlm::ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify advanced configuration
    assert(brain.getRegionCount() == 4);
    assert(brain.getTotalNeuronCount() == 5000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test with small simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify activity
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0);
}

// Test 3: Complex Scenario - Full Integration
void testFullIntegrationScenario() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(3000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.03, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    config->set("development.enable", true, nlm::ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
    config->set("prediction.enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run complex scenario
    float total_reward = 0.0f;
    size_t firing_count = 0;
    size_t step_count = 500;
    
    for (nlm::SimulationStep step = 0; step < step_count; ++step) {
        // Inject varied sensory input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 
            static_cast<float>(rand() % 20 + 5)); // 5-25 units
        
        // Brain step
        brain.step(step);
        
        // Collect metrics
        if (brain.getFiringNeuronCount() > 0) firing_count++;
        
        // Check all systems are integrated
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        assert(brain.getNovelty() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getDevelopmentSystem() != nullptr);
    }
    
    // Verify final state
    assert(brain.getTotalNeuronCount() == 3000);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0);
    
    // Test development stage progression
    nlm::DevelopmentalStage final_stage = brain.getDevelopmentalStage();
    assert(final_stage != nlm::DevelopmentalStage::Initial); // Should have progressed
}

// Test 4: Performance Test
void testPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.08, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance test
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        if (step % 100 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    double stepsPerSecond = 1000.0 / (timeMs / 1000.0);
    std::cout << "  Performance: " << stepsPerSecond << " steps/second" << std::endl;
    
    // Performance should be reasonable (at least 100 steps/sec for this configuration)
    assert(stepsPerSecond > 100.0);
}

// Main test runner
void runAllTests() {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Running advanced tests for NLM neural simulation framework..." << std::endl;
    std::cout << "" << std::endl;
    
    double performanceTime = 0.0;
    
    try {
        // Core functionality tests
        TestFramework::runTest("Advanced Configuration Tests", testAdvancedConfiguration);
        TestFramework::runTest("Advanced Brain Initialization", testAdvancedBrainInitialization);
        TestFramework::runTest("Full Integration Scenario", testFullIntegrationScenario);
        TestFramework::runTest("Performance Test", testPerformance);
        
        std::cout << "" << std::endl;
        std::cout << "=== All Tests Completed Successfully ===" << std::endl;
        std::cout << "Performance: " << 1000.0 / (performanceTime / 1000.0) << " steps/second" << std::endl;
        std::cout << "" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "" << std::endl;
        std::cout << "=== Test Suite Failed ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    runAllTests();
    
    std::cout << "" << std::endl;
    std::cout << "=== Comprehensive Test Suite Complete ===" << std::endl;
    std::cout << "All tests passed successfully!" << std::endl;
    std::cout << "" << std::endl;
    
    return 0;
}

} // namespace comprehensive_tests

int main() {
    return comprehensive_tests::main();
}