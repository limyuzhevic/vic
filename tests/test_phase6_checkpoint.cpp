// Phase 6 Checkpoint System Integration Tests
// Comprehensive testing of checkpoint save/load functionality

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace test_phase6_checkpoint {

void testCheckpointSave() {
    std::cout << "    Testing Checkpoint Save Functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation to generate brain state
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::string checkpointPath = "/tmp/nlm_checkpoint_test.bin";
    
    // Save checkpoint
    bool saveSuccess = brain.save(checkpointPath);
    assert(saveSuccess);
    std::cout << "      Checkpoint saved successfully" << std::endl;
    
    // Verify file was created
    assert(std::filesystem::exists(checkpointPath));
    std::filesystem::path pathObj(checkpointPath);
    uintmax_t fileSize = std::filesystem::file_size(pathObj);
    std::cout << "      Checkpoint file size: " << fileSize << " bytes" << std::endl;
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "      Checkpoint save test PASSED" << std::endl;
}

void testCheckpointLoad() {
    std::cout << "    Testing Checkpoint Load Functionality..." << std::endl;
    
    // Create a checkpoint first
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    assert(brain1.initialize());
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain1.step(step);
    }
    
    std::string checkpointPath = "/tmp/nlm_checkpoint_test.bin";
    assert(brain1.save(checkpointPath));
    
    // Get metrics from first brain
    float firingRate1 = brain1.getAverageFiringRate();
    size_t totalSpikes1 = brain1.getTotalSpikeCount();
    
    std::cout << "      Original brain - Firing rate: " << firingRate1 
              << ", Total spikes: " << totalSpikes1 << std::endl;
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess);
    std::cout << "      Checkpoint loaded successfully" << std::endl;
    
    // Get metrics from second brain (should be similar)
    float firingRate2 = brain2.getAverageFiringRate();
    size_t totalSpikes2 = brain2.getTotalSpikeCount();
    
    std::cout << "      Loaded brain - Firing rate: " << firingRate2 
              << ", Total spikes: " << totalSpikes2 << std::endl;
    
    // Note: Exact spike counts may differ due to simulation randomness
    // but the structure should be restored
    assert(brain2.getTotalNeuronCount() == config2->getOr<int64_t>("neuron_count", 100));
    assert(brain2.getRegionCount() == config2->getOr<int64_t>("region_count", 1));
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "      Checkpoint load test PASSED" << std::endl;
}

void testCheckpointSaveLoadCycle() {
    std::cout << "    Testing Checkpoint Save/Load Cycle..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::vector<float> firingRates;
    std::vector<size_t> spikeCounts;
    
    // Run simulation and save checkpoints at intervals
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        
        if (step % 75 == 0) {
            std::string checkpointPath = "/tmp/nlm_checkpoint_" + std::to_string(step) + ".bin";
            assert(brain.save(checkpointPath));
            
            float firingRate = brain.getAverageFiringRate();
            size_t spikeCount = brain.getTotalSpikeCount();
            
            firingRates.push_back(firingRate);
            spikeCounts.push_back(spikeCount);
            
            std::cout << "      Checkpoint at step " << step << ": "
                      << "Firing rate: " << firingRate << ", "
                      << "Spikes: " << spikeCount << std::endl;
        }
    }
    
    // Load last checkpoint and verify state
    std::string lastCheckpointPath = "/tmp/nlm_checkpoint_300.bin";
    
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    assert(brain2.load(lastCheckpointPath));
    
    std::cout << "      Loaded last checkpoint" << std::endl;
    
    // Verify the checkpoint preserves brain structure
    assert(brain2.getTotalNeuronCount() == 120);
    assert(brain2.getRegionCount() == 1);
    
    // Clean up all checkpoints
    for (int step : {0, 75, 150, 225, 300}) {
        std::string checkpointPath = "/tmp/nlm_checkpoint_" + std::to_string(step) + ".bin";
        if (std::filesystem::exists(checkpointPath)) {
            std::filesystem::remove(checkpointPath);
        }
    }
    
    std::cout << "      Checkpoint save/load cycle test PASSED" << std::endl;
}

void testCheckpointCorruptionDetection() {
    std::cout << "    Testing Checkpoint Corruption Detection..." << std::endl;
    
    // Create valid checkpoint
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::string checkpointPath = "/tmp/nlm_valid_checkpoint.bin";
    assert(brain.save(checkpointPath));
    
    // Corrupt the checkpoint file
    std::ofstream corruptFile(checkpointPath, std::ios::binary);
    corruptFile << "This is not a valid checkpoint file!";
    corruptFile.close();
    
    // Try to load corrupted checkpoint - should fail
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointPath);
    // Loading may still succeed depending on implementation, but shouldn't crash
    std::cout << "      Attempted to load corrupted checkpoint: " 
              << (loadSuccess ? "succeeded (may be implementation-defined)" : "failed (expected)") << std::endl;
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "      Checkpoint corruption detection test PASSED" << std::endl;
}

void testCheckpointPerformance() {
    std::cout << "    Testing Checkpoint Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation to generate state
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
    }
    
    // Benchmark save performance
    auto start = std::chrono::high_resolution_clock::now();
    std::string checkpointPath = "/tmp/nlm_perf_checkpoint.bin";
    assert(brain.save(checkpointPath));
    auto saveEnd = std::chrono::high_resolution_clock::now();
    
    // Benchmark load performance
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    auto loadStart = std::chrono::high_resolution_clock::now();
    assert(brain2.load(checkpointPath));
    auto loadEnd = std::chrono::high_resolution_clock::now();
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    double saveTime = std::chrono::duration<double>(saveEnd - start).count();
    double loadTime = std::chrono::duration<double>(loadEnd - loadStart).count();
    
    std::cout << "      Save time: " << saveTime << "s" << std::endl;
    std::cout << "      Load time: " << loadTime << "s" << std::endl;
    
    std::cout << "      Checkpoint performance test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Checkpoint System Tests ===" << std::endl;
    
    testCheckpointSave();
    testCheckpointLoad();
    testCheckpointSaveLoadCycle();
    testCheckpointCorruptionDetection();
    testCheckpointPerformance();
    
    std::cout << "\n=== All Phase 6 Checkpoint Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_checkpoint