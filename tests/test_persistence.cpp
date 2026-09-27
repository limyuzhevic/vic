// Persistence Integration Tests for Phase 6 NLM Brain
// Tests save/load functionality with checkpoint system

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

namespace test_persistence {

void testBasicSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run some steps to create activity
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "./test_checkpoint.brain";
    bool saveResult = brain1.save(checkpointFile);
    assert(saveResult);
    
    // Create second brain and load saved state
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loadResult = brain2.load(checkpointFile);
    assert(loadResult);
    
    // Verify loaded state
    assert(brain2.getTotalNeuronCount() == 100);
    assert(brain2.getRegionCount() == 2);
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testBasicSaveLoad passed" << std::endl;
}

void testSaveLoadWithActivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run steps with activity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain1.step(step);
        
        // Create some memory
        if (step % 10 == 0) {
            auto* episodicMemory = brain1.getEpisodicMemory();
            if (episodicMemory) {
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.reward = 0.5f;
                for (size_t i = 0; i < 5; ++i) {
                    episode.activeNeurons.push_back(i + 1);
                    episode.neuronActivations.push_back(0.6f);
                }
                episodicMemory->storeEpisode(episode);
            }
        }
    }
    
    // Save with memory data
    std::string checkpointFile = "./test_checkpoint_activity.brain";
    bool saveResult = brain1.save(checkpointFile);
    assert(saveResult);
    
    // Load into second brain
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loadResult = brain2.load(checkpointFile);
    assert(loadResult);
    
    // Verify loaded brain has expected properties
    assert(brain2.getTotalNeuronCount() == 150);
    assert(brain2.getRegionCount() == 3);
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testSaveLoadWithActivity passed" << std::endl;
}

void testSaveLoadMultipleBrains() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create multiple brain instances with different states
    std::vector<std::string> checkpointFiles;
    
    for (int i = 0; i < 3; ++i) {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run different number of steps for each brain
        for (nlm::SimulationStep step = 0; step < (i + 1) * 10; ++step) {
            brain.step(step);
        }
        
        // Save to different files
        std::string checkpointFile = "./test_checkpoint_" + std::to_string(i) + ".brain";
        checkpointFiles.push_back(checkpointFile);
        
        bool saveResult = brain.save(checkpointFile);
        assert(saveResult);
    }
    
    // Load each checkpoint
    for (int i = 0; i < 3; ++i) {
        nlm::Brain brain(config);
        brain.initialize();
        
        bool loadResult = brain.load(checkpointFiles[i]);
        assert(loadResult);
        
        assert(brain.getTotalNeuronCount() == 120);
        assert(brain.getRegionCount() == 2);
    }
    
    // Cleanup
    for (const auto& file : checkpointFiles) {
        std::remove(file.c_str());
    }
    
    std::cout << "    testSaveLoadMultipleBrains passed" << std::endl;
}

void testSaveLoadWithIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Create complex state with all integrated systems
    for (nlm::SimulationStep step = 0; step < 80; ++step) {
        brain1.step(step);
        
        // Create memory
        auto* episodicMemory = brain1.getEpisodicMemory();
        if (episodicMemory && step % 15 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = 0.7f + (step % 3) * 0.1f;
            for (size_t i = 0; i < 8; ++i) {
                episode.activeNeurons.push_back(i + 1);
                episode.neuronActivations.push_back(0.4f + (i % 3) * 0.3f);
            }
            episodicMemory->storeEpisode(episode);
        }
        
        // Create associations
        auto* associativeMemory = brain1.getAssociativeMemory();
        if (associativeMemory && step % 25 == 0) {
            associativeMemory->storeAssociation(step % 10 + 1, step % 20 + 11, 0.8f);
        }
    }
    
    // Save complex state
    std::string checkpointFile = "./test_checkpoint_complex.brain";
    bool saveResult = brain1.save(checkpointFile);
    assert(saveResult);
    
    // Load into second brain
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loadResult = brain2.load(checkpointFile);
    assert(loadResult);
    
    // Verify loaded state
    assert(brain2.getTotalNeuronCount() == 200);
    assert(brain2.getRegionCount() == 2);
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testSaveLoadWithIntegration passed" << std::endl;
}

void testPersistenceCorruptionHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Try to load from non-existent file
    bool loadResult = brain.load("./non_existent_file.brain");
    assert(!loadResult); // Should fail
    
    // Save to a file, then corrupt it
    std::string checkpointFile = "./test_corrupted.brain";
    bool saveResult = brain.save(checkpointFile);
    assert(saveResult);
    
    // Corrupt the file
    std::ofstream corruptFile(checkpointFile, std::ios::trunc);
    corruptFile << "This is not a valid checkpoint file";
    corruptFile.close();
    
    // Try to load corrupted file
    bool loadCorrupted = brain.load(checkpointFile);
    assert(!loadCorrupted); // Should fail
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testPersistenceCorruptionHandling passed" << std::endl;
}

} // namespace test_persistence
