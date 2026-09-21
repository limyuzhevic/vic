#include "Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "performance/CheckpointManager.hpp"
#include <iostream>
#include <filesystem>

using namespace nlm;

Phase6IntegratedExperiment::Phase6IntegratedExperiment() : testCheckpointManager_(new SimpleCheckpointManager("test_checkpoints")) {
}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() {
    if (testCheckpointManager_) {
        delete testCheckpointManager_;
        testCheckpointManager_ = nullptr;
    }
}

bool Phase6IntegratedExperiment::testCheckpointing() {
    NLM_LOG_INFO("=== Testing Checkpoint Save/Load ===");
    
    // Create a minimal configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 2);
    cfg->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for checkpoint test");
        return false;
    }
    
    // Run brain for a few steps to have some activity
    for (uint64_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Test 1: Save checkpoint using CheckpointManager
    NLM_LOG_INFO("Test 1: Saving checkpoint with CheckpointManager...");
    bool saveSuccess = testCheckpointManager_->save("test_checkpoint.brain", "test_checkpoint_1");
    if (!saveSuccess) {
        NLM_LOG_ERROR("CheckpointManager save failed");
        return false;
    }
    NLM_LOG_INFO("CheckpointManager save successful");
    
    // Test 2: Save using Brain::save
    NLM_LOG_INFO("Test 2: Saving brain state using Brain::save...");
    bool brainSaveSuccess = brain->save("test_brain.bin");
    if (!brainSaveSuccess) {
        NLM_LOG_ERROR("Brain::save failed");
        return false;
    }
    NLM_LOG_INFO("Brain::save successful");
    
    // Test 3: Load using CheckpointManager
    NLM_LOG_INFO("Test 3: Loading brain state using CheckpointManager...");
    std::string loadedPath;
    bool loadSuccess = testCheckpointManager_->load("test_checkpoint_1", loadedPath);
    if (!loadSuccess) {
        NLM_LOG_ERROR("CheckpointManager load failed");
        return false;
    }
    NLM_LOG_INFO("CheckpointManager load successful, path: " + loadedPath);
    
    // Test 4: Load using Brain::load
    NLM_LOG_INFO("Test 4: Loading brain state using Brain::load...");
    auto brain2 = std::make_shared<Brain>(cfg);
    brain2->initialize();
    
    bool brainLoadSuccess = brain2->load("test_brain.bin");
    if (!brainLoadSuccess) {
        NLM_LOG_ERROR("Brain::load failed");
        return false;
    }
    NLM_LOG_INFO("Brain::load successful");
    
    // Test 5: Verify loaded state
    NLM_LOG_INFO("Test 5: Verifying loaded state...");
    NLM_LOG_INFO("Original brain has " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
    NLM_LOG_INFO("Loaded brain has " + std::to_string(brain2->getTotalNeuronCount()) + " neurons");
    
    NLM_LOG_INFO("Original brain has " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
    NLM_LOG_INFO("Loaded brain has " + std::to_string(brain2->getTotalSynapseCount()) + " synapses");
    
    // Test 6: List checkpoints
    NLM_LOG_INFO("Test 6: Listing available checkpoints...");
    auto checkpoints = testCheckpointManager_->listCheckpoints();
    NLM_LOG_INFO("Found " + std::to_string(checkpoints.size()) + " checkpoints");
    
    // Cleanup
    std::filesystem::remove("test_brain.bin");
    std::filesystem::remove_all("test_checkpoints");
    
    NLM_LOG_INFO("=== Checkpoint Test Completed Successfully ===");
    return true;
}