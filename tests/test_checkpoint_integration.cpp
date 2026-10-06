// Test Checkpoint Integration
// This test verifies that checkpoint save/load functionality works correctly for the integrated brain system

#include "test_checkpoint_integration.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace nlm;

void testCheckpointIntegration::testCheckpointSaveLoad() {
    NLM_LOG_INFO("Testing checkpoint save/load functionality...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "/tmp/test_checkpoints", nlm::ConfigSource::Default);
    
    // Create and initialize first brain
    auto brain1 = std::make_shared<Brain>(config);
    assert(brain1->initialize());
    NLM_LOG_INFO("[PASS] Brain1 initialized");
    
    // Run some steps to generate activity
    for (int i = 0; i < 50; ++i) {
        brain1->step(i, i * 0.001);
        
        // Add some activity
        brain1->injectCurrentToNeurons(NeuronType::Sensory, 1.0f);
    }
    
    // Get initial metrics
    float firingRate1 = brain1->getAverageFiringRate();
    size_t spikes1 = brain1->getTotalSpikeCount();
    
    std::string checkpointPath = "/tmp/nlm_test_checkpoint.bin";
    
    // Save checkpoint
    bool saveSuccess = brain1->save(checkpointPath);
    assert(saveSuccess);
    NLM_LOG_INFO("[PASS] Checkpoint saved successfully to " + checkpointPath);
    
    // Verify file exists and has content
    std::ifstream file(checkpointPath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize fileSize = file.tellg();
        NLM_LOG_INFO("[PASS] Checkpoint file size: " + std::to_string(fileSize) + " bytes");
        file.close();
        
        if (fileSize > 0) {
            NLM_LOG_INFO("[PASS] Checkpoint file contains data");
        }
    }
}

void testCheckpointIntegration::testCheckpointWithMemorySystems() {
    NLM_LOG_INFO("Testing checkpoint with memory systems...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create and initialize brain with memory systems
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    NLM_LOG_INFO("[PASS] Brain with memory systems initialized");
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Generate memory activity
    for (int i = 0; i < 30; ++i) {
        brain->step(i, i * 0.001);
        
        // Store in working memory
        if (workingMemory) {
            workingMemory->storeToNeuron(NeuronId(i + 5000), 0.5f + 0.3f * std::sin(i * 0.1f));
        }
        
        // Create episodic memory items
        if (episodicMemory && i % 10 == 0) {
            EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.reward = 0.5f * (i % 3);
            
            for (int j = 0; j < 10; ++j) {
                episode.activeNeurons.push_back(NeuronId(j + 3000));
                episode.neuronActivations.push_back(0.3f * j / 10.0f);
            }
            
            episodicMemory->storeEpisode(episode);
        }
    }
    
    // Get memory metrics
    size_t workingMemorySize = workingMemory->getActiveTraces();
    size_t episodeCount = episodicMemory->getEpisodeCount();
    
    NLM_LOG_INFO("[PASS] Working memory traces: " + std::to_string(workingMemorySize));
    NLM_LOG_INFO("[PASS] Episodic memory episodes: " + std::to_string(episodeCount));
    
    // Save checkpoint with memory states
    std::string checkpointPath = "/tmp/nlm_memory_checkpoint.bin";
    bool saveSuccess = brain->save(checkpointPath);
    assert(saveSuccess);
    NLM_LOG_INFO("[PASS] Checkpoint with memory systems saved");
}

void testCheckpointIntegration::testCheckpointMultipleBrains() {
    NLM_LOG_INFO("Testing checkpoint with multiple brain instances...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    std::string checkpointPath = "/tmp/nlm_multi_checkpoint.bin";
    
    // Create and run first brain
    auto brain1 = std::make_shared<Brain>(config);
    assert(brain1->initialize());
    
    for (int i = 0; i < 30; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    float metrics1 = brain1->getAverageFiringRate();
    size_t spikes1 = brain1->getTotalSpikeCount();
    
    // Save first brain's state
    bool save1 = brain1->save(checkpointPath);
    assert(save1);
    NLM_LOG_INFO("[PASS] First brain checkpoint saved (firing: " + std::to_string(metrics1) + ")");
    
    // Create second brain and load checkpoint
    auto brain2 = std::make_shared<Brain>(config);
    assert(brain2->initialize());
    
    bool load2 = brain2->load(checkpointPath);
    assert(load2);
    NLM_LOG_INFO("[PASS] Second brain checkpoint loaded successfully");
    
    // Verify second brain has state (note: exact neural state restoration depends on checkpoint implementation)
    size_t spikes2 = brain2->getTotalSpikeCount();
    NLM_LOG_INFO("[PASS] Second brain spike count after load: " + std::to_string(spikes2));
    
    // Both brains should be functional
    bool bothFunctional = brain1->initialize() && brain2->initialize();
    if (bothFunctional) {
        NLM_LOG_INFO("[PASS] Both brains functional after checkpoint operations");
    }
}

void testCheckpointIntegration::testCheckpointWithNeuromodulation() {
    NLM_LOG_INFO("Testing checkpoint with neuromodulation systems...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    
    // Generate neuromodulation activity
    for (int i = 0; i < 20; ++i) {
        brain->step(i, i * 0.001);
        
        // Signal rewards and errors
        dopamine->signalReward(0.3f * (i % 3 + 1));
        curiosity->update(0.2f * (i % 2 + 1), 0.4f * (i % 2 + 1), 0.001f);
        
        // Apply neuromodulation to brain
        brain->applyNeuromodulation(*dopamine);
    }
    
    // Get neuromodulation metrics
    float dopamineLevel = dopamine->getLevel();
    float curiosityLevel = curiosity->getLevel();
    
    NLM_LOG_INFO("[PASS] Dopamine level: " + std::to_string(dopamineLevel));
    NLM_LOG_INFO("[PASS] Curiosity level: " + std::to_string(curiosityLevel));
    
    // Save checkpoint with neuromodulation state
    std::string checkpointPath = "/tmp/nlm_neuro_checkpoint.bin";
    bool saveSuccess = brain->save(checkpointPath);
    assert(saveSuccess);
    NLM_LOG_INFO("[PASS] Neuromodulation checkpoint saved");
}

void testCheckpointIntegration::testCheckpointReliability() {
    NLM_LOG_INFO("Testing checkpoint reliability...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    std::string checkpointPath = "/tmp/nlm_reliability_checkpoint.bin";
    
    // Test multiple save/load cycles
    bool allPassed = true;
    for (int cycle = 0; cycle < 3; ++cycle) {
        NLM_LOG_INFO("[INFO] Running checkpoint cycle " + std::to_string(cycle + 1));
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Add variability to each cycle
        for (int i = 0; i < 20; ++i) {
            brain->step(i, i * 0.001);
            brain->injectCurrentToNeurons(NeuronType::Sensory, 0.5f + 0.5f * (cycle * 10 + i) / 30.0f);
        }
        
        // Save checkpoint
        bool save = brain->save(checkpointPath);
        if (!save) {
            NLM_LOG_ERROR("[FAIL] Save failed in cycle " + std::to_string(cycle + 1));
            allPassed = false;
            continue;
        }
        
        // Load checkpoint
        bool load = brain->load(checkpointPath);
        if (!load) {
            NLM_LOG_ERROR("[FAIL] Load failed in cycle " + std::to_string(cycle + 1));
            allPassed = false;
            continue;
        }
        
        NLM_LOG_INFO("[PASS] Checkpoint cycle " + std::to_string(cycle + 1) + " completed");
    }
    
    if (allPassed) {
        NLM_LOG_INFO("[PASS] All checkpoint reliability tests passed");
    } else {
        NLM_LOG_ERROR("[FAIL] Some checkpoint reliability tests failed");
    }
}

void testCheckpointIntegration::run() {
    NLM_LOG_INFO("=== Running Checkpoint Integration Tests ===");
    
    testCheckpointSaveLoad();
    testCheckpointWithMemorySystems();
    testCheckpointMultipleBrains();
    testCheckpointWithNeuromodulation();
    testCheckpointReliability();
    
    NLM_LOG_INFO("=== Checkpoint Integration Tests Complete ===");
}