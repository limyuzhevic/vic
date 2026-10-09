#include "Neuromodulator.hpp"
#include "PredictionSystem.hpp"
#include "MotorSystem.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {

void test_dopamine_system() {
    std::cout << "Testing Dopamine Neuromodulator System" << std::endl;
    std::cout << "===================================== " << std::endl;
    
    // Test 1: Basic dopamine creation and properties
    std::cout << "Test 1: Basic dopamine creation..." << std::endl;
    Dopamine dopamine;
    
    assert(dopamine.getName() == std::string("DA"));
    assert(dopamine.getLevel() >= 0.0f);
    assert(dopamine.getPlasticityFactor() >= 0.0f);
    assert(dopamine.getTonicLevel() >= 0.0f);
    assert(dopamine.getPhasicBurst() >= 0.0f);
    
    std::cout << "✓ Basic dopamine creation passed" << std::endl;
    
    // Test 2: Reward signaling
    std::cout << "Test 2: Reward signaling..." << std::endl;
    float initialLevel = dopamine.getLevel();
    dopamine.signalReward(1.0f);
    assert(dopamine.getLevel() > initialLevel);
    
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() > 0.0f);
    std::cout << "✓ Reward signaling passed" << std::endl;
    
    // Test 3: Parameter configuration
    std::cout << "Test 3: Parameter configuration..." << std::endl;
    dopamine.setBaseline(20.0f);
    assert(dopamine.getTonicLevel() == 20.0f);
    
    dopamine.setPeakLevel(80.0f);
    assert(dopamine.getPhasicBurst() >= 0.0f);
    
    dopamine.setDecayRate(0.1f);
    dopamine.setReleaseRate(0.5f);
    
    dopamine.setReceptorParameters(10.0f, 1.0f);
    assert(dopamine.getReceptorState().affinity == 10.0f);
    assert(dopamine.getReceptorState().density == 1.0f);
    
    std::cout << "✓ Parameter configuration passed" << std::endl;
    
    // Test 4: Plasticity modulation
    std::cout << "Test 4: Plasticity modulation..." << std::endl;
    float plasticity1 = dopamine.getPlasticityFactor();
    dopamine.setLevel(50.0f);  // Near peak for inverted U-shaped function
    float plasticity2 = dopamine.getPlasticityFactor();
    
    assert(plasticity2 <= plasticity1);  // Should be lower at peak for dopamine
    
    // Test reward-modulated plasticity
    float rewardPlasticity = dopamine.getRewardModulatedPlasticity();
    assert(rewardPlasticity >= 0.0f);
    
    float learningRate = dopamine.getLearningRateEnhancement();
    assert(learningRate >= 0.0f);
    std::cout << "✓ Plasticity modulation passed" << std::endl;
    
    // Test 5: Update functionality
    std::cout << "Test 5: Update functionality..." << std::endl;
    dopamine.update(0.1f);
    assert(dopamine.getLevel() >= 0.0f);
    std::cout << "✓ Update functionality passed" << std::endl;
    
    // Test 6: Event signaling
    std::cout << "Test 6: Event signaling..." << std::endl;
    dopamine.signalEvent(1.0f, EventType::Reward);
    dopamine.signalEvent(0.5f, EventType::Novelty);
    dopamine.signalEvent(0.8f, EventType::PredictionError);
    assert(dopamine.getLevel() >= 0.0f);
    std::cout << "✓ Event signaling passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Dopamine System tests PASSED! ✓" << std::endl;
    std::cout << "===================================== " << std::endl;
}

