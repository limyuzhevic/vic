// System-Level Integration Tests for NLM
// Tests checkpoint save/load, memory replay, consolidation, and complete brain loop

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace test_system_integration {

void testCheckpointSaveLoad() {
    std::cout << "    Testing checkpoint save/load..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("checkpoint_path", "./test_checkpoint.nlm", nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run simulation to generate state
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain1.step(step);
        
        // Generate some activity in memory systems
        brain1.getWorkingMemory()->updateWorkingMemory("test_data");
        brain1.getEpisodicMemory()->storeExperience("test_experience");
    }
    
    // Save checkpoint
    std::string checkpointFile = "test_checkpoint.nlm";
    bool saveSuccess = brain1.save(checkpointFile);
    assert(saveSuccess);
    
    // Verify checkpoint exists
    assert(std::filesystem::exists(checkpointFile));
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Verify loaded brain has expected state
    assert(brain2.getRegionCount() == 2);
    assert(brain2.getTotalNeuronCount() == 100);
    
    // Cleanup
    if (std::filesystem::exists(checkpointFile)) {
        std::filesystem::remove(checkpointFile);
    }
    
    std::cout << "    testCheckpointSaveLoad passed" << std::endl;
}

void testMemoryReplayConsolidation() {
    std::cout << "    Testing memory replay and consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* workingMemory = brain.getWorkingMemory();
    
    // Store multiple experiences
    std::vector<size_t> episodeIds;
    for (int i = 0; i < 10; ++i) {
        std::string experience = "experience_" + std::to_string(i);
        size_t episodeId = episodicMemory->storeExperience(experience);
        episodeIds.push_back(episodeId);
        
        // Update working memory
        workingMemory->updateWorkingMemory(experience);
    }
    
    // Trigger replay phase
    for (size_t steps = 0; steps < 150; ++steps) {
        brain.step(steps);
        
        // Simulate replay during rest phase
        if (steps % 100 == 0) {
            episodicMemory->triggerReplay();
        }
    }
    
    // Verify consolidation occurred
    size_t consolidatedCount = episodicMemory->getConsolidatedCount();
    assert(consolidatedCount > 0);
    
    std::cout << "    testMemoryReplayConsolidation passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage changes
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Juvenile);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Juvenile);
    
    // Run through developmental phases
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Development should affect plasticity
        brain.develop();
    }
    
    // Verify development integration
    assert(developmentSystem->getMaturationProgress() > 0.0f);
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void testCompleteBrainLoop() {
    std::cout << "    Testing complete brain loop integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("prediction_enabled", true, nlm::ConfigSource::Default);
    config->set("memory_consolidation_interval", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run complete brain loop
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // 1. Apply neuromodulation
        nlm::Neuromodulator dopamineSignal(nlm::Dopamine::Create(0.5f, 0.1f));
        brain.applyNeuromodulation(dopamineSignal);
        
        // 2. Process sensory input (simulate)
        // (In real scenario, this would come from environment)
        
        // 3. Step brain (neural dynamics, plasticity, etc.)
        brain.step(step);
        
        // 4. Apply developmental changes
        brain.develop();
        
        // 5. Update working memory
        brain.getWorkingMemory()->updateWorkingMemory("step_data");
        
        // 6. Generate action
        auto action = brain.produceAction();
        if (action) {
            // Process action consequences
        }
        
        // 7. Update prediction system
        auto* predictionSystem = brain.getPredictionSystem();
        if (predictionSystem) {
            std::vector<float> dummyPrediction = {0.5f};
            predictionSystem->updatePrediction(dummyPrediction);
        }
        
        // 8. Trigger memory replay periodically
        if (step % 50 == 0) {
            brain.getEpisodicMemory()->triggerReplay();
        }
        
        // 9. Save checkpoints periodically
        if (step % 100 == 0) {
            brain.save("./periodic_checkpoint.nlm");
        }
    }
    
    // Verify integration
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalSpikeCount() > 0);
    
    // Cleanup
    if (std::filesystem::exists("./periodic_checkpoint.nlm")) {
        std::filesystem::remove("./periodic_checkpoint.nlm");
    }
    
    std::cout << "    testCompleteBrainLoop passed" << std::endl;
}

void runAll() {
    std::cout << "=== System Integration Tests ===" << std::endl;
    testCheckpointSaveLoad();
    testMemoryReplayConsolidation();
    testDevelopmentSystemIntegration();
    testCompleteBrainLoop();
    std::cout << "=== System Integration Tests Complete ===" << std::endl;
}

} // namespace test_system_integration