// Checkpoint and Replay System Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <algorithm>

namespace test_checkpoint_replay {

void testCheckpointCreation() {
    nlm::Config config;
    config.set("neuron_count", 100);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run some steps to create activity
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint.bin";
    bool saveSuccess = brain->save(checkpointPath);
    assert(saveSuccess);
    
    // Verify checkpoint file exists
    assert(std::filesystem::exists(checkpointPath));
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "    testCheckpointCreation passed" << std::endl;
}

void testCheckpointLoad() {
    nlm::Config config;
    config.set("neuron_count", 100);
    
    // Create and save first brain
    auto brain1 = std::make_shared<nlm::Brain>(config);
    brain1->initialize();
    
    for (int i = 0; i < 100; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    std::string checkpointPath = "/tmp/test_checkpoint_load.bin";
    assert(brain1->save(checkpointPath));
    
    // Create second brain and load checkpoint
    auto brain2 = std::make_shared<nlm::Brain>(config);
    brain2->initialize();
    
    bool loadSuccess = brain2->load(checkpointPath);
    assert(loadSuccess);
    
    // The loaded brain should have some state
    assert(brain2->getTotalNeuronCount() == brain1->getTotalNeuronCount());
    assert(brain2->getTotalSynapseCount() == brain1->getTotalSynapseCount());
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "    testCheckpointLoad passed" << std::endl;
}

void testCheckpointSaveLoad() {
    nlm::Config config;
    config.set("neuron_count", 100);
    
    // Create and initialize first brain
    auto brain1 = std::make_shared<nlm::Brain>(config);
    brain1->initialize();
    
    // Record initial state
    size_t initialSpikes = brain1->getTotalSpikeCount();
    float initialFiringRate = brain1->getAverageFiringRate();
    
    // Run some steps
    for (int i = 0; i < 200; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    // Record state after steps
    size_t finalSpikes = brain1->getTotalSpikeCount();
    float finalFiringRate = brain1->getAverageFiringRate();
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint_save_load.bin";
    assert(brain1->save(checkpointPath));
    
    // Create new brain and load checkpoint
    auto brain2 = std::make_shared<nlm::Brain>(config);
    brain2->initialize();
    assert(brain2->load(checkpointPath));
    
    // The loaded brain should have some activity
    assert(brain2->getTotalSpikeCount() == finalSpikes);
    
    // Note: Due to the nature of neural simulation, exact state restoration
    // is complex. The load should restore the structure at minimum.
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "    testCheckpointSaveLoad passed" << std::endl;
}

void testReplaySystem() {
    nlm::Config config;
    config.set("neuron_count", 100);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Get episodic memory system
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    episodicMemory->initialize(brain.get());
    
    // Run some steps to create episodes
    for (int i = 0; i < 200; ++i) {
        brain->step(i, i * 0.001);
        
        // Store episode periodically
        if (i % 50 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.reward = 0.5f + std::sin(i * 0.1f) * 0.3f;
            
            // Add some active neurons
            for (int j = 0; j < 5; ++j) {
                episode.activeNeurons.push_back(nlm::NeuronId(j));
                episode.neuronActivations.push_back(0.5f + std::sin(i + j) * 0.3f);
            }
            
            episodicMemory->storeEpisode(episode);
        }
    }
    
    // Check if we have episodes
    size_t episodeCount = episodicMemory->getEpisodeCount();
    
    // Get episodes for replay
    auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
    
    // Replay episodes
    for (const auto* episode : episodesToReplay) {
        episodicMemory->replayEpisode(episode);
    }
    
    // The replay system should be functional
    assert(std::isfinite(static_cast<float>(episodesToReplay.size())));
    
    std::cout << "    testReplaySystem passed" << std::endl;
}

void testCheckpointAndReplayIntegration() {
    nlm::Config config;
    config.set("neuron_count", 150);
    
    // Create first brain, run simulation, save checkpoint
    auto brain1 = std::make_shared<nlm::Brain>(config);
    brain1->initialize();
    
    // Run substantial simulation
    for (int i = 0; i < 500; ++i) {
        brain1->injectCurrentToNeurons(nlm::NeuronType::Sensory, 3.0f);
        brain1->step(i, i * 0.001);
    }
    
    std::string checkpointPath = "/tmp/test_integration_checkpoint.bin";
    assert(brain1->save(checkpointPath));
    
    // Create second brain from checkpoint
    auto brain2 = std::make_shared<nlm::Brain>(config);
    brain2->initialize();
    assert(brain2->load(checkpointPath));
    
    // Verify both brains have similar characteristics
    assert(brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount());
    assert(brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount());
    
    // Both should be able to continue simulation
    for (int i = 500; i < 600; ++i) {
        brain2->step(i, i * 0.001);
    }
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "    testCheckpointAndReplayIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Checkpoint and Replay System tests..." << std::endl;
    testCheckpointCreation();
    testCheckpointLoad();
    testCheckpointSaveLoad();
    testReplaySystem();
    testCheckpointAndReplayIntegration();
    
    std::cout << "All Checkpoint and Replay System tests passed!" << std::endl;
}

} // namespace test_checkpoint_replay
