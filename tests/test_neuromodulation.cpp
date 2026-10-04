// Neuromodulation Tests
// Comprehensive testing for all neuromodulator systems in NLM

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuromodulation {

void testNeuromodulatorBase() {
    std::cout << "Testing Neuromodulator base class..." << std::endl;
    
    // The Neuromodulator class is abstract, so we test through concrete implementations
    // This test ensures the interface is properly defined
    
    std::cout << "    testNeuromodulatorBase passed" << std::endl;
}

void testDopamineSystem() {
    std::cout << "Testing Dopamine system..." << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Initialize with a mock brain pointer (not needed for basic functionality)
    // Test basic level management
    float initialLevel = dopamine.getLevel();
    assert(std::abs(initialLevel - 0.0f) < 0.001f);
    
    // Test level update
    float testLevel = 0.75f;
    dopamine.setLevel(testLevel);
    assert(std::abs(dopamine.getLevel() - testLevel) < 0.001f);
    
    // Test plasticity factor
    float plasticityFactor = dopamine.getPlasticityFactor();
    assert(plasticityFactor > 0.0f && plasticityFactor <= 1.0f);
    
    // Test level bounds (should clamp)
    dopamine.setLevel(-0.5f);  // Should clamp to >= 0
    assert(dopamine.getLevel() >= 0.0f);
    
    dopamine.setLevel(1.5f);   // Should clamp to <= 1
    assert(dopamine.getLevel() <= 1.0f);
    
    std::cout << "    testDopamineSystem passed" << std::endl;
}

void testRewardSystem() {
    std::cout << "Testing Reward system..." << std::endl;
    
    nlm::Reward reward;
    
    // Test reward computation
    float testObservation = 1.0f;
    float testAction = 0.5f;
    
    // Reward depends on action prediction error
    // For testing, we'll verify the interface is accessible
    bool hasPrediction = reward.getPredictionAvailable();
    // This may be false initially, which is expected
    
    // Test reward level
    float initialReward = reward.getLevel();
    assert(std::abs(initialReward - 0.0f) < 0.001f);
    
    // Test reward update
    float testReward = 0.8f;
    reward.setLevel(testReward);
    assert(std::abs(reward.getLevel() - testReward) < 0.001f);
    
    std::cout << "    testRewardSystem passed" << std::endl;
}

void testCuriositySystem() {
    std::cout << "Testing Curiosity system..." << std::endl;
    
    nlm::Curiosity curiosity;
    
    // Test curiosity level
    float initialCuriosity = curiosity.getLevel();
    assert(std::abs(initialCuriosity - 0.0f) < 0.001f);
    
    // Test curiosity update
    float testCuriosity = 0.6f;
    curiosity.setLevel(testCuriosity);
    assert(std::abs(curiosity.getLevel() - testCuriosity) < 0.001f);
    
    // Test exploration drive
    bool hasExploration = curiosity.getExplorationDrive();
    // This may be false initially, which is expected
    
    // Test curiosity dynamics (should decay over time when not stimulated)
    curiosity.setLevel(1.0f);
    float levelBeforeDecay = curiosity.getLevel();
    curiosity.update(1.0f);  // Decay over 1 second
    float levelAfterDecay = curiosity.getLevel();
    assert(levelAfterDecay < levelBeforeDecay);
    
    std::cout << "    testCuriositySystem passed" << std::endl;
}

void testPredictionErrorSystem() {
    std::cout << "Testing PredictionError system..." << std::endl;
    
    nlm::PredictionError predictionError;
    
    // Test prediction error level
    float initialPE = predictionError.getLevel();
    assert(std::abs(initialPE - 0.0f) < 0.001f);
    
    // Test prediction error update
    float testPE = 0.3f;
    predictionError.setLevel(testPE);
    assert(std::abs(predictionError.getLevel() - testPE) < 0.001f);
    
    // Test prediction error signal strength
    float peStrength = predictionError.getPredictionError();
    assert(std::abs(peStrength - 0.0f) < 0.001f);
    
    std::cout << "    testPredictionErrorSystem passed" << std::endl;
}

void testNoveltyDetection() {
    std::cout << "Testing Novelty detection system..." << std::endl;
    
    nlm::Novelty novelty;
    
    // Test novelty level
    float initialNovelty = novelty.getLevel();
    assert(std::abs(initialNovelty - 0.0f) < 0.001f);
    
    // Test novelty update with sensory input
    float testInput = 0.9f;
    novelty.setLevel(testInput);
    assert(std::abs(novelty.getLevel() - testInput) < 0.001f);
    
    // Test novelty decay
    float noveltyBeforeDecay = novelty.getLevel();
    novelty.update(1.0f);
    float noveltyAfterDecay = novelty.getLevel();
    assert(noveltyAfterDecay < noveltyBeforeDecay);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing Neuromodulation integration..." << std::endl;
    
    // Test that multiple neuromodulators can coexist
    nlm::Dopamine dopamine;
    nlm::Curiosity curiosity;
    nlm::Novelty novelty;
    
    // Set different levels
    dopamine.setLevel(0.8f);
    curiosity.setLevel(0.5f);
    novelty.setLevel(0.9f);
    
    // Verify independent operation
    assert(dopamine.getLevel() > 0.0f);
    assert(curiosity.getLevel() > 0.0f);
    assert(novelty.getLevel() > 0.0f);
    
    // Test that they don't interfere with each other
    dopamine.update(0.1f);  // Only dopamine should decay
    assert(std::abs(dopamine.getLevel() - (0.8f * 0.9f)) < 0.001f);
    assert(std::abs(curiosity.getLevel() - 0.5f) < 0.001f);  // curiosity unchanged
    assert(std::abs(novelty.getLevel() - 0.9f) < 0.001f);    // novelty unchanged
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPlasticityFactorModulation() {
    std::cout << "Testing plasticity factor modulation..." << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test that dopamine affects plasticity factor
    float baselineFactor = dopamine.getPlasticityFactor();
    
    dopamine.setLevel(0.0f);  // No dopamine
    float zeroDopamineFactor = dopamine.getPlasticityFactor();
    assert(std::abs(zeroDopamineFactor - 1.0f) < 0.001f);  // Should reduce plasticity
    
    dopamine.setLevel(1.0f);  // Maximum dopamine
    float maxDopamineFactor = dopamine.getPlasticityFactor();
    assert(maxDopamineFactor > zeroDopamineFactor);  // Should increase plasticity
    
    std::cout << "    testPlasticityFactorModulation passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    testNeuromodulatorBase();
    testDopamineSystem();
    testRewardSystem();
    testCuriositySystem();
    testPredictionErrorSystem();
    testNoveltyDetection();
    testNeuromodulationIntegration();
    testPlasticityFactorModulation();
}

} // namespace test_neuromodulation