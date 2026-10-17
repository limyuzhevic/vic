// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

// NEW COMPREHENSIVE TESTS

void testBrainEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test very small brain
    assert(brain.getTotalNeuronCount() == 10);
    assert(brain.getTotalSynapseCount() >= 0);
    
    // Test stepping
    for (int step = 0; step < 5; ++step) {
        brain.step(step);
        assert(brain.getTotalNeuronCount() == 10);  // Should remain constant
    }
    
    // Test reset with small brain
    brain.reset();
    assert(brain.getTotalNeuronCount() == 10);
    
    // Test re-initialization after reset
    assert(brain.initialize());
    
    std::cout << "    testBrainEdgeCases passed" << std::endl;
}

void testBrainStatisticsConsistency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get initial statistics
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run some steps
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Statistics should remain consistent
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() == initialSynapses);
    
    // Total spikes should have increased or stayed the same
    assert(brain.getTotalSpikeCount() >= initialSpikes);
    
    // Firing neuron count should be non-negative
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Average firing rate should be non-negative
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatisticsConsistency passed" << std::endl;
}

void testBrainRegionConsistency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all regions
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    // Verify each region is valid
    size_t totalRegionNeurons = 0;
    size_t totalRegionSynapses = 0;
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        
        totalRegionNeurons += region->getTotalNeuronCount();
        totalRegionSynapses += region->getSynapseCount();
    }
    
    // Verify totals match brain totals
    assert(totalRegionNeurons == brain.getTotalNeuronCount());
    assert(totalRegionSynapses == brain.getTotalSynapseCount());
    
    std::cout << "    testBrainRegionConsistency passed" << std::endl;
}

void testBrainConfigValidation() {
    // Test with invalid/edge case configurations
    
    // Very small configuration
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    assert(brain1.initialize());  // Should succeed with minimal config
    
    // Higher connection probability
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config2->set("connection_probability", 0.5, nlm::ConfigSource::Default);  // High probability
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    assert(brain2.getTotalSynapseCount() > 0);  // Should have many connections
    
    std::cout << "    testBrainConfigValidation passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainEdgeCases();
    testBrainStatisticsConsistency();
    testBrainRegionConsistency();
    testBrainConfigValidation();
}

} // namespace test_brain
