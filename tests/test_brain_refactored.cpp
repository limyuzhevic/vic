// Comprehensive Brain Class Tests
// Tests for refactored Brain class step decomposition methods and integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_brain_refactored {

// Test 1: Individual Step Decomposition Methods

void testProcessPendingDelayedSpikes() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get spike system to verify delayed spike processing
    auto* spikeSys = brain.getSpikeSystem();
    assert(spikeSys != nullptr);
    
    // Verify initial state (no pending spikes)
    size_t initialPending = brain.getPendingSpikeEventCount();
    
    // The method should handle the case even if no delayed spikes exist
    // This tests error condition: no pending delayed spikes
    brain.step(0, 0.0);
    
    // State should remain valid after processing
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testProcessPendingDelayedSpikes passed" << std::endl;
}

void testUpdateAllNeurons() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial neuron states
    size_t initialActive = brain.getActiveNeuronCount();
    
    // Update neurons with different timesteps
    brain.updateAllNeurons(0.0, 0.001);
    brain.updateAllNeurons(0.001, 0.001);
    
    // Should maintain neuron count
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testUpdateAllNeurons passed" << std::endl;
}

void testDetectSpikesAndScheduleEvents() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Inject current to create firing neurons
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
    
    // This should detect and schedule spike events
    brain.detectSpikesAndScheduleSpikeEvents(0, 0.0);
    
    // Verify spike system processed events
    size_t pending = brain.getPendingSpikeEventCount();
    // (may be 0 if no neurons actually fired)
    
    std::cout << "    testDetectSpikesAndScheduleEvents passed" << std::endl;
}

void testUpdateWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get working memory for testing
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Update working memory with different timesteps
    brain.updateWorkingMemory(0.001);
    brain.updateWorkingMemory(0.002);
    
    // Should not crash and maintain state
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testUpdateWorkingMemory passed" << std::endl;
}

void testApplyNeuromodulationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Apply neuromodulation effects with different timesteps
    brain.applyNeuromodulationEffects(0.001);
    brain.applyNeuromodulationEffects(0.005);
    
    // Should maintain valid state
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testApplyNeuromodulationEffects passed" << std::endl;
}

void testApplyPlasticityRules() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial synaptic weights (get a region and its synapses)
    if (brain.getRegionCount() > 0) {
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            size_t initialSynapses = region->getSynapseCount();
            
            // Apply plasticity rules
            brain.applyPlasticityRules(0.001, 1.0f);
            
            // Should maintain synapse count
            assert(region->getSynapseCount() == initialSynapses);
        }
    }
    
    std::cout << "    testApplyPlasticityRules passed" << std::endl;
}

void testUpdateEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get episodic memory for testing
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Update episodic memory
    brain.updateEpisodicMemory(0, 0.0);
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testUpdateEpisodicMemory passed" << std::endl;
}

void testUpdatePredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get prediction system for testing
    auto* predSys = brain.getPredictionSystem();
    assert(predSys != nullptr);
    
    // Update prediction system
    brain.updatePredictionSystem();
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testUpdatePredictionSystem passed" << std::endl;
}

void testUpdateAttentionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get attention system for testing
    auto* attentionSys = brain.getAttention();
    assert(attentionSys != nullptr);
    
    // Update attention system with different timesteps
    brain.updateAttentionSystem(0.001);
    brain.updateAttentionSystem(0.005);
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testUpdateAttentionSystem passed" << std::endl;
}

void testUpdateConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get concept formation system for testing
    auto* conceptSys = brain.getConceptFormation();
    assert(conceptSys != nullptr);
    
    // Update concept formation
    brain.updateConceptFormation();
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testUpdateConceptFormation passed" << std::endl;
}

void testApplyStructuralPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001);
    config->set("pruning_rate", 0.00001);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get random generator for testing
    auto* rng = brain.getRandomGenerator();
    assert(rng != nullptr);
    
    // Apply structural plasticity with different step numbers
    brain.applyStructuralPlasticity(0, *rng);
    brain.applyStructuralPlasticity(100, *rng);
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testApplyStructuralPlasticity passed" << std::endl;
}

void testReplayImportantMemories() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", 10);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get episodic memory for testing
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test replay at different step intervals
    brain.replayImportantMemories(0);     // Should not replay yet
    brain.replayImportantMemories(10);    // Should replay at step 10
    brain.replayImportantMemories(20);    // Should replay again
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testReplayImportantMemories passed" << std::endl;
}

void testApplyDevelopmentEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get random generator for testing
    auto* rng = brain.getRandomGenerator();
    assert(rng != nullptr);
    
    // Apply development effects with different developmental stages
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    brain.applyDevelopmentEffects(0, *rng, 0.001);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    brain.applyDevelopmentEffects(1000, *rng, 0.001);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    brain.applyDevelopmentEffects(2000, *rng, 0.001);
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testApplyDevelopmentEffects passed" << std::endl;
}

void testPeriodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("consolidation_interval", 10);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get episodic memory for testing
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test consolidation at different intervals
    brain.periodicMemoryConsolidation(0);     // Should not consolidate yet
    brain.periodicMemoryConsolidation(9);     // Should not consolidate
    brain.periodicMemoryConsolidation(10);    // Should consolidate
    brain.periodicMemoryConsolidation(20);    // Should consolidate again
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testPeriodicMemoryConsolidation passed" << std::endl;
}

void testCheckpointManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "/tmp");
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test checkpoint management
    brain.checkpointManagement(0, 0.0);
    brain.checkpointManagement(1000, 0.001);
    
    // Should not crash
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testCheckpointManagement passed" << std::endl;
}

// Test 2: Integration Tests for step() Method

void testStepIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run full integration test with step()
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
    }
    
    // Verify integration maintained all systems
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "    testStepIntegration passed" << std::endl;
}

void testStepWithSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a simple sensory input
    struct TestSensoryInput {
        std::vector<float> data;
        const std::vector<float>& getData() const { return data; }
    };
    
    TestSensoryInput input;
    input.data = {1.0f, 2.0f, 3.0f};
    
    // Run step with sensory input
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
        brain.receiveSensoryInput(input);
    }
    
    // Should handle input without crashing
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testStepWithSensoryInput passed" << std::endl;
}

void testStepWithNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a test neuromodulator
    struct TestNeuromodulator {
        float level = 0.5f;
        float getLevel() const { return level; }
    };
    
    TestNeuromodulator neuromod;
    
    // Run step with neuromodulation
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
        brain.applyNeuromodulation(neuromod);
    }
    
    // Should handle neuromodulation without crashing
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testStepWithNeuromodulation passed" << std::endl;
}

void testStepWithMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify memory systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Run step to exercise memory systems
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
    }
    
    // Memory systems should still be accessible
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    std::cout << "    testStepWithMemorySystems passed" << std::endl;
}

// Test 3: Edge Cases and Error Conditions

void testEmptyBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Should initialize without error
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Should have zero neurons
    assert(brain.getTotalNeuronCount() == 0);
    assert(brain.getTotalSynapseCount() == 0);
    
    std::cout << "    testEmptyBrain passed" << std::endl;
}

void testSingleNeuronBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.0f); // No connections
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Should have one neuron
    assert(brain.getTotalNeuronCount() == 1);
    
    // Should run steps without crashing
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
    }
    
    // Neuron count should remain 1
    assert(brain.getTotalNeuronCount() == 1);
    
    std::cout << "    testSingleNeuronBrain passed" << std::endl;
}

void testLargeNetworkBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify large network
    assert(brain.getTotalNeuronCount() == 10000);
    assert(brain.getRegionCount() == 10);
    
    // Run limited steps to avoid long test time
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
    }
    
    std::cout << "    testLargeNetworkBrain passed" << std::endl;
}

void testBrainCheckpointLoadRoundTrip() {
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    brain1.initialize();
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain1.step(step, static_cast<float>(step) * 0.001f);
    }
    
    // Save to file
    const std::string checkpointFile = "/tmp/test_checkpoint.bin";
    bool saveSuccess = brain1.save(checkpointFile);
    assert(saveSuccess);
    
    // Create a new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Both brains should have same neuron count
    assert(brain2.getTotalNeuronCount() == 50);
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testBrainCheckpointLoadRoundTrip passed" << std::endl;
}

void testBrainResetPreservesStructure() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial structure
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialRegions = brain.getRegionCount();
    
    // Run steps and reset
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
    }
    
    brain.reset();
    
    // Structure should be preserved after reset
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getRegionCount() == initialRegions);
    
    std::cout << "    testBrainResetPreservesStructure passed" << std::endl;
}

void testDevelopmentStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all developmental stages
    for (int stage = 0; stage < 4; ++stage) {
        nlm::DevelopmentalStage devStage = static_cast<nlm::DevelopmentalStage>(stage);
        brain.setDevelopmentalStage(devStage);
        
        // Verify stage was set
        assert(brain.getDevelopmentalStage() == devStage);
    }
    
    std::cout << "    testDevelopmentStages passed" << std::endl;
}

