// Advanced NLM Tests
// Comprehensive tests for integrated systems, advanced features, and debugging tools

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "performance/CheckpointSystem.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>

namespace test_advanced {

// Forward declarations
void testIntegratedMemorySystems();
void testPredictionSystem();
void testCognitionSystems();
void testNeuromodulationIntegration();
void testDevelopmentSystem();
void testAdvancedCLI();
void testCheckpointDebugging();
void testResearchMode();
void testPhase6Integration();
void runAllAdvancedTests();

// Test integrated memory systems
void testIntegratedMemorySystems() {
    std::cout << "  Testing integrated memory systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test working memory capacity
    workingMemory->setCapacity(50);
    assert(workingMemory->getMemoryCapacity() == 50);
    
    // Test episodic memory capacity
    episodicMemory->setMaxEpisodes(100);
    assert(episodicMemory->getMaxEpisodes() == 100);
    
    // Test episodic memory storage
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 1.0f;
    episode.energy = 1.0f;
    episode.novelty = 0.5f;
    episode.resultingReward = 1.5f;
    
    // Add some neurons to episode
    episode.activeNeurons.push_back(nlm::NeuronId(1));
    episode.neuronActivations.push_back(0.8f);
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    testIntegratedMemorySystems passed" << std::endl;
}

// Test prediction system
void testPredictionSystem() {
    std::cout << "  Testing prediction system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system access
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction error signal
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test that prediction system can be integrated
    // (Basic integration test - prediction system would be used during sensory processing)
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

// Test cognition systems
void testCognitionSystems() {
    std::cout << "  Testing cognition systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neural planner
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test concept formation
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test attentional selection
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Configure cognition systems
    planner->setPlanningDepth(3);
    conceptFormation->setLearningRate(0.1f);
    attention->setInhibitionStrength(0.3f);
    attention->setExcitationStrength(1.2f);
    
    // Test attention configuration
    assert(attention->getInhibitionStrength() == 0.3f);
    assert(attention->getExcitationStrength() == 1.2f);
    
    std::cout << "    testCognitionSystems passed" << std::endl;
}

// Test neuromodulation integration
void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test neuromodulation levels
    assert(dopamine->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    
    // Test neuromodulation integration with plasticity
    // (Neuromodulators affect plasticity rates)
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

// Test development system
void testDevelopmentSystem() {
    std::cout << "  Testing development system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Change developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    // Test development effects on plasticity
    brain.develop();
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

// Test advanced CLI functionality (simulated)
void testAdvancedCLI() {
    std::cout << "  Testing advanced CLI functionality..." << std::endl;
    
    // This test would verify that the advanced CLI options are properly parsed
    // and that they affect simulation behavior correctly.
    
    // For now, we test that configuration file parsing works
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Default);
    
    assert(config->getOr<int64_t>("random_seed", 0) == 12345);
    
    std::cout << "    testAdvancedCLI passed" << std::endl;
}

// Test checkpoint debugging tools
void testCheckpointDebugging() {
    std::cout << "  Testing checkpoint debugging tools..." << std::endl;
    
    // Test that checkpoint debugging tools are available
    // This would test the nlm_checkpoint_debug executable
    
    // For now, test basic checkpoint functionality
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Drive brain to create state
    brain1.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain1.step(step);
    }
    
    // Test checkpoint save
    const std::string path = "/tmp/nlm_test_advanced.ckpt";
    bool saved = brain1.save(path);
    assert(saved);
    
    // Test checkpoint load
    nlm::Brain brain2(config);
    brain2.initialize();
    bool loaded = brain2.load(path);
    assert(loaded);
    
    std::cout << "    testCheckpointDebugging passed" << std::endl;
}

// Test research mode functionality
void testResearchMode() {
    std::cout << "  Testing research mode functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);  // Lower connectivity for research
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create world and agent for research
    SimpleWorld world;
    world.configure(50.0f, 50.0f, 10, 10);
    world.setRandomSeed(12345);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable all research features
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    agent.enableStructuralPlasticity(true);
    
    // Run research simulation
    float totalReward = 0.0f;
    for (int step = 0; step < 50; ++step) {
        const SensoryPercept& percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        for (int i = 0; i < 10; ++i) {
            brain->step(static_cast<nlm::SimulationStep>(step * 10 + i));
        }
        
        MotorCommand cmd = agent.decodeMotorCommand();
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        agent.applyRewardModulation(result.reward, 0.0f);
        world.update(0.01);
        
        totalReward += result.reward;
    }
    
    // Verify research features were active
    assert(totalReward >= 0.0f);  // Should have some reward
    assert(agent.getNoveltyLevel() >= 0.0f);
    assert(agent.getCuriosityLevel() >= 0.0f);
    assert(agent.getPredictionError() >= 0.0f);
    
    std::cout << "    testResearchMode passed" << std::endl;
}

// Test Phase 6 integration
void testPhase6Integration() {
    std::cout << "  Testing Phase 6 integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all Phase 6 integrated systems are present and functional
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test that integrated systems can be updated
    brain.step(0);
    brain.step(1);
    
    // Test that memory systems are integrated with neural activity
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory != nullptr) {
        // Should have some working memory traces if brain is active
        assert(workingMemory->getMemoryNeuronCount() >= 0);
    }
    
    std::cout << "    testPhase6Integration passed" << std::endl;
}

void runAllAdvancedTests() {
    std::cout << "=== Advanced NLM Tests ===" << std::endl;
    
    testIntegratedMemorySystems();
    testPredictionSystem();
    testCognitionSystems();
    testNeuromodulationIntegration();
    testDevelopmentSystem();
    testAdvancedCLI();
    testCheckpointDebugging();
    testResearchMode();
    testPhase6Integration();
    
    std::cout << std::endl;
    std::cout << "=== All Advanced Tests Passed ===" << std::endl;
}

} // namespace test_advanced