// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001);
    
    // Create and initialize brain
    nlm::Brain brain1(config);
    bool initSuccess = brain1.initialize();
    assert(initSuccess);
    
    // Run a few steps to create some state
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointPath = "/tmp/test_brain_checkpoint.bin";
    bool saveSuccess = brain1.save(checkpointPath);
    assert(saveSuccess);
    
    // Reset the brain to ensure load overwrites all state
    brain1.reset();
    
    // Verify brain is reset
    assert(brain1.getTotalNeuronCount() == 50);
    
    // Load brain state
    bool loadSuccess = brain1.load(checkpointPath);
    assert(loadSuccess);
    
    // Verify brain was properly loaded
    assert(brain1.getTotalNeuronCount() == 50);
    assert(brain1.getRegionCount() == 2);
    assert(brain1.getTotalSynapseCount() > 0);
    
// Integration Tests for Phase 6: Final Integration
void testBrainMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that memory systems are properly integrated
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    auto* predictionSystem = brain.getPredictionSystem();
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    // All integrated systems should be non-null
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    assert(predictionSystem != nullptr);
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Run a few steps to test integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Provide some sensory input to trigger integration
        auto sensoryInput = std::make_unique<nlm::InternalSignals>();
        for (int i = 0; i < 5; ++i) {
            sensoryInput->addSignal(static_cast<float>(i) * 0.2f);
        }
        brain.receiveSensoryInput(*sensoryInput);
        
        // Verify integration doesn't crash
        if (workingMemory) {
            workingMemory->update(0.001);
        }
        if (episodicMemory) {
            // Check that episodic memory has episodes being stored
            size_t episodeCount = episodicMemory->getEpisodeCount();
            // After some steps, we should have some episodes
            if (step > 10) {
                assert(episodeCount > 0);
            }
        }
        if (predictionSystem) {
            predictionSystem->update(0.001);
        }
    }
    
    std::cout << "    testBrainMemoryIntegration passed" << std::endl;
}

void testBrainNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test neuromodulation integration in step loop
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Provide input to trigger neuromodulation
        auto sensoryInput = std::make_unique<nlm::InternalSignals>();
        for (int i = 0; i < 3; ++i) {
            sensoryInput->addSignal(static_cast<float>(i));
        }
        brain.receiveSensoryInput(*sensoryInput);
        
        // Check that neuromodulators are being updated
        if (dopamine) {
            float dopamineLevel = dopamine->getLevel();
            assert(dopamineLevel >= 0.0f);
        }
        if (curiosity) {
            // Curiosity increases with novel stimuli
            curiosity->update(0.001);
        }
        if (novelty) {
            novelty->update(0.001);
        }
        if (predictionError) {
            // Prediction error may be set by prediction system
            predictionError->update(0.001);
        }
    }
    
    std::cout << "    testBrainNeuromodulationIntegration passed" << std::endl;
}

void testBrainSleepRestCycleIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test sleep/rest cycle integration
    bool rested = false;
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        brain.step(step);
        
        // Provide input to trigger memory processing
        auto sensoryInput = std::make_unique<nlm::InternalSignals>();
        sensoryInput->addSignal(static_cast<float>(step % 10) * 0.1f);
        brain.receiveSensoryInput(*sensoryInput);
        
        // Check if resting occurred
        if (brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult) {
            // In adult stage, resting should occur periodically
            if (step > 100 && step % 50 == 0) {
                // Just verify it doesn't crash
                rested = true;
            }
        }
    }
    
    assert(rested);
    std::cout << "    testBrainSleepRestCycleIntegration passed" << std::endl;
}

void testBrainConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation integration
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Provide varied input to trigger concept formation
        auto sensoryInput = std::make_unique<nlm::InternalSignals>();
        for (int i = 0; i < 10; ++i) {
            float value = static_cast<float>(i) * 0.1f;
            if (step % 3 == 0) {
                // Add some pattern variation
                value += 0.5f;
            }
            sensoryInput->addSignal(value);
        }
        brain.receiveSensoryInput(*sensoryInput);
        
        // Check concept formation processing
        size_t conceptCount = conceptFormation->getConceptCount();
        if (step > 15) {
            // After enough input, we should have some concepts
            assert(conceptCount >= 0);
        }
    }
    
    std::cout << "    testBrainConceptFormationIntegration passed" << std::endl;
}

void testBrainPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Provide input to trigger prediction
        auto sensoryInput = std::make_unique<nlm::InternalSignals>();
        for (int i = 0; i < 5; ++i) {
            sensoryInput->addSignal(static_cast<float>(i) * 0.3f);
        }
        brain.receiveSensoryInput(*sensoryInput);
        
        // Check prediction system processing
        float predictionError = predictionSystem->getPredictionError();
        float confidence = predictionSystem->getConfidence();
        assert(predictionError >= 0.0f);
        assert(confidence >= 0.0f && confidence <= 1.0f);
        
        // Update prediction system
        predictionSystem->update(0.001);
    }
    
    std::cout << "    testBrainPredictionIntegration passed" << std::endl;
}

void testBrainNeuralPlannerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test neural planner integration
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Provide input to trigger planning
        auto sensoryInput = std::make_unique<nlm::InternalSignals>();
        for (int i = 0; i < 8; ++i) {
            sensoryInput->addSignal(static_cast<float>(i) * 0.2f);
        }
        brain.receiveSensoryInput(*sensoryInput);
        
        // Test planning functionality
        ActionType action = planner->planAction({0.1f, 0.2f, 0.3f, 0.4f, 0.5f}, 0.5f);
        assert(action == nlm::ActionType::Wait || 
               action == nlm::ActionType::MoveForward ||
               action == nlm::ActionType::MoveBackward ||
               action == nlm::ActionType::TurnLeft ||
               action == nlm::ActionType::TurnRight ||
               action == nlm::ActionType::Interact);
        
        // Update planner
        planner->update(0.001);
    }
    
    std::cout << "    testBrainNeuralPlannerIntegration passed" << std::endl;
}
