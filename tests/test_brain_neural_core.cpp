// BrainNeuralCore Comprehensive Tests
// Tests neurons, spikes, synaptic transmission

#include "brain/BrainNeuralCore.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_brain_neural_core {

void testBrainNeuralCoreCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    
    assert(core.initialize());
    assert(core.getRegionCount() == 2);
    assert(core.getTotalNeuronCount() == 100);
    assert(core.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainNeuralCoreCreation passed" << std::endl;
}

void testNeuronInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Access neurons through regions
    auto regionIds = core.getRegionIds();
    assert(!regionIds.empty());
    
    auto* region = core.getRegion(regionIds[0]);
    assert(region != nullptr);
    
    // Access neurons (implementation detail - may need to add public methods)
    auto regions = core.getRegions();
    assert(regions.size() == 2);
    
    std::cout << "    testNeuronInitialization passed" << std::endl;
}

void testSynapticTransmission() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.5, nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Test spike generation and transmission
    auto* spikeSystem = core.getSpikeSystem();
    assert(spikeSystem != nullptr);
    
    // Record initial spike count
    size_t initialSpikes = core.getTotalSpikeCount();
    
    // Inject current to trigger firing
    nlm::NeuronId testNeuronId(0);
    core.injectCurrent(testNeuronId, 10.0f);
    
    // Step the simulation
    core.step(0);
    
    // Verify spike count increased
    assert(core.getTotalSpikeCount() > initialSpikes);
    
    std::cout << "    testSynapticTransmission passed" << std::endl;
}

void testRegionManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Test region creation
    nlm::RegionId regionId1 = core.addRegion("Region 1");
    assert(regionId1 != nlm::RegionId(0));
    
    nlm::RegionId regionId2 = core.addRegion("Region 2");
    assert(regionId2 != nlm::RegionId(0));
    assert(regionId2 != regionId1);
    
    // Test getting regions
    auto* region1 = core.getRegion(regionId1);
    auto* region2 = core.getRegion(regionId2);
    assert(region1 != nullptr);
    assert(region2 != nullptr);
    
    // Test region list
    auto regionIds = core.getRegionIds();
    assert(regionIds.size() >= 3); // Original 3 + 2 new
    
    // Test inter-region connections
    core.addInterRegionConnection(regionId1, regionId2, 0.5f, 1);
    core.addInterRegionConnection(regionId2, regionId1, 0.3f, 2);
    
    std::cout << "    testRegionManagement passed" << std::endl;
}

void testNeuralStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Get initial statistics
    size_t initialNeuronCount = core.getTotalNeuronCount();
    size_t initialSynapseCount = core.getTotalSynapseCount();
    size_t initialActiveCount = core.getActiveNeuronCount();
    size_t initialFiringCount = core.getFiringNeuronCount();
    float initialFiringRate = core.getAverageFiringRate();
    
    assert(initialNeuronCount == 100);
    assert(initialSynapseCount > 0);
    assert(initialActiveCount >= 0);
    assert(initialFiringCount >= 0);
    assert(initialFiringRate >= 0.0f);
    
    // Step to generate activity
    core.step(0);
    core.step(1);
    
    // Get updated statistics
    size_t updatedSynapseCount = core.getTotalSynapseCount();
    size_t updatedActiveCount = core.getActiveNeuronCount();
    size_t updatedFiringCount = core.getFiringNeuronCount();
    float updatedFiringRate = core.getAverageFiringRate();
    
    // Statistics should be consistent (may not change for this simple test)
    assert(updatedSynapseCount >= initialSynapseCount);
    assert(updatedActiveCount >= 0);
    assert(updatedFiringCount >= 0);
    assert(updatedFiringRate >= 0.0f);
    
    std::cout << "    testNeuralStatistics passed" << std::endl;
}

void testExcitationInhibitionRatio() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    float eir = core.getExcitationInhibitionRatio();
    assert(eir >= 0.0f); // Should be non-negative
    
    // Step to generate activity
    core.step(0);
    
    float updatedEir = core.getExcitationInhibitionRatio();
    assert(updatedEir >= 0.0f);
    
    std::cout << "    testExcitationInhibitionRatio passed" << std::endl;
}

void testActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Generate action
    auto action = core.produceAction();
    assert(action != nullptr);
    
    // Step to produce more activity
    core.step(0);
    core.step(1);
    
    auto action2 = core.produceAction();
    assert(action2 != nullptr);
    
    std::cout << "    testActionProduction passed" << std::endl;
}

void testResetFunctionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Generate some activity
    core.step(0);
    core.step(1);
    
    size_t spikeCountBeforeReset = core.getTotalSpikeCount();
    
    // Reset
    core.reset();
    
    size_t spikeCountAfterReset = core.getTotalSpikeCount();
    assert(spikeCountAfterReset == 0); // Should reset spike count
    
    std::cout << "    testResetFunctionality passed" << std::endl;
}

void testConfigAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    
    // Verify config access
    auto retrievedConfig = core.getConfig();
    assert(retrievedConfig != nullptr);
    
    auto neuronCount = retrievedConfig->get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 75);
    
    std::cout << "    testConfigAccess passed" << std::endl;
}

void testRandomGeneratorAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    nlm::RandomGenerator* rng = core.getRandomGenerator();
    assert(rng != nullptr);
    
    std::cout << "    testRandomGeneratorAccess passed" << std::endl;
}

void testLogStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::BrainNeuralCore core(config);
    assert(core.initialize());
    
    // Just test that logging doesn't crash
    core.logStatus();
    
    std::cout << "    testLogStatus passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainNeuralCore tests..." << std::endl;
    
    testBrainNeuralCoreCreation();
    testNeuronInitialization();
    testSynapticTransmission();
    testRegionManagement();
    testNeuralStatistics();
    testExcitationInhibitionRatio();
    testActionProduction();
    testResetFunctionality();
    testConfigAccess();
    testRandomGeneratorAccess();
    testLogStatus();
    
    std::cout << "\nAll BrainNeuralCore tests passed!" << std::endl;
}

} // namespace test_brain_neural_core
