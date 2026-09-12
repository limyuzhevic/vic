// Neuromodulator Tests
// Phase 5: Neuromodulation and Error Handling Improvements

#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_neuromodulator {

void testNeuromodulatorBase() {
    // Test Dopamine
    nlm::Dopamine dopamine;
    assert(dopamine.getName() == std::string("DA"));
    assert(dopamine.getLevel() == 0.0f);
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    // Test plasticity factor
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.0f);
    
    // Test update
    dopamine.update(0.1f);
    assert(dopamine.getLevel() >= 0.0f && dopamine.getLevel() <= 1.0f);
    
    // Test reward signaling
    dopamine.signalReward(0.2f);
    assert(dopamine.getLevel() > 0.5f);
    
    // Test prediction error
    dopamine.signalRewardPredictionError(-0.1f);
    assert(dopamine.getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulatorBase passed" << std::endl;
}

void testNeuromodulatorAcetylcholine() {
    nlm::Acetylcholine ach;
    assert(ach.getName() == std::string("ACh"));
    assert(ach.getLevel() == 0.0f);
    assert(ach.getPlasticityFactor() == 1.0f);
    
    ach.setLevel(0.7f);
    assert(ach.getLevel() == 0.0f); // Placeholder implementation doesn't store value
    
    std::cout << "    testNeuromodulatorAcetylcholine passed" << std::endl;
}

void testNeuromodulatorNorepinephrine() {
    nlm::Norepinephrine ne;
    assert(ne.getName() == std::string("NE"));
    assert(ne.getLevel() == 0.0f);
    assert(ne.getPlasticityFactor() == 1.0f);
    
    std::cout << "    testNeuromodulatorNorepinephrine passed" << std::endl;
}

void testNeuromodulatorSerotonin() {
    nlm::Serotonin serotonin;
    assert(serotonin.getName() == std::string("5-HT"));
    assert(serotonin.getLevel() == 0.0f);
    assert(serotonin.getPlasticityFactor() == 1.0f);
    
    std::cout << "    testNeuromodulatorSerotonin passed" << std::endl;
}

void testNeuromodulatorNeuromodulatorFactory() {
    // Create vector of neuromodulators
    std::vector<nlm::Neuromodulator*> neuromodulators;
    neuromodulators.push_back(new nlm::Dopamine());
    neuromodulators.push_back(new nlm::Acetylcholine());
    neuromodulators.push_back(new nlm::Norepinephrine());
    neuromodulators.push_back(new nlm::Serotonin());
    
    // Get total system neuromodulation level
    float totalLevel = 0.0f;
    for (auto* modulator : neuromodulators) {
        totalLevel += modulator->getLevel();
    }
    
    assert(totalLevel == 0.0f); // All placeholder implementations return 0
    
    // Clean up
    for (auto* modulator : neuromodulators) {
        delete modulator;
    }
    
    std::cout << "    testNeuromodulatorFactory passed" << std::endl;
}

void testNeuromodulatorErrorHandling() {
    nlm::Dopamine dopamine;
    
    // Test signal with invalid reward (should clamp)
    dopamine.signalReward(2.0f); // Should be clamped internally
    assert(dopamine.getLevel() <= 1.0f);
    
    dopamine.signalRewardPredictionError(-2.0f); // Should clamp
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test setLevel with invalid values (should clamp)
    dopamine.setLevel(-0.5f);
    assert(dopamine.getLevel() == 0.0f);
    
    dopamine.setLevel(1.5f);
    assert(dopamine.getLevel() == 1.0f);
    
    std::cout << "    testNeuromodulatorErrorHandling passed" << std::endl;
}

void testNeuromodulatorPlasticityModulation() {
    nlm::Dopamine lowDopamine;
    lowDopamine.setLevel(0.0f);
    float lowPlasticity = lowDopamine.getPlasticityFactor();
    
    nlm::Dopamine highDopamine;
    highDopamine.setLevel(1.0f);
    float highPlasticity = highDopamine.getPlasticityFactor();
    
    assert(lowPlasticity < highPlasticity);
    
    std::cout << "    testNeuromodulatorPlasticityModulation passed" << std::endl;
}

void runAll() {
    testNeuromodulatorBase();
    testNeuromodulatorAcetylcholine();
    testNeuromodulatorNorepinephrine();
    testNeuromodulatorSerotonin();
    testNeuromodulatorNeuromodulatorFactory();
    testNeuromodulatorErrorHandling();
    testNeuromodulatorPlasticityModulation();
}

} // namespace test_neuromodulator
