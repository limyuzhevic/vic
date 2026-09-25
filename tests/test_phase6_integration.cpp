// Phase 6 Integration Tests for NLM
// Tests integration of all Phase 6 systems: memory, prediction, cognition, neuromodulation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>

namespace test_phase6_integration {

void testMemorySystemIntegration() {
    // Test Memory System integration
    auto config = nlm::ConfigFactory::createMemoryConfig();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Working Memory
    auto workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test Episodic Memory
    auto episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test Associative Memory
    auto associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Simulate memory operations
    brain.step(0);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testPredictionSystemIntegration() {
    // Test Prediction System integration
    auto config = nlm::ConfigFactory::createBrainSimulationConfig();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Prediction System
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Simulate prediction operations
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testCognitionSystemIntegration() {
    // Test Cognition System integration
    auto config = nlm::ConfigFactory::createDevelopmentConfig();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Neural Planner
    auto planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test Concept Formation
    auto conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test Attentional Selection
    auto attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Simulate cognition operations
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testCognitionSystemIntegration passed" << std::endl;
}

void testNeuromodulationSystemIntegration() {
    // Test Neuromodulation System integration
    auto config = nlm::ConfigFactory::createNeuromodulationConfig();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Dopamine
    auto dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test Curiosity
    auto curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test Novelty
    auto novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test Prediction Error
    auto predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test Reward
    auto reward = brain.getReward();
    assert(reward != nullptr);
    
    // Simulate neuromodulation operations
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testNeuromodulationSystemIntegration passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    // Test Development System integration
    auto config = nlm::ConfigFactory::createDevelopmentConfig();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Development System
    auto developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Simulate development operations
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void testAllSystemsIntegration() {
    // Test integration of all Phase 6 systems
    auto config = nlm::ConfigFactory::createBrainSimulationConfig();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all systems are accessible
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    assert(brain.getReward() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Run simulation with all systems integrated
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify statistics
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getRegionCount() == 3);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testAllSystemsIntegration passed" << std::endl;
}

void testPhase6CompleteIntegration() {
    // Test complete Phase 6 integration
    auto config = nlm::ConfigFactory::createDevelopmentConfig();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run complex simulation with all systems
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Simulate various system interactions
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 25.0f);
        }
        
        brain.step(step);
    }
    
    // Final verification
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 5);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testPhase6CompleteIntegration passed" << std::endl;
}

void runAll() {
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testCognitionSystemIntegration();
    testNeuromodulationSystemIntegration();
    testDevelopmentSystemIntegration();
    testAllSystemsIntegration();
    testPhase6CompleteIntegration();
    
    std::cout << "\nAll Phase 6 integration tests passed!\n" << std::endl;
}

} // namespace test_phase6_integration
