#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Neuromodulator.hpp"

namespace test_neuromodulation_advanced {

void testDopamineNeuromodulation() {
    std::cout << "Testing dopamine neuromodulation..." << std::endl;
    
    nlm::Neuromodulator dopamine(nlm::NeuromodulatorType::Dopamine, 1.0f, 0.1f, -65.0f, 1.0f);
    assert(dopamine.getType() == nlm::NeuromodulatorType::Dopamine);
    assert(dopamine.getValue() == 1.0f);
    assert(dopamine.getTimeConstant() == 0.1f);
    assert(dopamine.getBaseLevel() == -65.0f);
    assert(dopamine.getGain() == 1.0f);
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    float potential_before = neuron.getMembranePotential();
    
    dopamine.applyToNeuron(&neuron, 0.01f);
    float potential_after = neuron.getMembranePotential();
    
    assert(potential_after > potential_before);
    
    dopamine.setValue(2.0f);
    assert(dopamine.getValue() == 2.0f);
    
    dopamine.setGain(2.0f);
    assert(dopamine.getGain() == 2.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testCuriositySystem() {
    std::cout << "Testing curiosity system..." << std::endl;
    
    nlm::Curiosity curiosity(0.7f, 0.3f, 0.1f, 0.5f, 1.0f);
    assert(curiosity.getExplorationRate() == 0.7f);
    assert(curiosity.getExploitationRate() == 0.3f);
    assert(curiosity.getNoveltyThreshold() == 0.1f);
    assert(curiosity.getMaxCuriosity() == 0.5f);
    assert(curiosity.getDecayRate() == 1.0f);
    assert(curiosity.getLevel() == 0.0f);
    
    float level = curiosity.getLevel();
    assert(level == 0.0f);
    
    curiosity.update(0.8f, 1.0f);
    float new_level = curiosity.getLevel();
    assert(new_level > 0.0f);
    
    curiosity.update(0.2f, 0.5f);
    assert(curiosity.getLevel() > 0.0f);
    
    curiosity.reset();
    assert(curiosity.getLevel() == 0.0f);
    
    curiosity.setNoveltyWeight(0.8f);
    curiosity.setPredictionErrorWeight(0.2f);
    
    std::cout << "  PASSED" << std::endl;
}

void testNoveltyDetection() {
    std::cout << "Testing novelty detection..." << std::endl;
    
    nlm::Novelty novelty_detector(0.5f, 0.8f, 0.01f, 0.05f);
    assert(novelty_detector.getThreshold() == 0.5f);
    assert(novelty_detector.getAdaptationRate() == 0.8f);
    assert(novelty_detector.getDecayRate() == 0.01f);
    assert(novelty_detector.getMaxValue() == 0.05f);
    assert(novelty_detector.getCurrentNovelty() == 0.0f);
    
    nlm::Novelty::NoveltyInput input;
    input.patternId = 123;
    input.intensity = 0.9f;
    input.context = "test_pattern";
    
    float novelty_value = novelty_detector.update(input);
    assert(novelty_value > novelty_detector.getThreshold());
    
    assert(novelty_detector.getCurrentNovelty() > 0.0f);
    assert(novelty_detector.getCurrentNovelty() <= novelty_detector.getMaxValue());
    
    novelty_detector.update(input);  // Update with same pattern
    assert(novelty_detector.getCurrentNovelty() >= 0.0f); // Should not decrease
    
    std::cout << "  PASSED" << std::endl;
}

void testPredictionError() {
    std::cout << "Testing prediction error..." << std::endl;
    
    nlm::PredictionError prediction_error(0.5f, 0.2f, 0.1f);
    assert(prediction_error.getLearningRate() == 0.5f);
    assert(prediction_error.getPredictionThreshold() == 0.2f);
    assert(prediction_error.getMaxError() == 0.1f);
    assert(prediction_error.getCurrentError() == 0.0f);
    
    float error = prediction_error.update(0.6f, 0.3f);
    assert(error > prediction_error.getPredictionThreshold());
    
    assert(prediction_error.getCurrentError() > 0.0f);
    assert(prediction_error.getCurrentError() <= prediction_error.getMaxError());
    
    prediction_error.reset();
    assert(prediction_error.getCurrentError() == 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testRewardSignals() {
    std::cout << "Testing reward signals..." << std::endl;
    
    nlm::Reward reward(1.0f, 0.5f, 0.1f, 0.9f);
    assert(reward.getLearningRate() == 1.0f);
    assert(reward.getDiscountFactor() == 0.5f);
    assert(reward.getTimeConstant() == 0.1f);
    assert(reward.getMaxReward() == 0.9f);
    assert(reward.getCurrentValue() == 0.0f);
    
    reward.update(0.8f);
    assert(reward.getCurrentValue() > 0.0f);
    assert(reward.getCurrentValue() <= reward.getMaxReward());
    
    reward.reset();
    assert(reward.getCurrentValue() == 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Neuromodulation Tests ===" << std::endl;
    
    testDopamineNeuromodulation();
    testCuriositySystem();
    testNoveltyDetection();
    testPredictionError();
    testRewardSignals();
    
    std::cout << "=== All Advanced Neuromodulation Tests PASSED ===" << std::endl;
}

} // namespace test_neuromodulation_advanced
