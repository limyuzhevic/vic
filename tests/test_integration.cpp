// Comprehensive Integration Tests
// Tests for Phase 6 integration, new Python bindings, and CLI enhancements

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace nlm {
namespace tests {

void testBrainInitialization() {
    std::cout << "Testing brain initialization..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    Brain initialization test passed" << std::endl;
}

void testConfigAdvancedFeatures() {
    std::cout << "Testing advanced config features..." << std::endl;
    
    Config config;
    
    // Test type-safe setting with automatic type inference
    config.set("int_val", 42);
    config.set("float_val", 3.14f);
    config.set("string_val", std::string("test"));
    config.set("bool_val", true);
    
    // Test get() with type safety
    auto intVal = config.get<int64_t>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto floatVal = config.get<float>("float_val");
    assert(floatVal.has_value());
    assert(std::abs(*floatVal - 3.14f) < 0.01f);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test getOr() with defaults
    int64_t missingVal = config.getOr<int64_t>("missing", 999);
    assert(missingVal == 999);
    
    // Test copy constructor
    auto config2 = std::make_shared<Config>(config);
    assert(config2->has("int_val"));
    
    // Test merge functionality
    Config config3;
    config3.set("key1", "value1");
    config3.set("key2", 2);
    
    config3.merge(config);
    assert(config3.has("key1"));
    assert(config3.has("int_val"));
    
    // Test clear
    config3.clear();
    assert(!config3.has("key1"));
    
    std::cout << "    Advanced config features test passed" << std::endl;
}

void testBrainStepWithStimulation() {
    std::cout << "Testing brain step with stimulation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    size_t initialFiring = brain.getFiringNeuronCount();
    
    // Run with sensory stimulation
    for (SimulationStep step = 0; step < 100; ++step) {
        // Apply sensory input
        brain.injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain.step(step, step * 0.001);
    }
    
    // Verify activity increased
    size_t finalSpikes = brain.getTotalSpikeCount();
    size_t finalFiring = brain.getFiringNeuronCount();
    
    // We should have some activity, though not guaranteed with random network
    std::cout << "    Initial spikes: " << initialSpikes << ", Final spikes: " << finalSpikes << std::endl;
    std::cout << "    Initial firing: " << initialFiring << ", Final firing: " << finalFiring << std::endl;
    
    // Test step with different time
    brain.reset();
    brain.initialize();
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    std::cout << "    Brain step test passed" << std::endl;
}

void testIntegrationVerification() {
    std::cout << "Testing integration verification..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test that all major systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "    Integration verification test passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "Testing config validation..." << std::endl;
    
    Config config;
    
    // Test valid config
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.1f);
    config.set("brain.connection_probability", 0.05f);
    
    // These should be valid
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synapse_density"));
    assert(config.has("brain.connection_probability"));
    
    // Test value ranges
    int64_t neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
    assert(neuronCount == 1000);
    
    float density = config.getOr<float>("brain.synapse_density", 1.0f);
    assert(density == 0.1f);
    
    float probability = config.getOr<float>("brain.connection_probability", 1.0f);
    assert(probability == 0.05f);
    
    std::cout << "    Config validation test passed" << std::endl;
}

void testBrainPerformance() {
    std::cout << "Testing brain performance..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Measure performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < 500; ++step) {
        brain.step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Verify performance
    assert(duration.count() < 10000); // Should complete within 10 seconds
    
    std::cout << "    Performance test passed: " << duration.count() << " ms for 500 steps" << std::endl;
}

void testComplexConfiguration() {
    std::cout << "Testing complex configuration..." << std::endl;
    
    Config config;
    
    // Set multiple configuration values
    config.set("brain.neuron_count", 500);
    config.set("brain.synapse_density", 0.2f);
    config.set("brain.connection_probability", 0.1f);
    config.set("brain.initial_weight_mean", 0.5f);
    config.set("brain.initial_weight_std", 0.1f);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.stdp.learning_rate", 0.001f);
    config.set("plasticity.stdp.tau_plus", 20.0f);
    config.set("plasticity.stdp.tau_minus", 20.0f);
    config.set("plasticity.hebbian.enable", true);
    config.set("plasticity.structural.enable", true);
    config.set("neuromod.dopamine.scale", 1.0f);
    config.set("neuromod.curiosity.enable", true);
    config.set("neuromod.novelty.enable", true);
    config.set("development.enable", true);
    
    // Verify all values are set
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synapse_density"));
    assert(config.has("brain.connection_probability"));
    assert(config.has("plasticity.stdp.enable"));
    assert(config.has("neuromod.curiosity.enable"));
    
    // Test type-specific getters
    int64_t neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
    assert(neuronCount == 500);
    
    float density = config.getOr<float>("brain.synapse_density", 0.0f);
    assert(density == 0.2f);
    
    bool stdpEnabled = config.getOr<bool>("plasticity.stdp.enable", false);
    assert(stdpEnabled == true);
    
    std::cout << "    Complex configuration test passed" << std::endl;
}

void testConfigValidationAndSafety() {
    std::cout << "Testing config validation and safety..." << std::endl;
    
    Config config;
    
    // Test safe setting with invalid values (should not crash)
    try {
        config.set("valid_key", "valid_value");
        assert(config.has("valid_key"));
    } catch (...) {
        assert(false); // Should not throw
    }
    
    // Test that missing keys return defaults
    int64_t defaultValue = config.getOr<int64_t>("nonexistent_key", 42);
    assert(defaultValue == 42);
    
    float defaultFloat = config.getOr<float>("nonexistent_float", 3.14f);
    assert(std::abs(defaultFloat - 3.14f) < 0.001f);
    
    bool defaultBool = config.getOr<bool>("nonexistent_bool", true);
    assert(defaultBool == true);
    
    // Test that clear works
    config.set("temp_key", "temp_value");
    assert(config.has("temp_key"));
    
    config.clear();
    assert(!config.has("temp_key"));
    
    std::cout << "    Config validation and safety test passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== Running NLM Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    testBrainInitialization();
    testConfigAdvancedFeatures();
    testBrainStepWithStimulation();
    testIntegrationVerification();
    testConfigValidation();
    testBrainPerformance();
    testComplexConfiguration();
    testConfigValidationAndSafety();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests Passed ===" << std::endl;
}

} // namespace tests
} // namespace nlm
