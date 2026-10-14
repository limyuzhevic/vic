// Brain Save/Load Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace test_brain_save_load {

void testBrainSaveLoad() {
    std::cout << "    testBrainSaveLoad - testing save/load functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create and initialize first brain
    nlm::Brain brain1(config);
    bool success = brain1.initialize();
    assert(success);
    
    // Run a few steps to create activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "test_checkpoint.ncl";
    bool saveSuccess = brain1.save(checkpointFile);
    assert(saveSuccess);
    
    // Verify file was created
    assert(std::filesystem::exists(checkpointFile));
    
    // Get brain state for comparison
    size_t originalNeuronCount = brain1.getTotalNeuronCount();
    size_t originalSynapseCount = brain1.getTotalSynapseCount();
    
    // Create a new brain (reset/empty)
    nlm::Brain brain2(config);
    bool initSuccess = brain2.initialize();
    assert(initSuccess);
    
    // Reset the new brain to ensure clean state
    brain2.reset();
    
    // Load saved state into second brain
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Verify that the loaded brain has the same neuron and synapse counts
    size_t loadedNeuronCount = brain2.getTotalNeuronCount();
    size_t loadedSynapseCount = brain2.getTotalSynapseCount();
    
    assert(loadedNeuronCount == originalNeuronCount);
    assert(loadedSynapseCount == originalSynapseCount);
    
    // Clean up
    if (std::filesystem::exists(checkpointFile)) {
        std::filesystem::remove(checkpointFile);
    }
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

void runAll() {
    testBrainSaveLoad();
}

} // namespace test_brain_save_load

int main() {
    test_brain_save_load::runAll();
    return 0;
}