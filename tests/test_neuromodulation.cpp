// Neuromodulation System Tests
#include "brain/Neuromodulation/DopamineSystem.hpp"
#include "brain/Neuromodulation/CuriositySystem.hpp"
#include "brain/Neuromodulation/NoveltySystem.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

void testDopamineSystem() {
    std::cout << "    Testing Dopamine System..." << std::endl;
    
    nlm::DopamineSystem dopamine;
    
    // Test initial state
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getAveragedLevel() == 0.0f);
    
    // Test level updates
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.update(0.01f); // Time step
    assert(dopamine.getLevel() < 0.5f); // Should decay
    
    // Test prediction error response
    dopamine.processPredictionError(0.3f);
    assert(dopamine.getLevel() > 0.5f); // Should increase
    
    // Test reward prediction
    dopamine.processRewardPrediction(0.8f);
    assert(dopamine.getAveragedLevel() > 0.5f);
    
    std::cout << "    testDopamineSystem passed" << std::endl;
}

void testCuriositySystem() {
    std::cout << "    Testing Curiosity System..." << std::endl;
    
    nlm::CuriositySystem curiosity;
    
    // Test initial state
    assert(curiosity.getCuriosityLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    // Test curiosity updates
    curiosity.setCuriosityLevel(0.7f);
    assert(curiosity.getCuriosityLevel() == 0.7f);
    
    curiosity.update(0.01f);
    // Should maintain or modify based on implementation
    
    // Test novel state detection
    curiosity.processNovelState(true);
    assert(curiosity.getCuriosityLevel() > 0.7f);
    
    curiosity.processNovelState(false);
    assert(curiosity.getCuriosityLevel() == 0.0f); // Should reset
    
    // Test exploration drive
    curiosity.processRewardPrediction(0.5f);
    assert(curiosity.getExplorationDrive() > 0.0f);
    
    std::cout << "    testCuriositySystem passed" << std::endl;
}

void testNoveltySystem() {
    std::cout << "    Testing Novelty System..." << std::endl;
    
    nlm::NoveltySystem novelty;
    
    // Test initial state
    assert(novelty.getNoveltyLevel() == 0.0f);
    assert(novelty.getAttentionWeight() == 1.0f);
    
    // Test novelty updates
    novelty.setNoveltyLevel(0.6f);
    assert(novelty.getNoveltyLevel() == 0.6f);
    
    novelty.update(0.01f);
    // Should decay over time
    
    // Test stimulus processing
    std::vector<float> stimulus = {0.1f, 0.5f, 0.9f, 0.2f, 0.8f};
    novelty.processStimulus(stimulus);
    assert(novelty.getNoveltyLevel() > 0.6f); // Should increase with high variance
    
    // Test attention modulation
    novelty.processAttentionWeight(0.5f);
    assert(novelty.getAttentionWeight() == 0.5f);
    
    // Reset and test decrease
    novelty.setNoveltyLevel(0.0f);
    novelty.update(0.01f);
    assert(novelty.getNoveltyLevel() == 0.0f);
    
    std::cout << "    testNoveltySystem passed" << std::endl;
}

void testMemoryNeuromodulation() {
    std::cout << "    Testing Memory-Neuromodulation Integration..." << std::endl;
    
    nlm::DopamineSystem dopamine;
    nlm::CuriositySystem curiosity;
    
    // Simulate reward prediction error affecting memory consolidation
    dopamine.processPredictionError(0.4f);
    curiosity.processNovelState(true);
    
    // Both systems should show increased activity
    assert(dopamine.getLevel() > 0.0f);
    assert(curiosity.getCuriosityLevel() > 0.0f);
    
    // Test novelty affecting curiosity
    curiosity.processNovelState(false); // Reset
    curiosity.processNovelState(true);
    assert(curiosity.getCuriosityLevel() > 0.0f);
    
    // Test dopamine-reward interaction
    dopamine.processRewardPrediction(0.7f);
    assert(dopamine.getAveragedLevel() > 0.0f);
    
    std::cout << "    testMemoryNeuromodulation passed" << std::endl;
}

void runAll() {
    testDopamineSystem();
    testCuriositySystem();
    testNoveltySystem();
    testMemoryNeuromodulation();
}

} // namespace test_neuromodulation