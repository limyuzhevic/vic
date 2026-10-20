// Neuromodulation System Tests
// Tests for dopamine, curiosity, novelty, prediction error, etc.

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

void testDopamineSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Dopamine dopamine(config);
    
    // Test basic operations
    assert(dopamine.getBaseline() == 0.0f);
    assert(dopamine.getScale() > 0.0f);
    
    // Test reward modulation
    float reward = 1.0f;
    dopamine.processReward(reward);
    
    assert(dopamine.getLevel() >= 0.0f);
    
    std::cout << "    testDopamineSystem passed" << std::endl;
}

void testCuriositySystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Curiosity curiosity(config);
    
    // Test basic operations
    assert(curiosity.getBaseline() == 0.0f);
    assert(curiosity.getSensitivity() > 0.0f);
    
    // Test curiosity increase
    curiosity.increaseCuriosity(0.5f);
    assert(curiosity.getLevel() > 0.0f);
    
    // Test curiosity decay
    curiosity.decayCuriosity();
    assert(curiosity.getLevel() >= 0.0f);
    
    std::cout << "    testCuriositySystem passed" << std::endl;
}

void testNoveltySystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Novelty novelty(config);
    
    // Test basic operations
    assert(novelty.getBaseline() == 0.0f);
    assert(novelty.getSensitivity() > 0.0f);
    
    // Test novelty detection
    novelty.detectNovelty(0.8f);
    assert(novelty.getLevel() > 0.0f);
    
    std::cout << "    testNoveltySystem passed" << std::endl;
}

void testPredictionErrorSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::PredictionError predError(config);
    
    // Test basic operations
    assert(predError.getBaseline() == 0.0f);
    assert(predError.getSensitivity() > 0.0f);
    
    // Test prediction error calculation
    float predicted = 0.7f;
    float actual = 0.9f;
    predError.calculateError(predicted, actual);
    
    assert(predError.getError() >= 0.0f);
    
    std::cout << "    testPredictionErrorSystem passed" << std::endl;
}

void runAll() {
    testDopamineSystem();
    testCuriositySystem();
    testNoveltySystem();
    testPredictionErrorSystem();
}

} // namespace test_neuromodulation