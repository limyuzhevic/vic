// Neuromodulation System Tests
// Testing dopamine, curiosity, novelty, and prediction error systems

#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

void testDopamine() {
    std::cout << "    Testing dopamine system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test initialization
    dopamine->initialize(brain.get());
    
    // Test level manipulation
    dopamine->setLevel(0.5f);
    float level = dopamine->getLevel();
    assert(level == 0.5f);
    
    // Test plasticity factor
    float plasticity = dopamine->getPlasticityFactor();
    assert(plasticity > 0.0f);
    
    std::cout << "      ✓ Dopamine system tests passed" << std::endl;
}

void testCuriosity() {
    std::cout << "    Testing curiosity system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test initialization
    curiosity->initialize(brain.get());
    
    // Test curiosity level
    curiosity->setCuriosity(0.3f);
    float curiosityLevel = curiosity->getCuriosityLevel();
    assert(curiosityLevel == 0.3f);
    
    // Test exploration bonus
    float exploration = curiosity->getExplorationBonus();
    assert(exploration > 0.0f);
    
    std::cout << "      ✓ Curiosity system tests passed" << std::endl;
}

void testNovelty() {
    std::cout << "    Testing novelty system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    // Test initialization
    novelty->initialize(brain.get());
    
    // Test novelty detection
    novelty->update(0.001);
    float noveltyLevel = novelty->getNoveltyLevel();
    assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
    
    std::cout << "      ✓ Novelty system tests passed" << std::endl;
}

void testPredictionError() {
    std::cout << "    Testing prediction error system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test initialization
    predictionError->initialize(brain.get());
    
    // Test prediction error level
    predictionError->update(0.001);
    float errorLevel = predictionError->getErrorLevel();
    assert(errorLevel >= 0.0f && errorLevel <= 1.0f);
    
    // Test prediction error direction (positive for better than expected, negative for worse)
    float direction = predictionError->getErrorDirection();
    assert(direction == 0.0f || direction == 1.0f || direction == -1.0f);
    
    std::cout << "      ✓ Prediction error system tests passed" << std::endl;
}

void testNeuromodulatorBase() {
    std::cout << "    Testing neuromodulator base class..." << std::endl;
    
    nlm::Neuromodulator neuromodulator;
    
    // Test initialization
    neuromodulator.initialize(nullptr);
    
    // Test level manipulation
    neuromodulator.setLevel(0.7f);
    float level = neuromodulator.getLevel();
    assert(level == 0.7f);
    
    // Test decay
    neuromodulator.update(0.1f);
    float decayedLevel = neuromodulator.getLevel();
    assert(decayedLevel <= level);
    
    std::cout << "      ✓ Neuromodulator base class tests passed" << std::endl;
}

void runAll() {
    std::cout << "=== Neuromodulation System Tests ===" << std::endl;
    
    testDopamine();
    testCuriosity();
    testNovelty();
    testPredictionError();
    testNeuromodulatorBase();
    
    std::cout << "=== All Neuromodulation Tests PASSED ===" << std::endl;
}

} // namespace test_neuromodulation
