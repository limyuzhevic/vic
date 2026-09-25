// Neuromodulator Tests
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulator {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "Dopamine");
    assert(dopamine.getLevel() >= 0.0f);
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineLevel() {
    nlm::Dopamine dopamine;
    
    // Test default level
    float defaultLevel = dopamine.getLevel();
    assert(defaultLevel >= 0.0f);
    
    // Test level setting
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(1.0f);
    assert(dopamine.getLevel() == 1.0f);
    
    std::cout << "    testDopamineLevel passed" << std::endl;
}

void testPlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // Test plasticity factor (placeholder implementation)
    float factor = dopamine.getPlasticityFactor();
    assert(factor >= 0.0f);
    
    // Test that setting level affects plasticity factor
    // (depending on implementation)
    dopamine.setLevel(0.5f);
    float factorAfterLevelChange = dopamine.getPlasticityFactor();
    assert(factorAfterLevelChange >= 0.0f);
    
    std::cout << "    testPlasticityFactor passed" << std::endl;
}

void testRewardSignaling() {
    nlm::Dopamine dopamine;
    
    // Test reward signaling (placeholder implementation)
    // This is a basic test to ensure methods exist
    dopamine.signalReward(1.0f);
    dopamine.signalRewardPredictionError(0.5f);
    
    // Verify level increased
    assert(dopamine.getLevel() >= 1.0f);
    
    std::cout << "    testRewardSignaling passed" << std::endl;
}

void testNeuromodulatorInterface() {
    // Test acetylcholine interface
    nlm::Acetylcholine ach;
    assert(std::string(ach.getName()) == "ACh");
    assert(ach.getLevel() == 0.0f);
    
    // Test norepinephrine interface
    nlm::Norepinephrine ne;
    assert(std::string(ne.getName()) == "NE");
    assert(ne.getLevel() == 0.0f);
    
    // Test serotonin interface
    nlm::Serotonin serotonin;
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getLevel() == 0.0f);
    
    std::cout << "    testNeuromodulatorInterface passed" << std::endl;
}

void testNeuromodulatorUpdate() {
    nlm::Dopamine dopamine;
    
    // Test update method exists and doesn't crash
    dopamine.update(0.001f);  // Small timestep
    
    assert(dopamine.getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulatorUpdate passed" << std::endl;
}

void runAll() {
    testDopamineCreation();
    testDopamineLevel();
    testPlasticityFactor();
    testRewardSignaling();
    testNeuromodulatorInterface();
    testNeuromodulatorUpdate();
    
    std::cout << "=== Neuromodulator Tests Complete ===" << std::endl;
}

} // namespace test_neuromodulator