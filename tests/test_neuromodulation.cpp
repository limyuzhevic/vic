#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_neuromodulation {

void testNeuromodulatorCreation() {
    nlm::Neuromodulator neuromodulator;
    assert(neuromodulator.getCurrentValue() == 0.0f);
    assert(neuromodulator.isActive() == false);
    std::cout << "    testNeuromodulatorCreation passed" << std::endl;
}

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    assert(dopamine.getCurrentValue() == 0.0f);
    assert(dopamine.getPredictionError() == 0.0f);
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    assert(curiosity.getNovelty() == 0.0f);
    assert(curiosity.getCuriosity() == 0.0f);
    assert(curiosity.isAdaptive() == false);
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testNoveltyCreation() {
    nlm::Novelty novelty;
    assert(novelty.getNoveltyScore() == 0.0f);
    assert(novelty.getThreshold() == 0.5f);
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testPredictionErrorCreation() {
    nlm::PredictionError error;
    assert(error.getPredictionError() == 0.0f);
    assert(error.getMagnitude() == 0.0f);
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testNeuromodulatorUpdate() {
    nlm::Dopamine dopamine;
    dopamine.update(0.1f, 0.05f, 0.01f);
    assert(dopamine.getCurrentValue() > 0.0f);
    
    nlm::Novelty novelty;
    novelty.update(0.5f, 0.3f, 0.1f);
    assert(novelty.getNoveltyScore() > 0.5f);
    std::cout << "    testNeuromodulatorUpdate passed" << std::endl;
}

void testNeuromodulatorReset() {
    nlm::Dopamine dopamine;
    dopamine.update(0.2f, 0.1f, 0.05f);
    dopamine.reset();
    assert(dopamine.getCurrentValue() == 0.0f);
    std::cout << "    testNeuromodulatorReset passed" << std::endl;
}

void testNeuromodulatorBounds() {
    nlm::Dopamine dopamine;
    
    // Test upper bound
    dopamine.update(10.0f, 0.0f, 0.0f);
    assert(dopamine.getCurrentValue() <= 1.0f);
    
    // Test lower bound  
    dopamine.update(-10.0f, 0.0f, 0.0f);
    assert(dopamine.getCurrentValue() >= -1.0f);
    std::cout << "    testNeuromodulatorBounds passed" << std::endl;
}

void testNoveltyThreshold() {
    nlm::Novelty novelty;
    assert(novelty.getThreshold() == 0.5f);
    
    novelty.setThreshold(0.8f);
    assert(novelty.getThreshold() == 0.8f);
    
    novelty.setThreshold(0.0f);
    assert(novelty.getThreshold() == 0.0f);
    std::cout << "    testNoveltyThreshold passed" << std::endl;
}

void testPredictionErrorUpdate() {
    nlm::PredictionError error;
    
    // Positive prediction error
    error.update(0.5f, 0.1f);
    assert(error.getPredictionError() > 0.0f);
    
    // Negative prediction error
    error.update(-0.3f, 0.1f);
    assert(error.getPredictionError() < 0.0f);
    
    error.reset();
    assert(error.getPredictionError() == 0.0f);
    std::cout << "    testPredictionErrorUpdate passed" << std::endl;
}

void testNeuromodulatorNames() {
    assert(std::string(nlm::Neuromodulator::getName(nlm::NeuromodulatorType::Dopamine)) == "Dopamine");
    assert(std::string(nlm::Neuromodulator::getName(nlm::NeuromodulatorType::Curiosity)) == "Curiosity");
    assert(std::string(nlm::Neuromodulator::getName(nlm::NeuromodulatorType::Novelty)) == "Novelty");
    assert(std::string(nlm::Neuromodulator::getName(nlm::NeuromodulatorType::Reward)) == "Reward");
    assert(std::string(nlm::Neuromodulator::getName(nlm::NeuromodulatorType::PredictionError)) == "PredictionError");
    std::cout << "    testNeuromodulatorNames passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    testNeuromodulatorCreation();
    testDopamineCreation();
    testCuriosityCreation();
    testNoveltyCreation();
    testPredictionErrorCreation();
    testNeuromodulatorUpdate();
    testNeuromodulatorReset();
    testNeuromodulatorBounds();
    testNoveltyThreshold();
    testPredictionErrorUpdate();
    testNeuromodulatorNames();
}

} // namespace test_neuromodulation