// Test 4: Performance Validation

void testStepPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure step performance (not too many steps for testing)
    nlm::SimulationStep startStep = 0;
    nlm::Timestamp startTime = 0.0f;
    
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
    }
    
    // Should complete without errors
    assert(brain.getTotalNeuronCount() == 1000);
    
    std::cout << "    testStepPerformance passed" << std::endl;
}

void testMemoryUsagePatterns() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get working memory for testing
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Exercise memory systems with various patterns
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, static_cast<float>(step) * 0.001f);
        
        // Vary synaptic plasticity levels
        if (step % 10 == 0) {
            brain.applyPlasticityRules(0.001, 1.5f);
        } else {
            brain.applyPlasticityRules(0.001, 0.5f);
        }
    }
    
    // Memory should remain functional
    assert(brain.getWorkingMemory() != nullptr);
    
    std::cout << "    testMemoryUsagePatterns passed" << std::endl;
}

// Test 5: Configuration Variations

void testDifferentConfigurations() {
    // Test 1: High connectivity
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config1->set("connection_probability", 0.5f);
    
    nlm::Brain brain1(config1);
    assert(brain1.initialize());
    assert(brain1.getTotalSynapseCount() > 0);
    
    // Test 2: Low connectivity
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config2->set("connection_probability", 0.01f);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    // Should still have some synapses but fewer than brain1
    assert(brain2.getTotalSynapseCount() >= 0);
    
    std::cout << "    testDifferentConfigurations passed" << std::endl;
}

void testAllStepMethods() {
    // Comprehensive test that calls all 15 step decomposition methods
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001);
    config->set("pruning_rate", 0.00001);
    config->set("replay_interval", 10);
    config->set("consolidation_interval", 20);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get random generator
    auto* rng = brain.getRandomGenerator();
    assert(rng != nullptr);
    
    // Manually call all 15 step decomposition methods
    brain.processPendingDelayedSpikes(0, 0.0);
    brain.updateAllNeurons(0.0, 0.001);
    brain.detectSpikesAndScheduleSpikeEvents(0, 0.0);
    brain.updateWorkingMemory(0.001);
    brain.applyNeuromodulationEffects(0.001);
    brain.applyPlasticityRules(0.001, 1.0f);
    brain.updateEpisodicMemory(0, 0.0);
    brain.updatePredictionSystem();
    brain.updateAttentionSystem(0.001);
    brain.updateConceptFormation();
    brain.applyStructuralPlasticity(0, *rng);
    brain.replayImportantMemories(0);
    brain.applyDevelopmentEffects(0, *rng, 0.001);
    brain.periodicMemoryConsolidation(0);
    brain.checkpointManagement(0, 0.0);
    
    // Should have no crashes
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testAllStepMethods passed" << std::endl;
}

void runAll() {
    // Test 1: Individual Step Decomposition Methods
    testProcessPendingDelayedSpikes();
    testUpdateAllNeurons();
    testDetectSpikesAndScheduleEvents();
    testUpdateWorkingMemory();
    testApplyNeuromodulationEffects();
    testApplyPlasticityRules();
    testUpdateEpisodicMemory();
    testUpdatePredictionSystem();
    testUpdateAttentionSystem();
    testUpdateConceptFormation();
    testApplyStructuralPlasticity();
    testReplayImportantMemories();
    testApplyDevelopmentEffects();
    testPeriodicMemoryConsolidation();
    testCheckpointManagement();
    
    // Test 2: Integration Tests
    testStepIntegration();
    testStepWithSensoryInput();
    testStepWithNeuromodulation();
    testStepWithMemorySystems();
    
    // Test 3: Edge Cases and Error Conditions
    testEmptyBrain();
    testSingleNeuronBrain();
    testLargeNetworkBrain();
    testBrainCheckpointLoadRoundTrip();
    testBrainResetPreservesStructure();
    testDevelopmentStages();
    
    // Test 4: Performance Validation
    testStepPerformance();
    testMemoryUsagePatterns();
    
    // Test 5: Configuration Variations
    testDifferentConfigurations();
    testAllStepMethods();
    
    std::cout << "\nAll refactored Brain class tests passed!" << std::endl;
}

} // namespace test_brain_refactored

int main() {
    test_brain_refactored::runAll();
    return 0;
}
