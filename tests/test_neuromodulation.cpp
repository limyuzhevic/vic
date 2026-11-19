#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

void testAcetylcholine() {
    std::cout << "Testing Acetylcholine neuromodulator..." << std::endl;
    
    Acetylcholine ach;
    
    // Test initial state
    assert(ach.getName() == std::string("ACh"));
    assert(ach.getLevel() == 0.0f);
    
    // Test level setting
    ach.setLevel(0.5f);
    assert(ach.getLevel() == 0.5f);
    
    // Test plasticity factor
    float plasticity = ach.getPlasticityFactor();
    assert(plasticity >= 0.0f && plasticity <= 2.0f);
    
    // Test attention signaling
    ach.signalAttention(0.8f, 0.6f);
    assert(ach.getLevel() > 0.0f);
    
    // Test memory consolidation effect
    Brain mockBrain;
    NeuronId testNeuron(12345);
    ach.consolidateMemory(&mockBrain, testNeuron, 0.7f);
    
    std::cout << "✓ Acetylcholine tests passed" << std::endl;
}

void testNorepinephrine() {
    std::cout << "Testing Norepinephrine neuromodulator..." << std::endl;
    
    Norepinephrine ne;
    
    // Test initial state
    assert(ne.getName() == std::string("NE"));
    assert(ne.getLevel() == 0.0f);
    
    // Test level setting
    ne.setLevel(0.3f);
    assert(ne.getLevel() == 0.3f);
    
    // Test plasticity factor
    float plasticity = ne.getPlasticityFactor();
    assert(plasticity >= 0.0f && plasticity <= 2.0f);
    
    // Test arousal signaling
    ne.signalStress(0.5f);
    assert(ne.getArousalLevel() > 0.0f);
    
    // Test novelty signaling
    ne.signalNovelty(0.7f);
    assert(ne.getLevel() > 0.0f);
    
    std::cout << "✓ Norepinephrine tests passed" << std::endl;
}

void testSerotonin() {
    std::cout << "Testing Serotonin neuromodulator..." << std::endl;
    
    Serotonin serotonin;
    
    // Test initial state
    assert(serotonin.getName() == std::string("5-HT"));
    assert(serotonin.getLevel() == 0.0f);
    
    // Test level setting
    serotonin.setLevel(0.4f);
    assert(serotonin.getLevel() == 0.4f);
    
    // Test plasticity factor
    float plasticity = serotonin.getPlasticityFactor();
    assert(plasticity >= 0.0f && plasticity <= 2.0f);
    
    // Test reward signaling
    serotonin.signalReward(0.8f);
    assert(serotonin.getMoodLevel() > 0.0f);
    
    // Test punishment signaling
    serotonin.signalPunishment(0.6f);
    assert(serotonin.getLevel() < 0.5f);
    
    std::cout << "✓ Serotonin tests passed" << std::endl;
}

void testDopamine() {
    std::cout << "Testing Dopamine neuromodulator..." << std::endl;
    
    Dopamine dopamine;
    
    // Test initial state
    assert(dopamine.getName() == std::string("DA"));
    assert(dopamine.getLevel() == 0.0f);
    
    // Test level setting
    dopamine.setLevel(0.6f);
    assert(dopamine.getLevel() == 0.6f);
    
    // Test plasticity factor
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.0f && plasticity <= 2.0f);
    
    // Test reward prediction error
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() > 0.0f);
    
    // Test motivation update
    dopamine.updateMotivation(0.1f);
    assert(dopamine.getLevel() > 0.0f);
    
    std::cout << "✓ Dopamine tests passed" << std::endl;
}

void testIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    // Test that all neuromodulators can be instantiated
    Acetylcholine ach;
    Norepinephrine ne;
    Serotonin serotonin;
    Dopamine dopamine;
    
    // Test that they have different names
    assert(std::string(ach.getName()) != std::string(ne.getName()));
    assert(std::string(ne.getName()) != std::string(serotonin.getName()));
    assert(std::string(serotonin.getName()) != std::string(dopamine.getName()));
    
    // Test that they can all have their levels set
    ach.setLevel(0.3f);
    ne.setLevel(0.4f);
    serotonin.setLevel(0.5f);
    dopamine.setLevel(0.6f);
    
    assert(ach.getLevel() == 0.3f);
    assert(ne.getLevel() == 0.4f);
    assert(serotonin.getLevel() == 0.5f);
    assert(dopamine.getLevel() == 0.6f);
    
    // Test that plasticity factors are different
    float achPlasticity = ach.getPlasticityFactor();
    float nePlasticity = ne.getPlasticityFactor();
    float serotoninPlasticity = serotonin.getPlasticityFactor();
    float dopaminePlasticity = dopamine.getPlasticityFactor();
    
    // They should all be in valid range
    assert(achPlasticity >= 0.0f && achPlasticity <= 2.0f);
    assert(nePlasticity >= 0.0f && nePlasticity <= 2.0f);
    assert(serotoninPlasticity >= 0.0f && serotoninPlasticity <= 2.0f);
    assert(dopaminePlasticity >= 0.0f && dopaminePlasticity <= 2.0f);
    
    std::cout << "✓ Integration tests passed" << std::endl;
}

int main() {
    std::cout << "Running neuromodulation integration tests..." << std::endl;
    std::cout << "=" << std::endl;
    
    try {
        testAcetylcholine();
        testNorepinephrine();
        testSerotonin();
        testDopamine();
        testIntegration();
        
        std::cout << std::endl;
        std::cout << "=" << std::endl;
        std::cout << "All neuromodulation tests passed! ✓" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}