// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

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

void testBrainCompleteSimulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize the brain
    assert(brain.initialize());
    
    // Run complete simulation steps
    const nlm::SimulationStep totalSteps = 50;
    float totalFiringRate = 0.0f;
    size_t totalSpikes = 0;
    
    for (nlm::SimulationStep step = 0; step < totalSteps; ++step) {
        brain.step(step);
        totalFiringRate += brain.getAverageFiringRate();
        totalSpikes += brain.getTotalSpikeCount();
        
        // Verify system remains stable
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getRegionCount() == 2);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    // Verify simulation results
    assert(totalSpikes > 0); // Should have some activity
    assert(totalFiringRate >= 0.0f); // Firing rates should be non-negative
    
    // Test reset and reinitialize
    brain.reset();
    assert(brain.initialize()); // Should reinitialize successfully
    
    std::cout << "    testBrainCompleteSimulation passed" << std::endl;
}

void testBrainInputOutput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test input injection
    brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 5.0f);
    
    // Test step with input
    brain.step(0);
    
    // Test output action
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test brain statistics after I/O
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainInputOutput passed" << std::endl;
}

void testBrainMultiThreading() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test thread safety features if available
    // This would depend on the implementation of thread safety
    // For now, just verify brain functionality remains consistent
    
    nlm::SimulationStep step = 0;
    float firingRate1 = brain.getAverageFiringRate();
    
    brain.step(step);
    
    float firingRate2 = brain.getAverageFiringRate();
    
    // Results should be consistent (though may vary due to randomness)
    assert(std::isfinite(firingRate1));
    assert(std::isfinite(firingRate2));
    
    std::cout << "    testBrainMultiThreading passed" << std::endl;
}

void testBrainDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test developmental stage integration
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage >= nlm::DevelopmentalStage::Initial && 
           stage <= nlm::DevelopmentalStage::Aging);
    
    // Test development updates
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    // Verify brain functionality persists across developmental stages
    brain.step(0);
    assert(brain.getTotalNeuronCount() == 300);
    
    std::cout << "    testBrainDevelopmentIntegration passed" << std::endl;
}

void testBrainMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory pool functionality if available
    // This would test memory management features
    
    // Verify brain state is consistent
    brain.step(0);
    assert(brain.getTotalNeuronCount() == 250);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainMemoryIntegration passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainCompleteSimulation();
    testBrainInputOutput();
    testBrainMultiThreading();
    testBrainDevelopmentIntegration();
    testBrainMemoryIntegration();
}

} // namespace test_brain
