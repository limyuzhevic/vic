// NLM Neuromodulation Tests
// Phase 6: Complete Neuromodulation Integration Tests

#include <iostream>
#include <cassert>
#include <cmath>
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"

namespace test_neuromodulation {

void testBasicNeuromodulatorCreation() {
    std::cout << "  Testing basic neuromodulator creation..." << std::endl;
    
    // Test dopamine
    Dopamine dopamine;
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() >= 0.0f);
    assert(dopamine.getLevel() <= 1.0f);
    assert(dopamine.getPlasticityFactor() > 0.0f);
    
    // Test acetylcholine
    Acetylcholine acetylcholine;
    assert(std::string(acetylcholine.getName()) == "ACh");
    assert(acetylcholine.getLevel() >= 0.0f);
    assert(acetylcholine.getLevel() <= 1.0f);
    assert(acetylcholine.getPlasticityFactor() > 0.0f);
    
    // Test norepinephrine
    Norepinephrine norepinephrine;
    assert(std::string(norepinephrine.getName()) == "NE");
    assert(norepinephrine.getLevel() >= 0.0f);
    assert(norepinephrine.getLevel() <= 1.0f);
    assert(norepinephrine.getPlasticityFactor() > 0.0f);
    
    // Test serotonin
    Serotonin serotonin;
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getLevel() >= 0.0f);
    assert(serotonin.getLevel() <= 1.0f);
    assert(serotonin.getPlasticityFactor() > 0.0f);
    
    std::cout << "    Basic neuromodulator creation: PASSED" << std::endl;
}

void testDopamineDynamics() {
    std::cout << "  Testing dopamine dynamics..." << std::endl;
    
    Dopamine dopamine;
    
    // Test initial state
    assert(dopamine.getLevel() == 0.0f);
    
    // Test reward signaling
    dopamine.signalReward(0.5f);
    assert(dopamine.getLevel() > 0.0f);
    assert(dopamine.getPredictionError() > 0.0f);
    
    // Test prediction error signaling
    dopamine.signalRewardPredictionError(0.3f);
    assert(dopamine.getPredictionError() > 0.0f);
    
    // Test extinction
    dopamine.signalExtinction(0.2f);
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test omission
    dopamine.signalOmission(0.0f);
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test working memory update
    dopamine.updateWorkingMemory(0.4f);
    
    // Test reward prediction update
    std::vector<float> predicted = {0.3f, 0.5f, 0.7f};
    std::vector<float> actual = {0.4f, 0.6f, 0.8f};
    dopamine.updateRewardPrediction(predicted, actual);
    assert(dopamine.getPredictionError() > 0.0f);
    
    // Test credit assignment
    dopamine.assignCredit(0.2f, 0.8f);
    
    // Test plasticity factor
    float plasticityFactor = dopamine.getPlasticityFactor();
    assert(plasticityFactor > 0.0f && plasticityFactor <= 1.5f);
    
    std::cout << "    Dopamine dynamics: PASSED" << std::endl;
}

void testAcetylcholineDynamics() {
    std::cout << "  Testing acetylcholine dynamics..." << std::endl;
    
    Acetylcholine acetylcholine;
    
    // Test attention modulation
    acetylcholine.modulateAttention(0.5f, 0.3f);
    assert(acetylcholine.getAttentionGain() > 1.0f);
    
    // Test learning modulation
    acetylcholine.modulateLearning(0.1f, 0.2f);
    assert(acetylcholine.getLearningModulation() > 0.8f);
    
    // Test working memory encoding
    acetylcholine.encodeWorkingMemory(0.7f, 0.8f, 0.5f);
    assert(acetylcholine.getLevel() > 0.0f);
    
    // Test working memory consolidation
    acetylcholine.consolidateWorkingMemory();
    
    // Test encoding facilitation
    acetylcholine.facilitateEncoding(0.2f);
    
    // Test retrieval facilitation
    acetylcholine.facilitateRetrieval(0.9f);
    
    std::cout << "    Acetylcholine dynamics: PASSED" << std::endl;
}

void testNorepinephrineDynamics() {
    std::cout << "  Testing norepinephrine dynamics..." << std::endl;
    
    Norepinephrine norepinephrine;
    
    // Test arousal modulation
    norepinephrine.modulateArousal(0.6f, 0.3f, 0.2f);
    assert(norepinephrine.getLevel() > 0.0f);
    
    // Test vigilance increase
    norepinephrine.increaseVigilance(0.7f, 0.8f);
    assert(norepinephrine.getLevel() > 0.0f);
    
    // Test attention focus
    norepinephrine.focusAttention(0.9f, 0.6f);
    assert(norepinephrine.getLevel() > 0.0f);
    
    // Test working memory enhancement
    norepinephrine.enhanceWorkingMemory(5, 0.8f);
    assert(norepinephrine.getWorkingMemoryEnhancement() > 1.0f);
    
    // Test cognitive control
    norepinephrine.modulateCognitiveControl(0.7f, 0.5f);
    assert(norepinephrine.getLevel() > 0.0f);
    
    // Test locus coeruleus interactions
    norepinephrine.updateLocusCoeruleus(0.4f, 0.6f, 0.1f);
    
    // Test stress modulation
    norepinephrine.modulateStress(0.3f, 0.4f);
    assert(norepinephrine.getStressModulation() > 1.0f);
    
    std::cout << "    Norepinephrine dynamics: PASSED" << std::endl;
}

void testSerotoninDynamics() {
    std::cout << "  Testing serotonin dynamics..." << std::endl;
    
    Serotonin serotonin;
    
    // Test mood modulation
    serotonin.modulateMood(0.7f, 0.2f, 0.5f);
    assert(serotonin.getLevel() > 0.0f);
    
    // Test anxiety reduction
    serotonin.reduceAnxiety(0.8f, 0.7f, 0.9f);
    assert(serotonin.getLevel() > 0.0f);
    
    // Test impulsivity modulation
    serotonin.modulateImpulsivity(1.5f, 0.2f);
    assert(serotonin.getLevel() > 0.0f);
    
    // Test social behavior modulation
    serotonin.modulateSocialBehavior(0.6f, 0.8f, 0.3f);
    assert(serotonin.getSocialModulation() > 0.0f);
    
    // Test punishment signaling
    serotonin.signalPunishment(0.8f, 0.9f);
    assert(serotonin.getLevel() > 0.0f);
    
    // Test behavioral strategy update
    serotonin.updateBehavioralStrategy(0.5f, 0.7f);
    
    // Test internal state adaptation
    serotonin.adaptToInternalState(0.3f, 0.4f, 0.5f);
    
    std::cout << "    Serotonin dynamics: PASSED" << std::endl;
}

void testNeuromodulatorIntegration() {
    std::cout << "  Testing neuromodulator integration..." << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulator access
    Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    Acetylcholine* acetylcholine = brain.getAcetylcholine();
    assert(acetylcholine != nullptr);
    
    Norepinephrine* norepinephrine = brain.getNorepinephrine();
    assert(norepinephrine != nullptr);
    
    Serotonin* serotonin = brain.getSerotonin();
    assert(serotonin != nullptr);
    
    Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    PredictionError* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    Reward* reward = brain.getReward();
    assert(reward != nullptr);
    
    // Test neuromodulation application
    // Create a neuromodulator signal
    Neuromodulator signal;
    signal.setLevel(0.5f);
    brain.applyNeuromodulation(signal);
    
    // Run brain step with neuromodulators
    for (int i = 0; i < 5; ++i) {
        brain.step(i);
    }
    
    std::cout << "    Neuromodulator integration: PASSED" << std::endl;
}

void testNeuromodulatorBalance() {
    std::cout << "  Testing neuromodulator balance..." << std::endl;
    
    // This would test the balance system if implemented
    // For now, just verify basic functionality
    Dopamine dopamine;
    Acetylcholine acetylcholine;
    Norepinephrine norepinephrine;
    Serotonin serotonin;
    
    // All should have different initial levels
    assert(dopamine.getLevel() != acetylcholine.getLevel());
    assert(dopamine.getLevel() != norepinephrine.getLevel());
    assert(dopamine.getLevel() != serotonin.getLevel());
    
    std::cout << "    Neuromodulator balance: PASSED" << std::endl;
}

void testNeuromodulatorPlasticity() {
    std::cout << "  Testing neuromodulator plasticity effects..." << std::endl;
    
    Dopamine dopamine;
    Acetylcholine acetylcholine;
    Norepinephrine norepinephrine;
    Serotonin serotonin;
    
    // Store initial plasticity factors
    float dopaminePlasticity = dopamine.getPlasticityFactor();
    float acetylcholinePlasticity = acetylcholine.getPlasticityFactor();
    float norepinephrinePlasticity = norepinephrine.getPlasticityFactor();
    float serotoninPlasticity = serotonin.getPlasticityFactor();
    
    // Update all neuromodulators
    dopamine.update(0.01f);
    acetylcholine.update(0.01f);
    norepinephrine.update(0.01f);
    serotonin.update(0.01f);
    
    // Plasticity factors should change with neuromodulator dynamics
    assert(dopamine.getPlasticityFactor() != dopaminePlasticity);
    assert(acetylcholine.getPlasticityFactor() != acetylcholinePlasticity);
    assert(norepinephrine.getPlasticityFactor() != norepinephrinePlasticity);
    assert(serotonin.getPlasticityFactor() != serotoninPlasticity);
    
    std::cout << "    Neuromodulator plasticity: PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Neuromodulation Tests ===" << std::endl;
    
    testBasicNeuromodulatorCreation();
    testDopamineDynamics();
    testAcetylcholineDynamics();
    testNorepinephrineDynamics();
    testSerotoninDynamics();
    testNeuromodulatorIntegration();
    testNeuromodulatorBalance();
    testNeuromodulatorPlasticity();
    
    std::cout << std::endl;
    std::cout << "=== All Neuromodulation Tests PASSED ===" << std::endl;
}

} // namespace test_neuromodulation