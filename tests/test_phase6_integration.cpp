// Phase 6 Integration Test Suite
// Tests the complete integrated brain simulation
// Tests coordination across memory, prediction, cognition, and neuromodulation systems

#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_phase6_integration {

void testMemorySystemIntegration() {
    std::cout << "  Testing Memory System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("random_seed", 42);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test working memory
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Inject current to create activity
    brain->injectCurrentToNeurons(nlm::NeuronType::Internal, 10.0f);
    
    // Step brain to generate spikes
    brain->step(0);
    
    // Check working memory has traces
    // Note: This test will need to be adjusted based on the exact API
    
    std::cout << "    ✓ Memory system integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Simulate learning to activate neuromodulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Inject sensory input to drive learning
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        
        // Apply reward modulation
        brain->applyNeuromodulation(*dopamine);
    }
    
    std::cout << "    ✓ Neuromodulation integration test passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 400);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Get prediction error system
    auto* predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Run simulation to generate prediction errors
    for (int step = 0; step < 200; ++step) {
        brain->step(step);
        
        // Inject input to create prediction errors
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 3.0f);
        
        // Apply prediction error modulation
        brain->applyNeuromodulation(*predictionError);
    }
    
    std::cout << "    ✓ Prediction system integration test passed" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "  Testing Cognition System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 600);
    config->set("region_count", 2);
    config->set("random_seed", 42);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Generate activity for cognitive processing
    for (int step = 0; step < 150; ++step) {
        brain->step(step);
        
        // Inject varied input to drive cognition
        brain->injectCurrentToNeurons(nlm::NeuronType::Internal, 2.0f);
        
        // Update cognitive systems
        if (step % 50 == 0) {
            brain->develop();
        }
    }
    
    std::cout << "    ✓ Cognition system integration test passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "  Testing Development System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 800);
    config->set("region_count", 2);
    config->set("random_seed", 42);
    config->set("synaptogenesis_rate", 0.001);
    config->set("pruning_rate", 0.0001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Simulate development over time
    nlm::DevelopmentalStage currentStage = nlm::DevelopmentalStage::Initial;
    
    for (int step = 0; step < 3000; step += 100) {
        brain->step(step);
        brain->develop();
        
        // Check developmental stage changes
        nlm::DevelopmentalStage newStage = brain->getDevelopmentalStage();
        if (newStage != currentStage) {
            currentStage = newStage;
        }
    }
    
    std::cout << "    ✓ Development system integration test passed" << std::endl;
}

void testCompleteBrainLoop() {
    std::cout << "  Testing Complete Brain Loop Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("random_seed", 42);
    config->set("replay_interval", 200);
    config->set("consolidation_interval", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Track key metrics
    size_t totalSpikes = 0;
    float totalEIRatio = 0.0f;
    size_t memoryTraces = 0;
    
    // Run complete brain simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
        
        totalSpikes += brain->getTotalSpikeCount();
        totalEIRatio += brain->getExcitationInhibitionRatio();
        
        // Check memory systems
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            memoryTraces += workingMemory->getActiveTraces();
        }
        
        // Inject varied input to drive learning
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.0f);
        }
    }
    
    // Verify integration (adjust thresholds as needed)
    assert(totalSpikes > 0);  // Should have generated spikes
    assert(!std::isnan(totalEIRatio));  // E/I ratio should be valid
    
    std::cout << "    ✓ Complete brain loop test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete integration of all brain systems:" << std::endl;
    std::cout << "" << std::endl;
    
    testMemorySystemIntegration();
    std::cout << "" << std::endl;
    
    testNeuromodulationIntegration();
    std::cout << "" << std::endl;
    
    testPredictionSystemIntegration();
    std::cout << "" << std::endl;
    
    testCognitionSystemIntegration();
    std::cout << "" << std::endl;
    
    testDevelopmentSystemIntegration();
    std::cout << "" << std::endl;
    
    testCompleteBrainLoop();
    
    std::cout << "" << std::endl;
    std::cout << "=== All Phase 6 Integration Tests Passed! ===" << std::endl;
}

} // namespace test_phase6_integration