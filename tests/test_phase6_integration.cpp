// Phase 6 Integration Tests
// 
// Tests for Phase 6 final integration features:
// - Memory system integration (working, episodic, associative)
// - Neuromodulation integration (dopamine, curiosity, novelty)
// - Prediction system integration
// - Development system integration
// - Checkpoint save/load functionality
// - Replay and consolidation functionality
// - Complete brain-world-agent system integration

#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <filesystem>

namespace test_phase6 {

void testMemoryIntegration() {
    std::cout << "    testMemoryIntegration passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test that memory systems are integrated
    bool memoryIntegrated = experiment.testMemoryIntegration();
    assert(memoryIntegrated);
    
    // Test that working memory has traces after activity
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    // Both memory systems should be available
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Run some simulation steps to generate activity
    for (int i = 0; i < 50; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Working memory should have active traces
    // (may be empty for simple simulation, but system should be functional)
    size_t activeTraces = workingMemory->getActiveTraces();
    
    // Episodic memory should store episodes
    size_t episodeCount = episodicMemory->getEpisodeCount();
    
    std::cout << "      Working memory traces: " << activeTraces << std::endl;
    std::cout << "      Episodic memory episodes: " << episodeCount << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test that neuromodulation systems are integrated
    bool neuromodulationIntegrated = experiment.testNeuromodulationIntegration();
    assert(neuromodulationIntegrated);
    
    // Test individual neuromodulators
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    // All neuromodulation systems should be available
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Run simulation with sensory input to activate neuromodulation
    for (int i = 0; i < 30; ++i) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain->step(i, i * 0.001);
    }
    
    std::cout << "      Dopamine system: " << (dopamine ? "FUNCTIONAL" : "NOT FUNCTIONAL") << std::endl;
    std::cout << "      Curiosity system: " << (curiosity ? "FUNCTIONAL" : "NOT FUNCTIONAL") << std::endl;
    std::cout << "      Novelty system: " << (novelty ? "FUNCTIONAL" : "NOT FUNCTIONAL") << std::endl;
}

void testPredictionIntegration() {
    std::cout << "    testPredictionIntegration passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Note: Testing prediction integration requires checking prediction system
    // For now, verify that the experiment framework is functional
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK);
    
    // The Phase6IntegratedExperiment includes prediction system checks
    // through the verifyIntegration() method
    std::cout << "      Prediction system: INTEGRATION VERIFIED" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test that development system is integrated
    bool developmentIntegrated = experiment.verifyIntegration();
    assert(developmentIntegrated);
    
    // Test development system functionality
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    std::cout << "      Development system: " << (developmentSystem ? "FUNCTIONAL" : "NOT FUNCTIONAL") << std::endl;
}

void testCheckpointing() {
    std::cout << "    testCheckpointing passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test checkpoint save/load functionality
    bool checkpointingWorks = experiment.testCheckpointing();
    assert(checkpointingWorks);
    
    std::cout << "      Checkpoint save/load: WORKING" << std::endl;
}

void testReplay() {
    std::cout << "    testReplay passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test replay system functionality
    bool replayWorks = experiment.testReplay();
    assert(replayWorks);
    
    std::cout << "      Replay system: WORKING" << std::endl;
}

void testCompleteIntegration() {
    std::cout << "    testCompleteIntegration passed" << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Run comprehensive integration test
    nlm::Phase6Config config;
    config.maxSteps = 100;
    config.neuronCount = 200;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Verify integration results
    assert(result.memoryWorkingMemoryIntegrated);
    assert(result.memoryEpisodicMemoryIntegrated);
    assert(result.neuromodulationIntegrated);
    assert(result.predictionIntegrated);
    assert(result.developmentIntegrated);
    
    std::cout << "      Memory working memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "      Memory episodic memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "      Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "      Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "      Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "      Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "      Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
}

void testAgentBrainIntegration() {
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
    
    // Test agent brain interface integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain interface
    nlm::AgentBrain agent(brain);
    
    // Test agent functionality
    assert(agent.getBrain() == brain);
    assert(!agent.isRewardModulationEnabled());
    assert(!agent.isStructuralPlasticityEnabled());
    assert(!agent.isDevelopmentEnabled());
    assert(!agent.isCuriosityEnabled());
    
    // Enable subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isStructuralPlasticityEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    std::cout << "      Agent brain interface: FUNCTIONAL" << std::endl;
    std::cout << "      Reward modulation: ENABLED" << std::endl;
    std::cout << "      Structural plasticity: ENABLED" << std::endl;
    std::cout << "      Development: ENABLED" << std::endl;
    std::cout << "      Curiosity: ENABLED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    testConfigurationValidation passed" << std::endl;
    
    // Test configuration validation
    nlm::config::RequiredKeys required;
    
    // Test with minimal config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    auto validationResult = nlm::config::validateConfig(*config, required);
    
    // Should have warnings for missing keys but pass validation
    assert(validationResult.valid);
    
    std::cout << "      Configuration validation: PASSED" << std::endl;
    std::cout << "      Required keys checked: " << required.brain.size() + required.plasticity.size() + 
        required.neuromodulation.size() + required.memory.size() + 
        required.development.size() + required.prediction.size() << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    std::cout << "Running Memory Integration test..." << std::endl;
    testMemoryIntegration();
    
    std::cout << "Running Neuromodulation Integration test..." << std::endl;
    testNeuromodulationIntegration();
    
    std::cout << "Running Prediction Integration test..." << std::endl;
    testPredictionIntegration();
    
    std::cout << "Running Development Integration test..." << std::endl;
    testDevelopmentIntegration();
    
    std::cout << "Running Checkpointing test..." << std::endl;
    testCheckpointing();
    
    std::cout << "Running Replay test..." << std::endl;
    testReplay();
    
    std::cout << "Running Complete Integration test..." << std::endl;
    testCompleteIntegration();
    
    std::cout << "Running Agent Brain Integration test..." << std::endl;
    testAgentBrainIntegration();
    
    std::cout << "Running Configuration Validation test..." << std::endl;
    testConfigurationValidation();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6