void test_serotonin_system() {
    std::cout << "Testing Serotonin Neuromodulator System" << std::endl;
    std::cout << "============================================ " << std::endl;
    
    // Test 1: Basic serotonin creation and properties
    std::cout << "Test 1: Basic serotonin creation..." << std::endl;
    Serotonin serotonin;
    
    assert(serotonin.getName() == std::string("5-HT"));
    assert(serotonin.getLevel() >= 0.0f);
    assert(serotonin.getPlasticityFactor() >= 0.0f);
    assert(serotonin.getLearningRateModulation() >= 0.0f);
    assert(serotonin.getNoveltySensitivity() >= 0.0f);
    
    std::cout << "✓ Basic serotonin creation passed" << std::endl;
    
    // Test 2: Novelty signaling
    std::cout << "Test 2: Novelty signaling..." << std::endl;
    float initialLevel = serotonin.getLevel();
    serotonin.signalNovelty(1.0f);
    assert(serotonin.getLevel() > initialLevel);
    
    serotonin.signalMoodState(0.8f);
    assert(serotonin.getLevel() > 0.0f);
    assert(serotonin.getLearningRateModulation() >= 0.0f);
    std::cout << "✓ Novelty signaling passed" << std::endl;
    
    // Test 3: Learning rate modulation
    std::cout << "Test 3: Learning rate modulation..." << std::endl;
    float baseRate = 0.1f;
    serotonin.setLearningRateBase(baseRate);
    assert(std::abs(serotonin.getLearningRateModulation() - baseRate) < 0.01f);
    
    serotonin.setLearningRateVariance(0.05f);
    assert(serotonin.getLearningRateModulation() >= 0.0f);
    std::cout << "✓ Learning rate modulation passed" << std::endl;
    
    // Test 4: Novelty detection
    std::cout << "Test 4: Novelty detection..." << std::endl;
    serotonin.setNoveltySensitivity(2.0f);
    assert(serotonin.getNoveltySensitivity() == 2.0f);
    
    float sensitivity = serotonin.getNoveltySensitivity();
    assert(sensitivity >= 1.0f);
    std::cout << "✓ Novelty detection passed" << std::endl;
    
    // Test 5: Update functionality
    std::cout << "Test 5: Update functionality..." << std::endl;
    serotonin.update(0.1f);
    assert(serotonin.getLevel() >= 0.0f);
    std::cout << "✓ Update functionality passed" << std::endl;
    
    // Test 6: Event signaling
    std::cout << "Test 6: Event signaling..." << std::endl;
    serotonin.signalEvent(1.0f, EventType::Reward);
    serotonin.signalEvent(0.5f, EventType::Novelty);
    serotonin.signalEvent(0.8f, EventType::LearningRateAdjustment);
    assert(serotonin.getLevel() >= 0.0f);
    std::cout << "✓ Event signaling passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Serotonin System tests PASSED! ✓" << std::endl;
    std::cout << "============================================ " << std::endl;
}

void test_acetylcholine_system() {
    std::cout << "Testing Acetylcholine Neuromodulator System" << std::endl;
    std::cout << "============================================== " << std::endl;
    
    // Test 1: Basic acetylcholine creation and properties
    std::cout << "Test 1: Basic acetylcholine creation..." << std::endl;
    Acetylcholine ach;
    
    assert(ach.getName() == std::string("ACh"));
    assert(ach.getLevel() >= 0.0f);
    assert(ach.getPlasticityFactor() >= 0.0f);
    assert(ach.getAttentionalFocus() >= 0.0f);
    assert(ach.getSensoryDetectionThreshold() >= 0.0f);
    assert(ach.getMemoryConsolidationBoost() >= 0.0f);
    
    std::cout << "✓ Basic acetylcholine creation passed" << std::endl;
    
    // Test 2: Attention signaling
    std::cout << "Test 2: Attention signaling..." << std::endl;
    float initialFocus = ach.getAttentionalFocus();
    ach.signalAttentionalFocus(0.9f);
    assert(ach.getAttentionalFocus() > initialFocus);
    
    ach.signalWorkingMemoryLoad(0.7f);
    assert(ach.getLevel() > 0.0f);
    
    ach.signalSensoryGain(0.8f);
    assert(ach.getSensoryDetectionThreshold() < 1.0f);
    std::cout << "✓ Attention signaling passed" << std::endl;
    
    // Test 3: Attention threshold configuration
    std::cout << "Test 3: Attention threshold configuration..." << std::endl;
    ach.setAttentionThreshold(0.5f);
    assert(ach.getSensoryDetectionThreshold() == 0.5f);
    
    ach.setMemoryConsolidationRate(0.8f);
    assert(ach.getMemoryConsolidationBoost() >= 0.0f);
    std::cout << "✓ Attention threshold configuration passed" << std::endl;
    
    // Test 4: Update functionality
    std::cout << "Test 4: Update functionality..." << std::endl;
    ach.update(0.1f);
    assert(ach.getLevel() >= 0.0f);
    std::cout << "✓ Update functionality passed" << std::endl;
    
    // Test 5: Event signaling
    std::cout << "Test 5: Event signaling..." << std::endl;
    ach.signalEvent(1.0f, EventType::Attention);
    ach.signalEvent(0.7f, EventType::WorkingMemoryLoad);
    ach.signalEvent(0.8f, EventType::SensoryGain);
    assert(ach.getLevel() >= 0.0f);
    std::cout << "✓ Event signaling passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Acetylcholine System tests PASSED! ✓" << std::endl;
    std::cout << "============================================== " << std::endl;
}

