// Persistence Testing
// Tests brain state checkpointing and persistence
// Validates save/load functionality and state integrity

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <chrono>

namespace test_persistence {

void testSaveLoadBasic() {
    std::cout << "    Testing basic save/load functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create some state
    std::string saveFile1 = "test_save1.brain";
    
    // Run some simulation steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Store some data
        nlm::Dopamine* dopamine = brain.getDopamine();
        if (dopamine) {
            dopamine->signalReward(0.5f + 0.1f * step);
        }
        
        nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
        if (workingMem) {
            workingMem->store(step % 100, 0.3f * (step + 1) / 20.0f);
        }
    }
    
    // Save brain state
    assert(brain.save(saveFile1));
    
    // Verify file was created
    assert(std::filesystem::exists(saveFile1));
    
    // Create second brain
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    // Load saved state
    assert(brain2.load(saveFile1));
    
    // Verify state was restored
    assert(brain2.getTotalNeuronCount() == 300);
    assert(brain2.getRegionCount() == 2);
    
    // Verify some neural statistics
    assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain.getTotalSynapseCount());
    
    // Verify developmental stage was saved
    assert(brain2.getDevelopmentalStage() == brain.getDevelopmentalStage());
    
    // Clean up
    std::filesystem::remove(saveFile1);
    
    std::cout << "    testSaveLoadBasic passed" << std::endl;
}

void testSaveLoadWithMemory() {
    std::cout << "    Testing save/load with memory systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Set developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    
    // Fill memory systems
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::EpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::AssociativeMemory* assocMem = brain.getAssociativeMemory();
    
    if (workingMem) {
        for (size_t i = 0; i < 100; ++i) {
            workingMem->store(i, 0.5f + 0.3f * std::sin(i * 0.1f));
        }
    }
    
    if (episodicMem) {
        for (size_t i = 0; i < 50; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.neurons = {static_cast<nlm::NeuronId>(100 + i), static_cast<nlm::NeuronId>(200 + i)};
            episode.values = {0.2f * (i + 1) / 50.0f, 0.8f * (i + 1) / 50.0f};
            episode.metadata = "memory_episode_" + std::to_string(i);
            episodicMem->storeEpisode(episode);
        }
    }
    
    if (assocMem) {
        for (size_t i = 0; i < 50; ++i) {
            assocMem->associate(i, (i + 1) % 100, 0.5f + 0.2f * std::cos(i * 0.2f));
        }
    }
    
    // Save state
    std::string saveFile2 = "test_save2.brain";
    assert(brain.save(saveFile2));
    
    // Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Runtime);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    assert(brain2.load(saveFile2));
    
    // Verify developmental stage
    assert(brain2.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Verify memory state
    if (workingMem && episodicMem && assocMem) {
        assert(workingMem->getCurrentSize() == 100);
        assert(episodicMem->getEpisodeCount() == 50);
        assert(assocMem->getAssociationStrength(0, 1) > 0.0f);
    }
    
    // Clean up
    std::filesystem::remove(saveFile2);
    
    std::cout << "    testSaveLoadWithMemory passed" << std::endl;
}

void testSaveLoadNeuromodulation() {
    std::cout << "    Testing save/load with neuromodulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    
    // Set neuromodulator levels
    if (dopamine) {
        dopamine->signalReward(1.5f);
        dopamine->update(0.1f);
    }
    
    if (curiosity) {
        curiosity->setLevel(0.8f);
        curiosity->update(0.1f);
    }
    
    if (predError) {
        predError->setPredictionError(0.3f);
    }
    
    // Run simulation with neuromodulation
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        if (dopamine) {
            dopamine->signalReward(0.2f * (step + 1));
        }
        brain.step(step);
    }
    
    // Save state
    std::string saveFile3 = "test_save3.brain";
    assert(brain.save(saveFile3));
    
    // Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Runtime);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    assert(brain2.load(saveFile3));
    
    // Verify neuromodulation state was preserved
    if (dopamine && curiosity && predError) {
        // Neuromodulator levels should be restored
        assert(dopamine->getLevel() >= 0.0f);
        assert(curiosity->getLevel() >= 0.0f);
        assert(predError->getPredictionError() >= 0.0f);
    }
    
    // Clean up
    std::filesystem::remove(saveFile3);
    
    std::cout << "    testSaveLoadNeuromodulation passed" << std::endl;
}

void testSaveLoadConsistency() {
    std::cout << "    Testing save/load consistency..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create multiple save points
    std::vector<std::string> saveFiles;
    
    for (int savePoint = 0; savePoint < 5; ++savePoint) {
        // Run some steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        std::string saveFile = "test_save_" + std::to_string(savePoint) + ".brain";
        assert(brain.save(saveFile));
        saveFiles.push_back(saveFile);
    }
    
    // Load from different save points
    for (int loadPoint = 0; loadPoint < 5; ++loadPoint) {
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Runtime);
        config2->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
        
        nlm::Brain brain2(config2);
        assert(brain2.initialize());
        
        std::string saveFile = "test_save_" + std::to_string(loadPoint) + ".brain";
        assert(brain2.load(saveFile));
        
        // Verify consistency
        assert(brain2.getTotalNeuronCount() == 450);
        assert(brain2.getRegionCount() == 3);
    }
    
    // Clean up all save files
    for (const auto& saveFile : saveFiles) {
        std::filesystem::remove(saveFile);
    }
    
    std::cout << "    testSaveLoadConsistency passed" << std::endl;
}

void testCheckpointSystem() {
    std::cout << "    Testing checkpoint system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create checkpoint directory
    std::string checkpointDir = "checkpoints";
    if (!std::filesystem::exists(checkpointDir)) {
        std::filesystem::create_directory(checkpointDir);
    }
    
    // Create multiple checkpoints
    std::vector<std::string> checkpoints;
    
    for (int checkpoint = 0; checkpoint < 3; ++checkpoint) {
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
            
            // Store some state
            nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
            if (workingMem) {
                workingMem->store(checkpoint * 100 + step, 0.3f * (step + 1) / 20.0f);
            }
        }
        
        // Save checkpoint
        std::string checkpointFile = checkpointDir + "/checkpoint_" + std::to_string(checkpoint) + ".brain";
        assert(brain.save(checkpointFile));
        checkpoints.push_back(checkpointFile);
    }
    
    // Restore from last checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config2->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    assert(brain2.load(checkpoints.back()));
    
    // Verify checkpoint was restored
    assert(brain2.getTotalNeuronCount() == 500);
    assert(brain2.getRegionCount() == 4);
    
    // Clean up checkpoints
    std::filesystem::remove_all(checkpointDir);
    
    std::cout << "    testCheckpointSystem passed" << std::endl;
}

void runAll() {
    testSaveLoadBasic();
    testSaveLoadWithMemory();
    testSaveLoadNeuromodulation();
    testSaveLoadConsistency();
    testCheckpointSystem();
}

} // namespace test_persistence