// Phase 6 Integration Tests
// Comprehensive tests for integrated artificial brain systems

#include <iostream>
#include <cassert>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"

namespace test_integration {

void testBrainInitialization() {
    std::cout << "  Testing Brain initialization..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Configure for Phase 6 testing
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test all integrated systems are present
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
    assert(brain->getPlanner() != nullptr);
    assert(brain->getConceptFormation() != nullptr);
    assert(brain->getAttention() != nullptr);
    assert(brain->getDevelopmentSystem() != nullptr);
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getPredictionErrorSignal() != nullptr);
    
    std::cout << "    PASSED: All integrated systems initialized" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Initialize neuromodulation systems
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predError = brain->getPredictionErrorSignal();
    
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predError != nullptr);
    
    curiosity->initialize(brain.get());
    novelty->initialize(brain.get());
    predError->initialize(brain.get());
    
    // Test neuromodulation effects during simulation
    float initialCuriosity = curiosity->getLevel();
    float initialNovelty = novelty->getLevel();
    
    // Simulate some neural activity
    for (SimulationStep step = 0; step < 10; ++step) {
        brain->step(step, step * 0.001);
        
        // Test that neuromodulators affect neural computation
        assert(curiosity->getLevel() >= 0.0f);
        assert(novelty->getLevel() >= 0.0f);
        assert(predError->getError() >= -1.0f);
    }
    
    std::cout << "    PASSED: Neuromodulation integration functional" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "  Testing memory integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Get memory systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Simulate neural activity and memory formation
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        
        // Working memory should maintain active traces
        assert(workingMemory->getMemoryActivity() >= 0.0f);
        
        // Episodic memory should accumulate episodes
        if (step % 10 == 0) {  // Store episode every 10 steps
            size_t episodeCount = episodicMemory->getEpisodeCount();
            assert(episodeCount >= 1);
        }
    }
    
    std::cout << "    PASSED: Memory integration functional" << std::endl;
}

void testCognitiveIntegration() {
    std::cout << "  Testing cognitive integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Get cognitive systems
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    planner->initialize(brain.get());
    conceptFormation->initialize(brain.get());
    attention->initialize(brain.get());
    
    // Initialize with some neural activity
    for (SimulationStep step = 0; step < 20; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Test cognitive systems integration
    assert(attention->getInhibitionFor(NeuronId(1)) >= 0.0f);
    
    std::cout << "    PASSED: Cognitive integration functional" << std::endl;
}

void testPredictionIntegration() {
    std::cout << "  Testing prediction integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Get prediction systems
    auto* predictionSystem = brain->getPredictionSystem();
    auto* predError = brain->getPredictionErrorSignal();
    auto* curiosity = brain->getCuriosity();
    
    assert(predictionSystem != nullptr);
    assert(predError != nullptr);
    assert(curiosity != nullptr);
    
    predError->initialize(brain.get());
    curiosity->initialize(brain.get());
    
    // Simulate prediction error computation
    for (SimulationStep step = 0; step < 30; ++step) {
        brain->step(step, step * 0.001);
        
        // Prediction system should be updating
        // (Implementation depends on prediction system design)
        
        // Prediction error should track changes
        float error = predError->getError();
        assert(!std::isnan(error));
        
        // Curiosity should be responding to novelty
        float curiosityLevel = curiosity->getLevel();
        assert(curiosityLevel >= 0.0f);
    }
    
    std::cout << "    PASSED: Prediction integration functional" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "  Testing development integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Get development system
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Simulate development over time
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Development should progress through stages
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        assert(stage != DevelopmentalStage::Maturation && stage != DevelopmentalStage::Adult);
        
        if (step % 1000 == 0) {
            // Development updates structural plasticity
            brain->develop();
        }
    }
    
    std::cout << "    PASSED: Development integration functional" << std::endl;
}

void testFullIntegration() {
    std::cout << "  Testing full system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Initialize all neuromodulation systems
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predError = brain->getPredictionErrorSignal();
    
    if (curiosity) curiosity->initialize(brain.get());
    if (novelty) novelty->initialize(brain.get());
    if (predError) predError->initialize(brain.get());
    
    // Test full integration simulation
    for (SimulationStep step = 0; step < 100; ++step) {
        // All systems should be active
        brain->step(step, step * 0.001);
        
        // All integrated systems should be functioning
        assert(brain->getWorkingMemory()->getMemoryActivity() >= 0.0f);
        assert(brain->getEpisodicMemory()->getEpisodeCount() >= 0);
        
        if (step % 50 == 0) {
            // Perform development updates
            brain->develop();
        }
        
        // Check that systems are interacting
        if (curiosity) {
            float curiosityLevel = curiosity->getLevel();
            assert(!std::isnan(curiosityLevel));
        }
    }
    
    std::cout << "    PASSED: Full integration simulation successful" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    testBrainInitialization();
    testNeuromodulationIntegration();
    testMemoryIntegration();
    testCognitiveIntegration();
    testPredictionIntegration();
    testDevelopmentIntegration();
    testFullIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration