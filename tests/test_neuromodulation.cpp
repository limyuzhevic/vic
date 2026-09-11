// test_neuromodulation.cpp - Comprehensive neuromodulation system tests

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>

using namespace nlm;

void testDopamineSystem() {
    std::cout << "Testing Dopamine system..." << std::endl;
    
    Dopamine dopamine;
    
    // Test initial state
    assert(dopamine.getBaselineLevel() == 0.0f);
    assert(dopamine.getCurrentLevel() == 0.0f);
    
    // Test reward response
    dopamine.onReward(1.0f);
    float level = dopamine.getCurrentLevel();
    assert(level > 0.0f && level < 1.0f);  // Should be elevated
    
    // Test decay over time
    float level1 = dopamine.getCurrentLevel();
    dopamine.update(0.1f);  // Small time step
    float level2 = dopamine.getCurrentLevel();
    assert(level2 < level1);  // Should decay
    
    // Test prediction error modulation
    dopamine.onPredictionError(0.5f);
    assert(dopamine.getPredictionError() > 0.0f);
    
    std::cout << "  ✓ Dopamine system tests passed" << std::endl;
}

void testCuriositySystem() {
    std::cout << "Testing Curiosity system..." << std::endl;
    
    Curiosity curiosity;
    
    // Test initial state
    assert(curiosity.getCuriosityLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    // Test curiosity response to novelty
    curiosity.onNovelty(0.8f);
    assert(curiosity.getCuriosityLevel() > 0.0f);
    
    // Test exploration behavior
    bool shouldExplore = curiosity.shouldExplore();
    assert(shouldExplore == true);  // Should explore when curiosity is high
    
    // Test decay
    curiosity.update(0.1f);
    assert(curiosity.getCuriosityLevel() < 0.8f);
    
    // Test sustained curiosity
    for (int i = 0; i < 10; ++i) {
        curiosity.onNovelty(0.1f);
        curiosity.update(0.1f);
    }
    assert(curiosity.getCuriosityLevel() > 0.0f);  // Should maintain baseline
    
    std::cout << "  ✓ Curiosity system tests passed" << std::endl;
}

void testNoveltySystem() {
    std::cout << "Testing Novelty system..." << std::endl;
    
    Novelty novelty;
    
    // Test novelty detection with similar inputs
    NoveltyInput input1;
    input1.featureVector = {0.1f, 0.2f, 0.3f};
    input1.similarityToRecent = 0.9f;  // Very similar to recent experience
    
    NoveltyInput input2;
    input2.featureVector = {0.8f, 0.7f, 0.6f};
    input2.similarityToRecent = 0.1f;  // Very different
    
    bool novelty1 = novelty.isNovel(input1);
    bool novelty2 = novelty.isNovel(input2);
    
    assert(novelty1 == false);  // Not novel
    assert(novelty2 == true);   // Novel
    
    // Test novelty response
    novelty.onNovelty(input2);
    assert(novelty.getNoveltyLevel() > 0.0f);
    
    // Test novelty memory
    size_t memorySize = novelty.getMemorySize();
    assert(memorySize > 0);
    
    std::cout << "  ✓ Novelty system tests passed" << std::endl;
}

void testPredictionErrorSystem() {
    std::cout << "Testing Prediction Error system..." << std::endl;
    
    PredictionError predError;
    
    // Test initial state
    assert(predError.getPredictionError() == 0.0f);
    assert(predError.getConfidence() == 1.0f);  // Full confidence initially
    
    // Test prediction error generation
    predError.onUnexpectedOutcome(0.5f);
    assert(predError.getPredictionError() > 0.0f);
    
    // Test confidence adjustment
    predError.onExpectedOutcome();
    assert(predError.getConfidence() < 1.0f);
    
    // Test learning rate modulation
    float learningRate = predError.getLearningRate();
    assert(learningRate > 0.0f && learningRate <= 1.0f);
    
    // Test prediction error accumulation
    predError.onUnexpectedOutcome(0.3f);
    float error1 = predError.getPredictionError();
    predError.onUnexpectedOutcome(0.4f);
    float error2 = predError.getPredictionError();
    assert(error2 > error1);  // Should accumulate
    
    std::cout << "  ✓ Prediction Error system tests passed" << std::endl;
}

void testNeuromodulatorBase() {
    std::cout << "Testing Neuromodulator base class..." << std::endl;
    
    Neuromodulator mod;
    
    // Test basic neuromodulator functionality
    assert(mod.getName() == "Neuromodulator");
    assert(mod.isActive() == true);
    
    // Test value changes
    float initial = mod.getValue();
    mod.update(0.1f);
    assert(mod.getValue() != initial);
    
    // Test influence calculation
    float influence = mod.calculateInfluence(1.0f);
    assert(influence >= 0.0f);
    
    std::cout << "  ✓ Neuromodulator base class tests passed" << std::endl;
}

void runAllNeuromodulationTests() {
    std::cout << "=== Neuromodulation System Tests ===" << std::endl << std::endl;
    
    testNeuromodulatorBase();
    testDopamineSystem();
    testCuriositySystem();
    testNoveltySystem();
    testPredictionErrorSystem();
    
    std::cout << std::endl;
    std::cout << "✅ All neuromodulation system tests passed!" << std::endl;
    std::cout << "Neuromodulation systems are properly integrated and functional." << std::endl;
}

int main() {
    try {
        runAllNeuromodulationTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}