void test_norepinephrine_system() {
    std::cout << "Testing Norepinephrine Neuromodulator System" << std::endl;
    std::cout << "=============================================== " << std::endl;
    
    // Test 1: Basic norepinephrine creation and properties
    std::cout << "Test 1: Basic norepinephrine creation..." << std::endl;
    Norepinephrine ne;
    
    assert(ne.getName() == std::string("NE"));
    assert(ne.getLevel() >= 0.0f);
    assert(ne.getPlasticityFactor() >= 0.0f);
    assert(ne.getArousal() >= 0.0f);
    assert(ne.getReactionTimeModulation() >= 0.0f);
    assert(ne.getStressResponseLevel() >= 0.0f);
    assert(ne.getFightOrFlightActivation() >= 0.0f);
    
    std::cout << "✓ Basic norepinephrine creation passed" << std::endl;
    
    // Test 2: Arousal signaling
    std::cout << "Test 2: Arousal signaling..." << std::endl;
    float initialArousal = ne.getArousal();
    ne.signalArousal(0.9f);
    assert(ne.getArousal() > initialArousal);
    
    ne.signalStressResponse(0.7f);
    assert(ne.getLevel() > 0.0f);
    
    ne.signalVigilance(0.8f);
    assert(ne.getArousal() > 0.5f);
    std::cout << "✓ Arousal signaling passed" << std::endl;
    
    // Test 3: Reaction time modulation
    std::cout << "Test 3: Reaction time modulation..." << std::endl;
    float reactionTime1 = ne.getReactionTimeModulation();
    ne.setReactionTimeFactor(1.5f);
    assert(ne.getReactionTimeModulation() < reactionTime1);  // Higher factor = faster
    
    float reactionTime2 = ne.getReactionTimeModulation();
    assert(reactionTime2 > 0.0f);
    std::cout << "✓ Reaction time modulation passed" << std::endl;
    
    // Test 4: Stress response
    std::cout << "Test 4: Stress response..." << std::endl;
    ne.setStressSensitivity(0.5f);
    assert(ne.getArousal() > 0.5f);
    
    float stressLevel = ne.getStressResponseLevel();
    assert(stressLevel >= 0.0f);
    
    float fightOrFlight = ne.getFightOrFlightActivation();
    assert(fightOrFlight >= 0.0f);
    std::cout << "✓ Stress response passed" << std::endl;
    
    // Test 5: Update functionality
    std::cout << "Test 5: Update functionality..." << std::endl;
    ne.update(0.1f);
    assert(ne.getLevel() >= 0.0f);
    std::cout << "✓ Update functionality passed" << std::endl;
    
    // Test 6: Event signaling
    std::cout << "Test 6: Event signaling..." << std::endl;
    ne.signalEvent(1.0f, EventType::Arousal);
    ne.signalEvent(0.7f, EventType::StressResponse);
    ne.signalEvent(0.8f, EventType::Vigilance);
    assert(ne.getLevel() >= 0.0f);
    std::cout << "✓ Event signaling passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Norepinephrine System tests PASSED! ✓" << std::endl;
    std::cout << "=============================================== " << std::endl;
}

