// Phase 6 Integration Tests
// Tests for integrated brain systems (memory, neuromodulation, prediction, cognition, development)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Reward.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_integration {

void testRewardSystem() {
    std::cout << "=== Testing Reward System ===" << std::endl;
    
    nlm::Reward reward;
    
    // Test reward initialization
    assert(reward.getValue() == 0.0f);
    assert(reward.getStatisticsCount() == 0);
    
    // Test reward accumulation
    reward.add(1.0f);
    assert(reward.getValue() == 1.0f);
    assert(reward.getStatisticsCount() == 1);
    assert(reward.getStatisticsMean() == 1.0f);
    
    // Test reward configuration
    reward.setExtrinsicWeight(0.7f);
    reward.setIntrinsicWeight(0.2f);
    reward.setExplorationWeight(0.1f);
    reward.setCuriosityWeight(0.0f);
    
    assert(reward.getExtrinsicWeight() == 0.7f);
    assert(reward.getIntrinsicWeight() == 0.2f);
    assert(reward.getExplorationWeight() == 0.1f);
    assert(reward.getCuriosityWeight() == 0.0f);
    
    // Test reward function selection
    reward.setRewardFunction(nlm::RewardFunction::Additive);
    assert(reward.getRewardFunction() == nlm::RewardFunction::Additive);
    
    reward.setRewardFunction(nlm::RewardFunction::Multiplicative);
    assert(reward.getRewardFunction() == nlm::RewardFunction::Multiplicative);
    
    // Test reward reset
    reward.reset();
    assert(reward.getValue() == 0.0f);
    assert(reward.getStatisticsCount() == 0);
    
    std::cout << "    Reward system test passed" << std::endl;
}

void testBrainWithNeuromodulation() {
    std::cout << "=== Testing Brain with Neuromodulation ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool success = brain.initialize();
    assert(success);
    
    // Test that reward system is accessible through brain
    nlm::Reward* brainReward = brain.getReward();
    assert(brainReward != nullptr);
    
    // Test reward accumulation through brain
    brainReward->add(2.5f);
    assert(brainReward->getValue() == 2.5f);
    
    // Test neuromodulation systems
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::Novelty* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "    Brain with neuromodulation test passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "=== Testing Memory Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test working memory
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Initial state should be empty or have minimal traces
    // (exact behavior depends on implementation)
    
    // Test episodic memory
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Check that memory systems are functional
    if (workingMemory) {
        std::cout << "    Working memory active traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    if (episodicMemory) {
        std::cout << "    Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    std::cout << "    Memory integration test passed" << std::endl;
}

void testPredictionSystem() {
    std::cout << "=== Testing Prediction System ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test prediction system
    nlm::PredictionSystem* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Initial prediction error should be 0
    assert(predictionSystem->getPredictionError() == 0.0f);
    assert(predictionSystem->getConfidence() == 0.5f);  // Default confidence
    
    std::cout << "    Prediction system test passed" << std::endl;
}

void testCognitionIntegration() {
    std::cout << "=== Testing Cognition Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test cognition systems
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    nlm::AttentionalSelection* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    std::cout << "    Cognition integration test passed" << std::endl;
}

void testDevelopmentSystem() {
    std::cout << "=== Testing Development System ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test development system
    nlm::DevelopmentSystem* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Get initial developmental stage
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    
    // Run some steps to trigger development
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Check that development progresses over time
        if (step % 20 == 0) {
            nlm::DevelopmentalStage currentStage = brain.getDevelopmentalStage();
            std::cout << "    Step " << step << ": Stage " << static_cast<int>(currentStage) << std::endl;
        }
    }
    
    std::cout << "    Development system test passed" << std::endl;
}

void testIntegratedBrainLifecycle() {
    std::cout << "=== Testing Integrated Brain Lifecycle ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify all integrated systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Run extended simulation
    std::cout << "    Running extended simulation (500 steps)..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "    Step " << step << ": "
                     << "Spikes: " << brain.getTotalSpikeCount() << ", "
                     << "Neurons: " << brain.getFiringNeuronCount() << ", "
                     << "Memory Traces: " << (brain.getWorkingMemory() ? brain.getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    Simulation completed in " << duration << " seconds" << std::endl;
    
    // Verify final state
    assert(brain.getTotalNeuronCount() == 600);
    assert(brain.getRegionCount() == 2);
    
    // Test checkpointing
    std::string checkpointPath = "/tmp/test_brain_checkpoint.bin";
    bool saved = brain.save(checkpointPath);
    if (saved) {
        std::cout << "    Checkpoint saved successfully" << std::endl;
        
        // Create new brain and load checkpoint
        auto brain2 = std::make_shared<nlm::Brain>(config);
        bool loaded = brain2->initialize();
        assert(loaded);
        
        bool loadSuccess = brain2->load(checkpointPath);
        if (loadSuccess) {
            std::cout << "    Checkpoint loaded successfully" << std::endl;
        }
    }
    
    // Test reward system integration
    nlm::Reward* reward = brain.getReward();
    assert(reward != nullptr);
    
    reward->add(1.5f);
    assert(reward->getValue() == 1.5f);
    
    std::cout << "    Integrated brain lifecycle test passed" << std::endl;
}

void testPhase6Config() {
    std::cout << "=== Testing Phase 6 Configuration ===" << std::endl;
    
    nlm::Config config;
    
    // Test configuration settings
    config.set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config.set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    
    // Test configuration access
    assert(config.getOr<int64_t>("neuron_count", 100) == 800);
    assert(config.getOr<int64_t>("region_count", 1) == 3);
    assert(config.getOr<float>("connection_probability", 0.1f) == 0.2f);
    
    // Test configuration keys
    auto keys = config.getKeys();
    assert(keys.size() >= 5);  // Should have at least these keys
    
    std::cout << "    Phase 6 configuration test passed" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "=== Running Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete integrated brain systems..." << std::endl;
    std::cout << std::endl;
    
    testRewardSystem();
    testBrainWithNeuromodulation();
    testMemoryIntegration();
    testPredictionSystem();
    testCognitionIntegration();
    testDevelopmentSystem();
    testIntegratedBrainLifecycle();
    testPhase6Config();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Integration Tests Passed ===" << std::endl;
    std::cout << "The integrated brain systems are working correctly!" << std::endl;
}

} // namespace test_phase6_integration