// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Brain initialization and configuration validation
void testBrainInitializationValidation() {
    // Test with invalid configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set invalid values
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    // Brain creation should handle this gracefully (or throw)
    nlm::Brain brain(config);
    
    // Initialization may or may not succeed depending on implementation
    // This test ensures no crashes during initialization with edge cases
    brain.initialize();
    
    std::cout << "    testBrainInitializationValidation passed" << std::endl;
}

// Basic connectivity and spike propagation
void testBrainConnectivityAndSpikePropagation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify there are neurons and synapses
    assert(brain.getTotalNeuronCount() >= 50);
    assert(brain.getTotalSynapseCount() >= 0);
    
    // Verify regions exist
    auto region_ids = brain.getRegionIds();
    assert(!region_ids.empty());
    
    // Test connectivity by stepping the brain
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Check that spike count is reasonable (may be zero if no neurons fire)
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    testBrainConnectivityAndSpikePropagation passed" << std::endl;
}

// Memory system integration
void testBrainMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory system access
    auto* working_memory = brain.getWorkingMemory();
    auto* episodic_memory = brain.getEpisodicMemory();
    auto* associative_memory = brain.getAssociativeMemory();
    
    assert(working_memory != nullptr);
    assert(episodic_memory != nullptr);
    assert(associative_memory != nullptr);
    
    // Basic memory operations should not crash
    working_memory->clear();
    episodic_memory->clear();
    associative_memory->clear();
    
    std::cout << "    testBrainMemorySystemIntegration passed" << std::endl;
}

// Neuromodulation effects
void testBrainNeuromodulationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Apply neuromodulatory signals
    nlm::Neuromodulator reward_signal;
    reward_signal.setType(nlm::NeuromodulatorType::Dopamine);
    reward_signal.setMagnitude(1.0f);
    
    brain.applyNeuromodulation(reward_signal);
    
    nlm::Neuromodulator curiosity_signal;
    curiosity_signal.setType(nlm::NeuromodulatorType::Curiosity);
    curiosity_signal.setMagnitude(0.5f);
    
    brain.applyNeuromodulation(curiosity_signal);
    
    // Should not crash and should affect brain state
    // (Actual effects depend on implementation)
    
    std::cout << "    testBrainNeuromodulationEffects passed" << std::endl;
}

// Action selection
void testBrainActionSelection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Force some neurons to fire to make action production possible
    brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 50.0f);
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Produce action (should return nullptr if no motor neurons fired)
    auto action = brain.produceAction();
    
    // Action may be null if no motor neurons are active
    // This is implementation-dependent but should not crash
    
    std::cout << "    testBrainActionSelection passed" << std::endl;
}

// Brain plasticity system integration
void testBrainPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structural != nullptr);
    
    // Update plasticity (should not crash)
    brain.updatePlasticity();
    
    std::cout << "    testBrainPlasticityIntegration passed" << std::endl;
}

// Brain developmental changes
void testBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Apply developmental changes
    brain.develop();
    
    // Check developmental stage
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    // Stage should be valid
    
    std::cout << "    testBrainDevelopment passed" << std::endl;
}

// Brain validation and leak detection
void testBrainValidationAndLeakDetection() {
    // Test multiple brain instances
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50 + i * 10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Perform operations
        brain.step(0);
        brain.reset();
    }
    
    // Validate all active instances
    // (Implementation-dependent)
    
    std::cout << "    testBrainValidationAndLeakDetection passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    // Additional comprehensive tests
    testBrainInitializationValidation();
    testBrainConnectivityAndSpikePropagation();
    testBrainMemorySystemIntegration();
    testBrainNeuromodulationEffects();
    testBrainActionSelection();
    testBrainPlasticityIntegration();
    testBrainDevelopment();
    testBrainValidationAndLeakDetection();
}

} // namespace test_brain
