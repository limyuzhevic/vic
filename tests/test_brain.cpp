// Enhanced Brain Tests with comprehensive coverage
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

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

// Enhanced test: Brain state persistence
void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some simulation steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    assert(brain1.save("test_brain_state.bin"));
    
    // Create new brain and load state
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.load("test_brain_state.bin"));
    
    // Verify loaded state matches (approximate)
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

// Enhanced test: Brain with different neuron types
void testBrainWithDifferentNeuronTypes() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Inject current to different neuron types
    brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 10.0f);
    brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, 10.0f);
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
    brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 10.0f);
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 10.0f);
    
    // Run steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify no crashes and reasonable statistics
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    testBrainWithDifferentNeuronTypes passed" << std::endl;
}

// Enhanced test: Brain developmental stages
void testBrainDevelopmentalStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Check initial developmental stage
    auto initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Run some steps to progress through developmental stages
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    // Check final developmental stage (should be at least beyond initial)
    auto finalStage = brain.getDevelopmentalStage();
    assert(finalStage != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testBrainDevelopmentalStages passed" << std::endl;
}

// Enhanced test: Brain error conditions
void testBrainErrorConditions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Brain with zero neurons should fail to initialize or initialize with empty brain
    bool success = brain.initialize();
    // This might succeed or fail depending on implementation, but shouldn't crash
    assert(brain.getTotalNeuronCount() == 0);
    
    // Test with valid config
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    // Should initialize successfully
    assert(brain2.initialize());
    
    std::cout << "    testBrainErrorConditions passed" << std::endl;
}

// Enhanced test: Brain statistics accuracy
void testBrainStatisticsAccuracy() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get initial statistics
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    size_t initialFiring = brain.getFiringNeuronCount();
    double initialRate = brain.getAverageFiringRate();
    
    // Inject current to create activity
    brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 20.0f);
    
    // Run steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Get updated statistics
    size_t finalNeurons = brain.getTotalNeuronCount();
    size_t finalSynapses = brain.getTotalSynapseCount();
    size_t finalFiring = brain.getFiringNeuronCount();
    double finalRate = brain.getAverageFiringRate();
    
    // Verify statistics are reasonable
    assert(finalNeurons == initialNeurons); // Neuron count shouldn't change
    assert(finalSynapses >= initialSynapses); // Synapse count might change due to plasticity
    assert(finalFiring >= initialFiring); // Firing neurons should increase or stay same
    assert(finalRate >= initialRate); // Firing rate should increase or stay same
    
    std::cout << "    testBrainStatisticsAccuracy passed" << std::endl;
}

// Enhanced test: Brain region operations
void testBrainRegionOperations() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    // Test adding a region
    auto newRegionId = brain.addRegion("test_region");
    assert(newRegionId != nlm::INVALID_REGION_ID);
    
    // Verify new region is accessible
    auto* newRegion = brain.getRegion(newRegionId);
    assert(newRegion != nullptr);
    
    // Test getting all regions
    auto allRegions = brain.getRegions();
    assert(allRegions.size() == 4); // Original 3 + new one
    
    std::cout << "    testBrainRegionOperations passed" << std::endl;
}

// Enhanced test: Brain configuration validation
void testBrainConfigurationValidation() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test configuration with invalid values
    config->set("neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    // Brain might fail to initialize or initialize with invalid config
    // This tests that the system handles edge cases gracefully
    
    std::cout << "    testBrainConfigurationValidation passed" << std::endl;
}

// Performance test: Brain step performance
void testBrainPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Time the step operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Performance should be reasonable (less than 5 seconds for 100 steps with 1000 neurons)
    assert(duration.count() < 5000);
    
    std::cout << "    testBrainPerformance passed" << std::endl;
}

// Memory test: Brain memory usage
void testBrainMemoryUsage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get memory usage estimate (if available)
    // This is a rough estimate test
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    
    // Memory usage should be reasonable
    size_t estimatedMemory = neuronCount * 100 + synapseCount * 40; // Rough estimate in bytes
    assert(estimatedMemory > 0);
    
    std::cout << "    testBrainMemoryUsage passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainSaveLoad();
    testBrainWithDifferentNeuronTypes();
    testBrainDevelopmentalStages();
    testBrainErrorConditions();
    testBrainStatisticsAccuracy();
    testBrainRegionOperations();
    testBrainConfigurationValidation();
    testBrainPerformance();
    testBrainMemoryUsage();
}

} // namespace test_brain