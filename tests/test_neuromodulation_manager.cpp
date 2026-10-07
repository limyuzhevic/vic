// NeuromodulationManager Tests
#include "neuromodulation/NeuromodulationManager.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

void testNeuromodulationManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuromodulationManager manager;
    
    std::cout << "    testNeuromodulationManagerCreation passed" << std::endl;
}

void testNeuromodulationManagerInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuromodulationManager manager;
    
    manager.initialize(&brain);
    assert(manager.isInitialized());
    
    std::cout << "    testNeuromodulationManagerInitialization passed" << std::endl;
}

void testNeuromodulationManagerUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuromodulationManager manager;
    manager.initialize(&brain);
    
    // Run a few updates
    for (int i = 0; i < 5; ++i) {
        manager.update(0.001f);
    }
    
    // Check that manager has recorded updates
    auto stats = manager.getStats();
    assert(stats.totalSteps == 5);
    
    std::cout << "    testNeuromodulationManagerUpdate passed" << std::endl;
}

void testNeuromodulationManagerStats() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuromodulationManager manager;
    manager.initialize(&brain);
    
    // Initial stats should be zero
    auto initialStats = manager.getStats();
    assert(initialStats.totalSteps == 0);
    assert(initialStats.plasticityUpdateCount == 0);
    assert(initialStats.stateUpdateCount == 0);
    
    // Perform updates
    manager.update(0.001f);
    auto updatedStats = manager.getStats();
    assert(updatedStats.totalSteps == 1);
    assert(updatedStats.stateUpdateCount == 1);
    
    std::cout << "    testNeuromodulationManagerStats passed" << std::endl;
}

void testNeuromodulationManagerResetStats() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuromodulationManager manager;
    manager.initialize(&brain);
    
    // Perform updates to get stats
    manager.update(0.001f);
    manager.update(0.001f);
    
    // Reset stats
    manager.resetStats();
    
    auto stats = manager.getStats();
    assert(stats.totalSteps == 0);
    assert(stats.plasticityUpdateCount == 0);
    assert(stats.stateUpdateCount == 0);
    
    std::cout << "    testNeuromodulationManagerResetStats passed" << std::endl;
}

void testNeuromodulationManagerWithoutBrain() {
    nlm::NeuromodulationManager manager;
    
    // Should not crash when updating without initialization
    manager.update(0.001f);
    
    // Stats should remain zero
    auto stats = manager.getStats();
    assert(stats.totalSteps == 0);
    assert(stats.plasticityUpdateCount == 0);
    assert(stats.stateUpdateCount == 0);
    
    std::cout << "    testNeuromodulationManagerWithoutBrain passed" << std::endl;
}

} // namespace test_neuromodulation