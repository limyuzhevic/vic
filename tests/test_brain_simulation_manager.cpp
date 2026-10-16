// BrainSimulationManager Comprehensive Tests
// Tests step execution, checkpoint/load, timing

#include "brain/BrainSimulationManager.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <filesystem>

namespace test_brain_simulation_manager {

void testBrainSimulationManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::BrainSimulationManager simulation(config);
    nlm::Brain brain(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    std::cout << "    testBrainSimulationManagerCreation passed" << std::endl;
}

void testSimulationStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::BrainSimulationManager simulation(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    // Get current step (should be 0 after initialization)
    nlm::SimulationStep initialStep = simulation.getCurrentStep();
    assert(initialStep == 0);
    
    nlm::Timestamp initialTime = simulation.getCurrentTime();
    assert(initialTime == 0.0);
    
    // Start simulation
    simulation.start();
    assert(simulation.isRunning());
    
    // Execute simulation step
    simulation.step(0, 0.0);
    
    // Step should have incremented
    nlm::SimulationStep updatedStep = simulation.getCurrentStep();
    assert(updatedStep >= initialStep);
    
    nlm::Timestamp updatedTime = simulation.getCurrentTime();
    assert(updatedTime >= initialTime);
    
    std::cout << "    testSimulationStep passed" << std::endl;
}

void testSimulationPauseStop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::BrainSimulationManager simulation(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    simulation.start();
    assert(simulation.isRunning());
    
    // Pause simulation
    simulation.pause();
    assert(!simulation.isRunning());
    
    // Stop simulation
    simulation.stop();
    assert(!simulation.isRunning());
    
    std::cout << "    testSimulationPauseStop passed" << std::endl;
}

void testCheckpointSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::BrainSimulationManager simulation(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    // Configure checkpoint manager
    simulation.configureCheckpoint("/tmp/checkpoints", 10, 5, true);
    
    // Execute some simulation steps
    for (int i = 0; i < 3; ++i) {
        simulation.step(i, i * 0.1);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint.bin";
    bool saveSuccess = brain.save(checkpointPath);
    assert(saveSuccess);
    
    // Save simulation state
    bool simSaveSuccess = simulation.save("/tmp/simulation_state.bin");
    assert(simSaveSuccess);
    
    // Create a new brain and load
    nlm::Brain brain2(config);
    nlm::BrainSimulationManager simulation2(config);
    
    assert(brain2.initialize());
    assert(simulation2.initialize());
    
    // Load brain state
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess);
    
    // Load simulation state
    bool simLoadSuccess = simulation2.load("/tmp/simulation_state.bin");
    assert(simLoadSuccess);
    
    std::cout << "    testCheckpointSaveLoad passed" << std::endl;
}

void testSimulationStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::BrainSimulationManager simulation(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    // Get initial statistics
    size_t initialSpikeCount = simulation.getTotalSpikeCount();
    size_t initialActiveCount = simulation.getActiveNeuronCount();
    float initialFiringRate = simulation.getAverageFiringRate();
    
    assert(initialSpikeCount >= 0);
    assert(initialActiveCount >= 0);
    assert(initialFiringRate >= 0.0f);
    
    // Execute simulation steps
    for (int i = 0; i < 5; ++i) {
        simulation.step(i, i * 0.1);
    }
    
    // Get updated statistics
    size_t updatedSpikeCount = simulation.getTotalSpikeCount();
    size_t updatedActiveCount = simulation.getActiveNeuronCount();
    float updatedFiringRate = simulation.getAverageFiringRate();
    
    assert(updatedSpikeCount >= initialSpikeCount);
    assert(updatedActiveCount >= 0);
    assert(updatedFiringRate >= 0.0f);
    
    std::cout << "    testSimulationStatistics passed" << std::endl;
}

void testSimulationReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::BrainSimulationManager simulation(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    // Execute some steps
    for (int i = 0; i < 3; ++i) {
        simulation.step(i, i * 0.1);
    }
    
    // Reset simulation
    simulation.reset();
    
    // Reset statistics
    simulation.resetStatistics();
    
    std::cout << "    testSimulationReset passed" << std::endl;
}

void testSimulationConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::BrainSimulationManager simulation(config);
    
    assert(brain.initialize());
    assert(simulation.initialize());
    
    // Test configuration access
    std::shared_ptr<const nlm::Config> cfg = simulation.getConfig();
    assert(cfg != nullptr);
    
    // Test status logging
    simulation.logStatus();
    
    std::cout << "    testSimulationConfiguration passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainSimulationManager tests..." << std::endl;
    
    testBrainSimulationManagerCreation();
    testSimulationStep();
    testSimulationPauseStop();
    testCheckpointSaveLoad();
    testSimulationStatistics();
    testSimulationReset();
    testSimulationConfiguration();
    
    std::cout << "\nAll BrainSimulationManager tests passed!" << std::endl;
}

} // namespace test_brain_simulation_manager
