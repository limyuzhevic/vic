// Integration Tests for NLM Phase 6 Full System
// Tests the complete integration of all Phase 6 brain systems
// Uses the existing Phase6IntegratedExperiment for comprehensive integration testing

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

namespace nlm {

namespace test_integration {

// Test 1: Basic Phase 6 integration verification
void testPhase6IntegrationVerification() {
    std::cout << "    testPhase6IntegrationVerification - Checking basic integration" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool integrationOK = experiment.verifyIntegration();
    
    assert(integrationOK);
    std::cout << "    testPhase6IntegrationVerification passed" << std::endl;
}

// Test 2: Memory system integration (working memory + episodic + associative)
void testMemorySystemIntegration() {
    std::cout << "    testMemorySystemIntegration - Testing memory systems" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool memoryOK = experiment.testMemoryIntegration();
    
    assert(memoryOK);
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

// Test 3: Neuromodulation system integration (dopamine + curiosity + novelty)
void testNeuromodulationSystemIntegration() {
    std::cout << "    testNeuromodulationSystemIntegration - Testing neuromodulation systems" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    
    assert(neuromodulationOK);
    std::cout << "    testNeuromodulationSystemIntegration passed" << std::endl;
}

// Test 4: Development system integration
void testDevelopmentSystemIntegration() {
    std::cout << "    testDevelopmentSystemIntegration - Testing development system" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool developmentOK = experiment.testDevelopmentIntegration();
    
    assert(developmentOK);
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

// Test 5: Checkpoint and replay integration
void testCheckpointReplayIntegration() {
    std::cout << "    testCheckpointReplayIntegration - Testing checkpoint and replay" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool checkpointOK = experiment.testCheckpointing();
    
    assert(checkpointOK);
    std::cout << "    testCheckpointReplayIntegration passed" << std::endl;
}

// Test 6: Complete lifecycle integration
void testCompleteLifecycleIntegration() {
    std::cout << "    testCompleteLifecycleIntegration - Testing complete lifecycle" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    config.maxSteps = 500;
    config.neuronCount = 300;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Verify all systems were integrated
    assert(result.memoryWorkingMemoryIntegrated);
    assert(result.memoryEpisodicMemoryIntegrated);
    assert(result.neuromodulationIntegrated);
    assert(result.predictionIntegrated);
    assert(result.developmentIntegrated);
    assert(result.checkpointingWorks);
    assert(result.replayWorks);
    
    std::cout << "    testCompleteLifecycleIntegration passed" << std::endl;
}

// Test 7: Brain-world-agent integration
void testBrainWorldAgentIntegration() {
    std::cout << "    testBrainWorldAgentIntegration - Testing brain-world-agent loop" << std::endl;
    
    // Test the complete brain-world-agent integration loop
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test sensory processing
    auto world = createSimpleWorld();
    world.configure(20, 20, 8, 8);
    world.reset();
    
    auto agent = createAgentBrain(brain);
    agent.initialize(world);
    
    // Run integration steps
    for (int step = 0; step < 100; ++step) {
        world.update(0.1);
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        brain.step(step);
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
    }
    
    std::cout << "    testBrainWorldAgentIntegration passed" << std::endl;
}

// Test 8: Plasticity integration (STDP + Hebbian + structural)
void testPlasticityIntegration() {
    std::cout << "    testPlasticityIntegration - Testing plasticity mechanisms" << std::endl;
    
    // Test STDP integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test plasticity step
    brain.step(0);
    
    // Verify plasticity systems are integrated
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structural != nullptr);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

// Test 9: Cognitive function integration
void testCognitiveFunctionIntegration() {
    std::cout << "    testCognitiveFunctionIntegration - Testing cognitive systems" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Verify cognitive systems are accessible
    auto* planner = brain.getPlanner();
    auto* concept = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(concept != nullptr);
    assert(attention != nullptr);
    
    std::cout << "    testCognitiveFunctionIntegration passed" << std::endl;
}

// Test 10: Complete Phase 6 demo integration
void testCompletePhase6Demo() {
    std::cout << "    testCompletePhase6Demo - Testing complete Phase 6 demo" << std::endl;
    
    // This simulates the existing Phase6Demo.cpp behavior
    Phase6IntegratedExperiment experiment;
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Verify all integration metrics
    assert(result.totalReward >= 0.0f);
    assert(result.avgFiringRate >= 0.0f);
    assert(result.memoryEpisodesStored > 0.0f);
    assert(result.dopamineLevel >= 0.0f);
    
    // Verify all systems are integrated
    assert(result.memoryWorkingMemoryIntegrated);
    assert(result.memoryEpisodicMemoryIntegrated);
    assert(result.neuromodulationIntegrated);
    assert(result.predictionIntegrated);
    assert(result.developmentIntegrated);
    
    std::cout << "    testCompletePhase6Demo passed" << std::endl;
}

// Test 11: Performance and stability test
void testPerformanceAndStability() {
    std::cout << "    testPerformanceAndStability - Testing performance and stability" << std::endl;
    
    // Test with larger brain to ensure stability
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Run extended simulation
    for (int step = 0; step < 5000; ++step) {
        brain.step(step);
        
        // Test that brain doesn't crash and maintains basic properties
        assert(brain.getTotalNeuronCount() == 1000);
        assert(brain.getRegionCount() == 4);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
    }
    
    std::cout << "    testPerformanceAndStability passed" << std::endl;
}

// Test 12: Error handling and recovery
void testErrorHandlingAndRecovery() {
    std::cout << "    testErrorHandlingAndRecovery - Testing error handling" << std::endl;
    
    // Test initialization failure recovery
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(0), ConfigSource::Default); // Invalid config
    
    // Brain should handle invalid config gracefully or throw exception
    try {
        Brain brain(config);
        brain.initialize(); // This might fail
    } catch (...) {
        // Expected behavior
    }
    
    // Test reset functionality
    auto validConfig = std::make_shared<Config>();
    validConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(validConfig);
    brain.initialize();
    brain.step(10);
    brain.reset(); // Should recover to initial state
    
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testErrorHandlingAndRecovery passed" << std::endl;
}

// Test 13: Memory checkpoint integration
void testMemoryCheckpointIntegration() {
    std::cout << "    testMemoryCheckpointIntegration - Testing checkpoint integration" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test checkpoint save/load
    bool checkpointOK = experiment.testCheckpointing();
    assert(checkpointOK);
    
    // Verify checkpoint functionality
    Phase6Config config;
    config.maxSteps = 200;
    config.neuronCount = 300;
    config.enableCheckpointing = true;
    
    auto result = experiment.run(config);
    assert(result.checkpointingWorks);
    
    std::cout << "    testMemoryCheckpointIntegration passed" << std::endl;
}

// Test 14: Replay system integration
void testReplaySystemIntegration() {
    std::cout << "    testReplaySystemIntegration - Testing replay system integration" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    bool replayOK = experiment.testReplay();
    assert(replayOK);
    
    std::cout << "    testReplaySystemIntegration passed" << std::endl;
}

// Test 15: Development stage integration
void testDevelopmentStageIntegration() {
    std::cout << "    testDevelopmentStageIntegration - Testing development stage integration" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test developmental stages
    auto stage = brain.getDevelopmentalStage();
    assert(stage != DevelopmentalStage::Initial); // Should progress through stages
    
    std::cout << "    testDevelopmentStageIntegration passed" << std::endl;
}

// Run all integration tests
void runAll() {
    testPhase6IntegrationVerification();
    testMemorySystemIntegration();
    testNeuromodulationSystemIntegration();
    testPredictionSystemIntegration();
    testDevelopmentSystemIntegration();
    testCheckpointReplayIntegration();
    testCompleteLifecycleIntegration();
    testBrainWorldAgentIntegration();
    testPlasticityIntegration();
    testCognitiveFunctionIntegration();
    testCompletePhase6Demo();
    testPerformanceAndStability();
    testErrorHandlingAndRecovery();
    testMemoryCheckpointIntegration();
    testReplaySystemIntegration();
    testDevelopmentStageIntegration();
}

} // namespace test_integration

} // namespace nlm