void test_neuromodulator_ensemble() {
    std::cout << "Testing Neuromodulator Ensemble" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Test 1: Basic ensemble creation
    std::cout << "Test 1: Basic ensemble creation..." << std::endl;
    NeuromodulatorEnsemble ensemble;
    
    assert(ensemble.getExplorationProbability() >= 0.0f && ensemble.getExplorationProbability() <= 1.0f);
    assert(ensemble.getDecisionUncertainty() >= 0.0f && ensemble.getDecisionUncertainty() <= 1.0f);
    assert(ensemble.getCombinedPlasticityFactor() >= 0.0f && ensemble.getCombinedPlasticityFactor() <= 1.0f);
    std::cout << "✓ Basic ensemble creation passed" << std::endl;
    
    // Test 2: Individual neuromodulator access
    std::cout << "Test 2: Individual neuromodulator access..." << std::endl;
    assert(ensemble.getDopamineLevel() >= 0.0f);
    assert(ensemble.getSerotoninLevel() >= 0.0f);
    assert(ensemble.getAChLevel() >= 0.0f);
    assert(ensemble.getNELevel() >= 0.0f);
    std::cout << "✓ Individual neuromodulator access passed" << std::endl;
    
    // Test 3: Event signaling
    std::cout << "Test 3: Event signaling..." << std::endl;
    ensemble.signalEvent(EventType::Reward, 1.0f);
    ensemble.signalEvent(EventType::Novelty, 0.5f);
    ensemble.signalEvent(EventType::Attention, 0.8f);
    ensemble.signalEvent(EventType::Stress, 0.3f);
    ensemble.signalEvent(EventType::Arousal, 0.9f);
    std::cout << "✓ Event signaling passed" << std::endl;
    
    // Test 4: Update functionality
    std::cout << "Test 4: Update functionality..." << std::endl;
    ensemble.update(0.1f);
    assert(ensemble.getExplorationProbability() >= 0.0f && ensemble.getExplorationProbability() <= 1.0f);
    assert(ensemble.getDecisionUncertainty() >= 0.0f && ensemble.getDecisionUncertainty() <= 1.0f);
    assert(ensemble.getCombinedPlasticityFactor() >= 0.0f && ensemble.getCombinedPlasticityFactor() <= 1.0f);
    std::cout << "✓ Update functionality passed" << std::endl;
    
    // Test 5: Composite metrics
    std::cout << "Test 5: Composite metrics..." << std::endl;
    float rewardPredictionError = ensemble.getRewardPredictionError();
    float noveltySignal = ensemble.getNoveltySignal();
    float attentionFocus = ensemble.getAttentionFocus();
    float arousalLevel = ensemble.getArousalLevel();
    
    assert(rewardPredictionError >= 0.0f);
    assert(noveltySignal >= 0.0f);
    assert(attentionFocus >= 0.0f);
    assert(arousalLevel >= 0.0f);
    
    // Test exploration-exploitation tradeoff
    float exploration = ensemble.getExplorationProbability();
    float exploitation = 1.0f - exploration;
    assert(exploration >= 0.0f && exploration <= 1.0f);
    assert(exploitation >= 0.0f && exploitation <= 1.0f);
    std::cout << "✓ Composite metrics passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Neuromodulator Ensemble tests PASSED! ✓" << std::endl;
    std::cout << "===============================" << std::endl;
}

void run_all_neuromodulation_tests() {
    std::cout << "Running comprehensive Neuromodulation tests..." << std::endl;
    std::cout << "==================================================" << std::endl;
    
    test_dopamine_system();
    test_serotonin_system();
    test_acetylcholine_system();
    test_norepinephrine_system();
    test_neuromodulator_ensemble();
    
    std::cout << "==================================================" << std::endl;
    std::cout << "All Neuromodulation tests PASSED! ✓" << std::endl;
    std::cout << "==================================================" << std::endl;
}

} // namespace nlm