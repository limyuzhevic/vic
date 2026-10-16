// Phase 6 Checkpoint System Tests
// Test save/load functionality

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

using namespace nlm;

namespace test_phase6_checkpoint {

void testCheckpointSaveLoad() {
    std::cout << "  Testing Checkpoint Save/Load..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testCheckpointing();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void testCheckpointFunctionality() {
    std::cout << "  Testing Checkpoint Functionality..." << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain1 = std::make_shared<Brain>(cfg);
    brain1->initialize();
    
    // Record initial state
    size_t initialNeurons = brain1->getTotalNeuronCount();
    size_t initialSynapses = brain1->getTotalSynapseCount();
    size_t initialSpikes = brain1->getTotalSpikeCount();
    
    // Run brain for a while
    for (int i = 0; i < 50; ++i) {
        brain1->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain1->step(i, i * 0.001);
    }
    
    // Record state after running
    size_t afterNeurons = brain1->getTotalNeuronCount();
    size_t afterSynapses = brain1->getTotalSynapseCount();
    size_t afterSpikes = brain1->getTotalSpikeCount();
    
    // Create checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint_phase6.bin";
    bool saveSuccess = brain1->save(checkpointPath);
    assert(saveSuccess);
    
    // Create new brain and load checkpoint
    auto brain2 = std::make_shared<Brain>(cfg);
    brain2->initialize();
    
    bool loadSuccess = brain2->load(checkpointPath);
    assert(loadSuccess);
    
    // Verify loaded brain has expected properties
    assert(brain2->getTotalNeuronCount() == afterNeurons);
    assert(brain2->getTotalSynapseCount() == afterSynapses);
    
    std::cout << "    PASSED" << std::endl;
}

void testCheckpointPersistence() {
    std::cout << "  Testing Checkpoint Persistence..." << std::endl;
    
    // Create multiple checkpoints
    std::vector<std::string> checkpointPaths;
    
    for (int i = 0; i < 3; ++i) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        // Run brain
        for (int j = 0; j < 20; ++j) {
            brain->step(j, j * 0.001);
        }
        
        // Save checkpoint
        std::string path = "/tmp/test_checkpoint_" + std::to_string(i) + ".bin";
        bool success = brain->save(path);
        assert(success);
        
        checkpointPaths.push_back(path);
    }
    
    // Load checkpoints in reverse order
    for (int i = checkpointPaths.size() - 1; i >= 0; --i) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        bool success = brain->load(checkpointPaths[i]);
        assert(success);
        
        // Clean up
        std::filesystem::remove(checkpointPaths[i]);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Checkpoint System Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testCheckpointSaveLoad();
    testCheckpointFunctionality();
    testCheckpointPersistence();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Checkpoint System Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_checkpoint