// Test for Neuromodulation System Integration (Dopamine, Curiosity, Novelty)
// Covers requirement: 4. Neuromodulation System Tests

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuromodulation_system {

void testNeuromodulatorBasic() {
    nlm::Neuromodulator neuromodulator;
    
    // Test initial state
    assert(neuromodulator.getValue() == 0.0f);
    assert(neuromodulator.getEffectMagnitude() == 1.0f);
    
    std::cout << "    testNeuromodulatorBasic passed" << std::endl;
}

void testRewardSystem() {
    nlm::Reward reward;
    
    // Test initial state
    assert(reward.getCumulativeReward() == 0.0f);
    assert(reward.getRecentReward() == 0.0f);
    
    // Add reward
    reward.addReward(1.0f);
    assert(reward.getCumulativeReward() == 1.0f);
    assert(reward.getRecentReward() == 1.0f);
    
    // Add more reward
    reward.addReward(2.5f);
    assert(reward.getCumulativeReward() == 3.5f);
    
    // Get decay factor
    float decay = reward.getDecayFactor();
    assert(std::isfinite(decay));
    
    std::cout << "    testRewardSystem passed" << std::endl;
}

void testCuriositySystem() {
    nlm::Curiosity curiosity;
    
    // Test initial state
    assert(curiosity.getCuriosityLevel() == 0.0f);
    assert(curiosity.getExplorationRate() == 0.0f);
    
    // Stimulate curiosity
    curiosity.triggerCuriosity();
    assert(curiosity.getCuriosityLevel() > 0.0f);
    assert(curiosity.getExplorationRate() > 0.0f);
    
    // Decay curiosity
    curiosity.decay(0.1f);
    assert(curiosity.getCuriosityLevel() >= 0.0f);
    
    // Reset
    curiosity.reset();
    assert(curiosity.getCuriosityLevel() == 0.0f);
    
    std::cout << "    testCuriositySystem passed" << std::endl;
}

void testNoveltyDetection() {
    nlm::Novelty novelty;
    
    // Test initial state
    assert(novelty.getNoveltyLevel() == 0.0f);
    assert(novelty.getNoveltyThreshold() == 0.5f);
    
    // Detect novelty in neuron activity
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.injectCurrent(5.0f); // High activity
    
    bool isNovel = novelty.detectNovelty(neuron.getTotalCurrent());
    assert(isNovel); // Should detect novelty with high activity
    
    assert(novelty.getNoveltyLevel() > 0.0f);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testPredictionErrorSystem() {
    nlm::PredictionError predictionError;
    
    // Test initial state
    assert(predictionError.getPredictionError() == 0.0f);
    assert(predictionError.getPEThreshold() == 0.5f);
    
    // Set prediction error
    predictionError.setPredictionError(0.8f);
    assert(predictionError.getPredictionError() == 0.8f);
    
    // Test error effect
    float errorEffect = predictionError.getErrorEffect();
    assert(std::isfinite(errorEffect));
    
    std::cout << "    testPredictionErrorSystem passed" << std::endl;
}

void testNeuromodulationIntegration() {
    nlm::Neuromodulator neuromodulator;
    nlm::Reward reward;
    
    // Set reward
    reward.addReward(1.0f);
    
    // Neuromodulator should respond to reward
    neuromodulator.setValue(reward.getCumulativeReward());
    assert(neuromodulator.getValue() == 1.0f);
    
    // Test interaction between neuromodulators
    nlm::Curiosity curiosity;
    curiosity.triggerCuriosity();
    
    // Both should be active
    assert(curiosity.getCuriosityLevel() > 0.0f);
    assert(neuromodulator.getValue() > 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void runAll() {
    testNeuromodulatorBasic();
    testRewardSystem();
    testCuriositySystem();
    testNoveltyDetection();
    testPredictionErrorSystem();
    testNeuromodulationIntegration();
}

} // namespace test_neuromodulation_system
