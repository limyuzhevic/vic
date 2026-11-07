// Dopamine Neuromodulator Tests
#include "neuromodulation/Dopamine.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(std::string(dopamine.getName()) == "Dopamine");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f);  // Default with zero level
    assert(dopamine.getLearningRate() == 0.01f);  // Default
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineLevel() {
    nlm::Dopamine dopamine;
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(-0.2f);
    assert(dopamine.getLevel() == -0.2f);
    
    std::cout << "    testDopamineLevel passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // With zero level, plasticity factor should be 1.0
    assert(dopamine.getPlasticityFactor() == 1.0f);
    
    // With positive level, plasticity factor should increase
    dopamine.setLevel(0.5f);
    assert(dopamine.getPlasticityFactor() == 1.0f + 0.5f * 0.5f);  // Formula from implementation
    
    // With negative level, plasticity factor should decrease
    dopamine.setLevel(-0.3f);
    assert(dopamine.getPlasticityFactor() == 1.0f + (-0.3f) * 0.5f);  // Formula from implementation
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void testDopamineSignal() {
    nlm::Dopamine dopamine;
    
    // Signal reward
    dopamine.signalReward(1.0f);
    assert(dopamine.getLevel() == 1.0f);
    
    // Signal prediction error
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() == 1.5f);
    
    std::cout << "    testDopamineSignal passed" << std::endl;
}

void testDopamineUpdate() {
    nlm::Dopamine dopamine;
    
    dopamine.setLevel(2.0f);
    dopamine.setLearningRate(0.05f);
    
    // Update should decay the level
    dopamine.update(0.1f);
    
    // The exact decay is implementation-dependent, but level should decrease
    assert(dopamine.getLevel() < 2.0f);
    
    std::cout << "    testDopamineUpdate passed" << std::endl;
}

void testDopamineConfiguration() {
    nlm::Dopamine dopamine;
    
    dopamine.setLearningRate(0.02f);
    assert(dopamine.getLearningRate() == 0.02f);
    
    std::cout << "    testDopamineConfiguration passed" << std::endl;
}

void testDopamineHistory() {
    nlm::Dopamine dopamine;
    
    // Clear history
    dopamine.clearRewardHistory();
    
    // Signal rewards
    dopamine.signalReward(1.0f);
    dopamine.signalReward(0.5f);
    
    const auto& history = dopamine.getRewardHistory();
    assert(history.size() == 2);
    assert(std::abs(history[0] - 1.0f) < 0.001f);
    assert(std::abs(history[1] - 0.5f) < 0.001f);
    
    std::cout << "    testDopamineHistory passed" << std::endl;
}

void runAll() {
    testDopamineCreation();
    testDopamineLevel();
    testDopaminePlasticityFactor();
    testDopamineSignal();
    testDopamineUpdate();
    testDopamineConfiguration();
    testDopamineHistory();
}

} // namespace test_dopamine