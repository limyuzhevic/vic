// Neuromodulation Tests
// Tests for dopamine, acetylcholine, norepinephrine, and serotonin systems

#include "neuromodulation/Neuromodulator.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_neuromodulation {

void testDopamine() {
    std::cout << "    Testing Dopamine neuromodulation..." << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test basic properties
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getBaselineLevel() > 0.0f);
    assert(dopamine.getPeakLevel() > 0.0f);
    assert(dopamine.isEnabled());
    
    // Test level manipulation
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(1.5f); // Should clamp to peak
    assert(dopamine.getLevel() <= dopamine.getPeakLevel());
    
    // Test plasticity factor
    float plasticityFactor = dopamine.getPlasticityFactor();
    assert(plasticityFactor >= 0.5f && plasticityFactor <= 1.0f); // Reasonable range
    
    // Test prediction error signaling
    dopamine.signalRewardPredictionError(0.1f);
    assert(dopamine.getLastPredictionError() == 0.1f);
    
    dopamine.signalRewardPredictionError(-0.2f);
    assert(dopamine.getLastPredictionError() == -0.2f);
    
    // Test reward signaling
    dopamine.signalReward(0.5f);
    assert(dopamine.getLevel() > 0.0f);
    
    // Test update function
    dopamine.update(0.001, 0.1f, 0.5f, 0.0f);
    
    // Test parameter setters
    dopamine.setLearningRate(0.02f);
    assert(dopamine.getLearningRate() == 0.02f);
    
    dopamine.setTimeConstant(100.0f);
    assert(dopamine.getTimeConstant() == 100.0f);
    
    dopamine.setEnabled(true);
    assert(dopamine.isEnabled());
    
    // Test reset
    dopamine.reset();
    assert(dopamine.getLevel() == dopamine.getBaselineLevel());
    
    std::cout << "      Dopamine neuromodulation test PASSED" << std::endl;
}

void testAcetylcholine() {
    std::cout << "    Testing Acetylcholine neuromodulation..." << std::endl;
    
    nlm::Acetylcholine ach;
    
    assert(std::string(ach.getName()) == "ACh");
    assert(ach.isEnabled());
    
    // Test attention signaling
    ach.setLevel(0.3f);
    assert(ach.getLevel() == 0.3f);
    
    float attention = ach.getAttentionLevel();
    assert(attention > 0.0f);
    
    ach.signalAttention(0.5f);
    ach.signalSensoryInput(0.7f);
    
    ach.update(0.001, 0.1f, 0.0f, 0.5f);
    
    ach.setTimeConstant(50.0f);
    assert(ach.getTimeConstant() == 50.0f);
    
    ach.setEnabled(false);
    assert(!ach.isEnabled());
    
    std::cout << "      Acetylcholine neuromodulation test PASSED" << std::endl;
}

void testNorepinephrine() {
    std::cout << "    Testing Norepinephrine neuromodulation..." << std::endl;
    
    nlm::Norepinephrine ne;
    
    assert(std::string(ne.getName()) == "NE");
    assert(ne.isEnabled());
    
    // Test arousal signaling
    ne.setLevel(0.2f);
    assert(ne.getLevel() == 0.2f);
    
    float arousal = ne.getArousalLevel();
    assert(arousal > 0.0f);
    
    ne.signalArousal(0.4f);
    ne.signalStress(0.3f);
    
    ne.update(0.001, 0.1f, 0.0f, 0.3f);
    
    ne.setTimeConstant(80.0f);
    assert(ne.getTimeConstant() == 80.0f);
    
    ne.setAdaptationRate(0.08f);
    assert(ne.getAdaptationRate() == 0.08f);
    
    ne.setEnabled(false);
    assert(!ne.isEnabled());
    
    std::cout << "      Norepinephrine neuromodulation test PASSED" << std::endl;
}

void testSerotonin() {
    std::cout << "    Testing Serotonin neuromodulation..." << std::endl;
    
    nlm::Serotonin serotonin;
    
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.isEnabled());
    
    // Test mood signaling
    serotonin.setLevel(0.15f);
    assert(serotonin.getLevel() == 0.15f);
    
    float mood = serotonin.getMoodLevel();
    assert(mood > 0.0f);
    
    float impulsivity = serotonin.getImpulsivityLevel();
    assert(impulsivity < 1.0f);
    
    serotonin.signalMood(0.4f);
    serotonin.signalSocialContext(0.3f);
    
    serotonin.update(0.001, 0.1f, 0.3f, 0.2f);
    
    serotonin.setTimeConstant(60.0f);
    assert(serotonin.getTimeConstant() == 60.0f);
    
    serotonin.setInhibitionFactor(0.9f);
    assert(serotonin.getInhibitionFactor() == 0.9f);
    
    serotonin.setEnabled(false);
    assert(!serotonin.isEnabled());
    
    std::cout << "      Serotonin neuromodulation test PASSED" << std::endl;
}

void testNeuromodulationSystem() {
    std::cout << "    Testing NeuromodulationSystem..." << std::endl;
    
    nlm::NeuromodulationSystem system;
    
    assert(system.isEnabled());
    
    // Add neuromodulators
    auto dopamine = std::make_shared<nlm::Dopamine>();
    auto acetylcholine = std::make_shared<nlm::Acetylcholine>();
    
    system.addNeuromodulator(dopamine);
    system.addNeuromodulator(acetylcholine);
    
    // Test retrieval
    auto retrievedDA = system.getNeuromodulator("DA");
    assert(retrievedDA != nullptr);
    assert(std::string(retrievedDA->getName()) == "DA");
    
    auto retrievedACh = system.getNeuromodulator("ACh");
    assert(retrievedACh != nullptr);
    assert(std::string(retrievedACh->getName()) == "ACh");
    
    // Test combined plasticity factor
    float combinedFactor = system.getCombinedPlasticityFactor();
    assert(combinedFactor > 0.0f);
    
    // Test update
    system.update(0.001, 0.1f, 0.5f, 0.3f);
    
    // Test reset
    system.reset();
    combinedFactor = system.getCombinedPlasticityFactor();
    assert(combinedFactor > 0.0f);
    
    // Test disable/enable
    system.setEnabled(false);
    assert(!system.isEnabled());
    
    system.setEnabled(true);
    assert(system.isEnabled());
    
    std::cout << "      NeuromodulationSystem test PASSED" << std::endl;
}

void runAll() {
    testDopamine();
    testAcetylcholine();
    testNorepinephrine();
    testSerotonin();
    testNeuromodulationSystem();
    
    std::cout << "    All neuromodulation tests PASSED" << std::endl;
}

} // namespace test_neuromodulation