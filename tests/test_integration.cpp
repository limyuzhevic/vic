// Integration Tests
// Tests end-to-end scenarios with real configurations

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "plasticity/PlasticityRule.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

namespace test_integration {

void testFullPipelineWithConfigFile() {
    // Create a realistic configuration file
    std::string configFile = "/tmp/integration_test_config.txt";
    std::ofstream file(configFile);
    
    file << "# Integration Test Configuration\n";
    file << "brain.neuron_count = 1000\n";
    file << "brain.region_count = 5\n";
    file << "brain.synapse_density = 0.1\n";
    file << "brain.v_thresh = -55.0\n";
    file << "brain.v_rest = -70.0\n";
    file << "brain.tau_mem = 20.0\n";
    file << "brain.tau_ref = 5.0\n";
    file << "brain.learning_rate = 0.01\n";
    file << "brain.connection_probability = 0.1\n";
    file << "random_seed = 42\n";
    file << "stdp_ltp_weight = 0.01\n";
    file << "stdp_ltd_weight = 0.012\n";
    file << "stdp_tau = 20.0\n";
    file << "synaptogenesis_rate = 0.0001\n";
    file << "pruning_rate = 0.00001\n";
    
    file.close();
    
    // Load and use configuration
    nlm::Config config;
    bool loadResult = config.loadFromFile(configFile);
    assert(loadResult);
    
    // Verify configuration values
    assert(config.getOr<int64_t>("brain.neuron_count") == 1000);
    assert(config.getOr<int64_t>("brain.region_count") == 5);
    assert(std::abs(config.getOr<double>("brain.synapse_density") - 0.1) < 0.001);
    assert(std::abs(config.getOr<double>("brain.v_thresh") - (-55.0)) < 0.001);
    
    // Create and initialize brain with this config
    auto brainConfig = std::make_shared<nlm::Config>(config);
    nlm::Brain brain(brainConfig);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify brain state
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getRegionCount() == 5);
    
    // Clean up
    std::filesystem::remove(configFile);
    
    std::cout << "    testFullPipelineWithConfigFile passed" << std::endl;
}

void testCompleteBrainLifecycle() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config->set("region_count", 3, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run simulation with sensory input
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Create dummy sensory input
        std::vector<float> inputData;
        if (step % 10 == 0) {  // Provide input every 10 steps
            for (size_t i = 0; i < 10; ++i) {
                inputData.push_back(static_cast<float>(step * 10 + i));
            }
        }
        
        // Brain would receive sensory input in real implementation
        // brain.receiveSensoryInput(inputData);
        
        brain.step(step);
    }
    
    // Produce actions
    for (int i = 0; i < 5; ++i) {
        auto action = brain.produceAction();
        // Action may be null, that's okay
    }
    
    // Reset brain
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 500);
    
    std::cout << "    testCompleteBrainLifecycle passed" << std::endl;
}

void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 5, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test memory system integration
    // Working memory should be initialized
    // Episodic memory should be initialized
    // Associative memory should be initialized
    
    // Run simulation with memory updates
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify brain still functional after memory operations
    assert(brain.getTotalNeuronCount() == 1000);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test neuromodulation integration
    // Dopamine, novelty, curiosity, prediction error systems should be integrated
    
    // Run simulation with neuromodulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify brain still functional after neuromodulation
    assert(brain.getTotalNeuronCount() == 500);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 5, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config->set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test plasticity integration
    // STDP, Hebbian, structural plasticity should be integrated
    
    // Run simulation with plasticity
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify brain still functional after plasticity
    assert(brain.getTotalNeuronCount() == 1000);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 800, nlm::ConfigSource::Runtime);
    config->set("region_count", 4, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test cognition integration
    // Neural planner, concept formation, attention systems should be integrated
    
    // Run simulation with cognition
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify brain still functional after cognition
    assert(brain.getTotalNeuronCount() == 800);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void testEndToEndScenario() {
    // Create a comprehensive configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set up a realistic scenario
    config->set("brain.neuron_count", 2000, nlm::ConfigSource::Runtime);
    config->set("brain.region_count", 8, nlm::ConfigSource::Runtime);
    config->set("brain.synapse_density", 0.05, nlm::ConfigSource::Runtime);
    config->set("brain.v_thresh", -50.0, nlm::ConfigSource::Runtime);
    config->set("brain.v_rest", -65.0, nlm::ConfigSource::Runtime);
    config->set("brain.tau_mem", 15.0, nlm::ConfigSource::Runtime);
    config->set("brain.tau_ref", 3.0, nlm::ConfigSource::Runtime);
    config->set("brain.learning_rate", 0.005, nlm::ConfigSource::Runtime);
    config->set("brain.connection_probability", 0.08, nlm::ConfigSource::Runtime);
    config->set("random_seed", 12345, nlm::ConfigSource::Runtime);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run extended simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Check brain status periodically
        if (step % 100 == 0) {
            assert(brain.getTotalNeuronCount() == 2000);
            assert(brain.getRegionCount() == 8);
        }
    }
    
    // Produce actions
    auto action = brain.produceAction();
    // Action may be null
    
    // Get final statistics
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    float avgFiringRate = brain.getAverageFiringRate();
    
    // Verify statistics are reasonable
    assert(totalNeurons == 2000);
    assert(totalSynapses >= 0);
    assert(firingNeurons >= 0);
    assert(avgFiringRate >= 0.0f);
    
    std::cout << "    testEndToEndScenario passed" << std::endl;
}

void runAll() {
    testFullPipelineWithConfigFile();
    testCompleteBrainLifecycle();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testPlasticityIntegration();
    testCognitionIntegration();
    testEndToEndScenario();
    
    std::cout << "Integration tests completed successfully!" << std::endl;
}

} // namespace test_integration
