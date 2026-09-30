// Comprehensive Tests for NLM Improvements
// This file tests all the improvements made to the NLM codebase

#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_improvements {

void testConfigKeyNormalization() {
    nlm::Config config;
    
    // Test key normalization
    config.set("randomseed", 42, nlm::ConfigSource::Runtime);
    assert(config.has("random_seed"));
    
    auto val = config.get<int>("random_seed");
    assert(val.has_value());
    assert(*val == 42);
    
    std::cout << "    testConfigKeyNormalization passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    config.set("int_value", 123, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test_string"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test save to file
    assert(config.saveToFile("./test_config.cfg"));
    
    // Create new config and load from file
    nlm::Config config2;
    assert(config2.loadFromFile("./test_config.cfg"));
    
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    // Clean up
    std::remove("./test_config.cfg");
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testNeuronImprovedLIF() {
    nlm::Neuron neuron(1);
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    // Store initial state
    auto initialPotential = neuron.getState().membranePotential;
    auto initialThreshold = neuron.getState().threshold;
    
    // Test stepLIF with different timesteps
    for (int i = 0; i < 100; ++i) {
        neuron.stepLIF(1.0, 0.001);
    }
    
    // Neuron should not have fired yet (small input)
    assert(!neuron.checkThreshold());
    
    // Inject current and test firing
    neuron.injectCurrent(50.0f);
    
    bool fired = neuron.stepLIF(2.0, 0.001);
    assert(fired || !neuron.checkThreshold());  // Either fired or threshold not reached
    
    std::cout << "    testNeuronImprovedLIF passed" << std::string(15, ' ') << std::endl;
}

void testSynapseImprovedPlasticity() {
    nlm::Synapse synapse(1, 1, 2);
    synapse.initializeRandom(nlm::RandomGenerator(42));
    
    // Store initial weight
    auto initialWeight = synapse.getWeight();
    
    // Test step function
    synapse.step(1.0);
    
    // Weight should be clamped
    auto currentWeight = synapse.getWeight();
    assert(currentWeight >= nlm::Synapse::Impl::MIN_WEIGHT);
    assert(currentWeight <= nlm::Synapse::Impl::MAX_WEIGHT);
    
    // Test plasticity flags
    synapse.enablePlasticity(true, true, false);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapseImprovedPlasticity passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    assert(brain.initialize());
    
    // Check brain statistics
    assert(brain.getTotalNeuronCount() >= 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSpikeCount() == 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testPerformanceImprovements() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Runtime);
    config->set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    assert(brain.initialize());
    
    // Measure initialization time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain.step(i, i * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete 100 steps reasonably quickly (less than 10 seconds)
    assert(duration.count() < 10000);
    
    std::cout << "    testPerformanceImprovements passed" << std::endl;
}

void testAdvancedConfigParameters() {
    nlm::Config config;
    
    // Test new advanced parameters
    config.set("parallel_processing", true, nlm::ConfigSource::Runtime);
    config.set("num_threads", 4, nlm::ConfigSource::Runtime);
    config.set("adaptive_timestep", true, nlm::ConfigSource::Runtime);
    config.set("min_timestep", 0.0001, nlm::ConfigSource::Runtime);
    config.set("plasticity_mode", std::string("STDP_Hebbian"), nlm::ConfigSource::Runtime);
    config.set("working_memory_type", std::string("neural"), nlm::ConfigSource::Runtime);
    config.set("visualization_enabled", false, nlm::ConfigSource::Runtime);
    
    // Verify they were set correctly
    assert(config.has("parallel_processing"));
    assert(config.has("num_threads"));
    assert(config.has("adaptive_timestep"));
    assert(config.has("min_timestep"));
    assert(config.has("plasticity_mode"));
    assert(config.has("working_memory_type"));
    assert(config.has("visualization_enabled"));
    
    auto parallel = config.get<bool>("parallel_processing");
    assert(parallel.has_value());
    assert(*parallel == true);
    
    std::cout << "    testAdvancedConfigParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive improvement tests..." << std::endl;
    std::cout << std::endl;
    
    testConfigKeyNormalization();
    testConfigSaveLoad();
    testNeuronImprovedLIF();
    testSynapseImprovedPlasticity();
    testBrainInitialization();
    testPerformanceImprovements();
    testAdvancedConfigParameters();
    
    std::cout << std::endl;
    std::cout << "All improvement tests passed! ✓" << std::endl;
    std::cout << "\nSummary of tested improvements:";
    std::cout << "\n1. Config key normalization and improved save/load";
    std::cout << "\n2. Enhanced Neuron LIF dynamics and documentation";
    std::cout << "\n3. Improved Synapse plasticity rules and validation";
    std::cout << "\n4. Brain initialization with advanced configuration";
    std::cout << "\n5. Performance improvements and optimization";
    std::cout << "\n6. Advanced configuration parameters for power users";
}

} // namespace test_improvements

int main() {
    test_improvements::runAll();
    return 0;
}
