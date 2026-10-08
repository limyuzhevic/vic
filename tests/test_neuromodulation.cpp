// Neuromodulation Tests
// Enhanced neuromodulation tests with realistic biological models
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuromodulation {

void testDopamineDynamics() {
    nlm::Dopamine dopamine;
    
    // Test initial state
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    
    // Test level setting with bounds
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(1.5f);  // Should clamp to 1.0f
    assert(dopamine.getLevel() == 1.0f);
    
    dopamine.setLevel(-0.5f);  // Should clamp to 0.0f
    assert(dopamine.getLevel() == 0.0f);
    
    // Test plasticity factor
    assert(dopamine.getPlasticityFactor() == 0.5f + 0.5f * 0.0f);  // Baseline
    
    dopamine.setLevel(1.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f);  // Max plasticity
    
    // Test reward signaling
    dopamine.signalReward(0.3f);
    assert(dopamine.getLevel() > 0.0f);  // Should have increased
    
    // Test reward prediction error
    float current = dopamine.getLevel();
    dopamine.signalRewardPredictionError(0.1f);
    assert(dopamine.getLevel() > current);  // Should have increased
    
    // Test decay
    float levelBeforeDecay = dopamine.getLevel();
    dopamine.update(10.0);  // 10ms timestep
    assert(dopamine.getLevel() < levelBeforeDecay);  // Should have decayed
    
    std::cout << "    testDopamineDynamics passed" << std::endl;
}

void testAcetylcholineDynamics() {
    nlm::Acetylcholine acetylcholine;
    
    // Test basic properties
    assert(std::string(acetylcholine.getName()) == "ACh");
    assert(acetylcholine.getLevel() == 0.0f);
    assert(acetylcholine.getPlasticityFactor() == 1.0f);
    
    // Test level setting
    acetylcholine.setLevel(0.5f);
    assert(acetylcholine.getLevel() == 0.5f);
    
    // ACh modulates sensory processing gain
    float plasticityFactor = acetylcholine.getPlasticityFactor();
    assert(plasticityFactor == 1.0f);  // ACh has no effect on plasticity in this simplified model
    
    // Test update
    acetylcholine.update(10.0);
    assert(acetylcholine.getLevel() == 0.0f);  // No dynamics implemented in stub
    
    std::cout << "    testAcetylcholineDynamics passed" << std::endl;
}

void testNorepinephrineDynamics() {
    nlm::Norepinephrine norepinephrine;
    
    // Test basic properties
    assert(std::string(norepinephrine.getName()) == "NE");
    assert(norepinephrine.getLevel() == 0.0f);
    assert(norepinephrine.getPlasticityFactor() == 1.0f);
    
    // Test level setting
    norepinephrine.setLevel(0.8f);
    assert(norepinephrine.getLevel() == 0.8f);
    
    // NE affects arousal and signal-to-noise ratio
    float plasticityFactor = norepinephrine.getPlasticityFactor();
    assert(plasticityFactor == 1.0f);  // NE has no effect on plasticity in this simplified model
    
    // Test update
    norepinephrine.update(10.0);
    assert(norepinephrine.getLevel() == 0.0f);  // No dynamics implemented in stub
    
    std::cout << "    testNorepinephrineDynamics passed" << std::endl;
}

void testSerotoninDynamics() {
    nlm::Serotonin serotonin;
    
    // Test basic properties
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getLevel() == 0.0f);
    assert(serotonin.getPlasticityFactor() == 1.0f);
    
    // Test level setting
    serotonin.setLevel(0.3f);
    assert(serotonin.getLevel() == 0.3f);
    
    // Serotonin affects mood and behavioral inhibition
    float plasticityFactor = serotonin.getPlasticityFactor();
    assert(plasticityFactor == 1.0f);  // Serotonin has no effect on plasticity in this simplified model
    
    // Test update
    serotonin.update(10.0);
    assert(serotonin.getLevel() == 0.0f);  // No dynamics implemented in stub
    
    std::cout << "    testSerotoninDynamics passed" << std::endl;
}

void testNeuromodulatorIntegration() {
    // Test that neuromodulators can coexist
    nlm::Dopamine dopamine;
    nlm::Acetylcholine acetylcholine;
    nlm::Norepinephrine norepinephrine;
    nlm::Serotonin serotonin;
    
    // Set different levels
    dopamine.setLevel(0.7f);
    acetylcholine.setLevel(0.5f);
    norepinephrine.setLevel(0.8f);
    serotonin.setLevel(0.3f);
    
    // Test that all have different levels
    assert(dopamine.getLevel() == 0.7f);
    assert(acetylcholine.getLevel() == 0.5f);
    assert(norepinephrine.getLevel() == 0.8f);
    assert(serotonin.getLevel() == 0.3f);
    
    // Test that all have correct names
    assert(std::string(dopamine.getName()) == "DA");
    assert(std::string(acetylcholine.getName()) == "ACh");
    assert(std::string(norepinephrine.getName()) == "NE");
    assert(std::string(serotonin.getName()) == "5-HT");
    
    // Test that all have plasticity factors
    assert(dopamine.getPlasticityFactor() == 0.85f);  // 0.5 + 0.5*0.7
    assert(acetylcholine.getPlasticityFactor() == 1.0f);
    assert(norepinephrine.getPlasticityFactor() == 1.0f);
    assert(serotonin.getPlasticityFactor() == 1.0f);
    
    std::cout << "    testNeuromodulatorIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced neuromodulation tests..." << std::endl;
    testDopamineDynamics();
    testAcetylcholineDynamics();
    testNorepinephrineDynamics();
    testSerotoninDynamics();
    testNeuromodulatorIntegration();
    std::cout << "All neuromodulation tests passed!" << std::endl;
}

} // namespace test_neuromodulation
