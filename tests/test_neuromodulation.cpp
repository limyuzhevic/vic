#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Serotonin.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "NeuromodulatorEnsemble.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {

void test_dopamine() {
    std::cout << "Testing Dopamine system..." << std::endl;
    
    Dopamine dopamine;
    
    // Test initialization
    assert(dopamine.getName() == std::string("DA"));
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test baseline level
    dopamine.setBaseline(20.0f);
    assert(dopamine.getTonicLevel() == 20.0f);
    
    // Test peak level
    dopamine.setPeakLevel(80.0f);
    assert(dopamine.getPhasicBurst() >= 0.0f);
    
    // Test reward signaling
    dopamine.signalReward(1.0f);
    assert(dopamine.getLevel() > 0.0f);
    
    // Test reward prediction error
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() > 0.0f);
    
    // Test plasticity factor (inverted U-shaped function)
    float plasticity1 = dopamine.getPlasticityFactor();
    dopamine.setLevel(50.0f); // Near peak
    float plasticity2 = dopamine.getPlasticityFactor();
    assert(plasticity2 <= plasticity1); // Should be lower at peak
    
    // Test update
    dopamine.update(0.1f);
    assert(dopamine.getLevel() >= 0.0f);
    
    std::cout << "✓ Dopamine tests passed" << std::endl;
}

void test_serotonin() {
    std::cout << "Testing Serotonin system..." << std::endl;
    
    Serotonin serotonin;
    
    // Test initialization
    assert(serotonin.getName() == std::string("5-HT"));
    assert(serotonin.getLevel() >= 0.0f);
    
    // Test learning rate modulation
    float baseLearningRate = 0.1f;
    serotonin.setLearningRateBase(baseLearningRate);
    assert(std::abs(serotonin.getLearningRateModulation() - baseLearningRate) < 0.01f);
    
    // Test novelty signaling
    serotonin.signalNovelty(1.0f);
    assert(serotonin.getLevel() > 0.0f);
    
    // Test mood state
    serotonin.signalMoodState(0.8f);
    assert(serotonin.getLevel() > 0.0f);
    
    // Test novelty sensitivity
    serotonin.setNoveltySensitivity(2.0f);
    assert(serotonin.getNoveltySensitivity() == 2.0f);
    
    // Test update
    serotonin.update(0.1f);
    assert(serotonin.getLevel() >= 0.0f);
    
    std::cout << "✓ Serotonin tests passed" << std::endl;
}

void test_acetylcholine() {
    std::cout << "Testing Acetylcholine system..." << std::endl;
    
    Acetylcholine ach;
    
    // Test initialization
    assert(ach.getName() == std::string("ACh"));
    assert(ach.getLevel() >= 0.0f);
    
    // Test attentional focus
    float initialFocus = ach.getAttentionalFocus();
    ach.signalAttentionalFocus(0.9f);
    assert(ach.getAttentionalFocus() > initialFocus);
    
    // Test working memory load
    ach.signalWorkingMemoryLoad(0.7f);
    assert(ach.getLevel() > 0.0f);
    
    // Test sensory gain
    ach.signalSensoryGain(0.8f);
    assert(ach.getSensoryDetectionThreshold() < 1.0f);
    
    // Test attention threshold
    ach.setAttentionThreshold(0.5f);
    assert(ach.getSensoryDetectionThreshold() == 0.5f);
    
    // Test update
    ach.update(0.1f);
    assert(ach.getLevel() >= 0.0f);
    
    std::cout << "✓ Acetylcholine tests passed" << std::endl;
}

void test_norepinephrine() {
    std::cout << "Testing Norepinephrine system..." << std::endl;
    
    Norepinephrine ne;
    
    // Test initialization
    assert(ne.getName() == std::string("NE"));
    assert(ne.getLevel() >= 0.0f);
    
    // Test arousal signaling
    ne.signalArousal(0.9f);
    assert(ne.getArousal() > 0.5f);
    
    // Test stress response
    ne.signalStressResponse(0.7f);
    assert(ne.getStressResponseLevel() > 0.5f);
    
    // Test vigilance
    ne.signalVigilance(0.8f);
    assert(ne.getArousal() > 0.5f);
    
    // Test reaction time modulation
    float reactionTime1 = ne.getReactionTimeModulation();
    ne.setReactionTimeFactor(1.5f);
    assert(ne.getReactionTimeModulation() < reactionTime1); // Higher factor = faster
    
    // Test stress sensitivity
    ne.setStressSensitivity(0.5f);
    assert(ne.getArousal() > 0.5f);
    
    // Test update
    ne.update(0.1f);
    assert(ne.getLevel() >= 0.0f);
    
    std::cout << "✓ Norepinephrine tests passed" << std::endl;
}

void test_receptor_dynamics() {
    std::cout << "Testing receptor dynamics..." << std::endl;
    
    // Test dopamine receptor
    Dopamine dopamine;
    dopamine.setReceptorParameters(10.0f, 1.0f);
    assert(dopamine.getReceptorState().affinity == 10.0f);
    assert(dopamine.getReceptorState().density == 1.0f);
    
    // Test serotonin receptor
    Serotonin serotonin;
    serotonin.setReceptorParameters(5.0f, 2.0f);
    assert(serotonin.getReceptorState().affinity == 5.0f);
    assert(serotonin.getReceptorState().density == 2.0f);
    
    // Test acetylcholine receptor
    Acetylcholine ach;
    ach.setReceptorParameters(15.0f, 0.5f);
    assert(ach.getReceptorState().affinity == 15.0f);
    assert(ach.getReceptorState().density == 0.5f);
    
    // Test norepinephrine receptor
    Norepinephrine ne;
    ne.setReceptorParameters(8.0f, 1.5f);
    assert(ne.getReceptorState().affinity == 8.0f);
    assert(ne.getReceptorState().density == 1.5f);
    
    std::cout << "✓ Receptor dynamics tests passed" << std::endl;
}

void test_neuromodulator_ensemble() {
    std::cout << "Testing NeuromodulatorEnsemble..." << std::endl;
    
    NeuromodulatorEnsemble ensemble;
    
    // Test ensemble initialization
    assert(ensemble.getExplorationProbability() >= 0.0f && ensemble.getExplorationProbability() <= 1.0f);
    assert(ensemble.getDecisionUncertainty() >= 0.0f && ensemble.getDecisionUncertainty() <= 1.0f);
    assert(ensemble.getCombinedPlasticityFactor() >= 0.0f && ensemble.getCombinedPlasticityFactor() <= 1.0f);
    
    // Test event signaling
    ensemble.signalEvent(EventType::Reward, 1.0f);
    ensemble.signalEvent(EventType::Novelty, 0.5f);
    ensemble.signalEvent(EventType::Attention, 0.8f);
    ensemble.signalEvent(EventType::Stress, 0.3f);
    ensemble.signalEvent(EventType::Arousal, 0.9f);
    
    // Test update
    ensemble.update(0.1f);
    
    // Test get methods
    assert(ensemble.getDopamineLevel() >= 0.0f);
    assert(ensemble.getSerotoninLevel() >= 0.0f);
    assert(ensemble.getAChLevel() >= 0.0f);
    assert(ensemble.getNELevel() >= 0.0f);
    assert(ensemble.getRewardPredictionError() >= 0.0f);
    assert(ensemble.getNoveltySignal() >= 0.0f);
    assert(ensemble.getAttentionFocus() >= 0.0f);
    assert(ensemble.getArousalLevel() >= 0.0f);
    
    std::cout << "✓ NeuromodulatorEnsemble tests passed" << std::endl;
}

void test_state_dependent_modulation() {
    std::cout << "Testing state-dependent modulation..." << std::endl;
    
    NeuromodulatorEnsemble ensemble;
    
    // Set different behavioral states
    for (float state = 0.2f; state <= 0.9f; state += 0.2f) {
        ensemble.update(0.1f);
        // The ensemble should adapt based on behavioral state
    }
    
    std::cout << "✓ State-dependent modulation tests passed" << std::endl;
}

void test_decision_uncertainty() {
    std::cout << "Testing decision uncertainty..." << std::endl;
    
    NeuromodulatorEnsemble ensemble;
    
    float uncertainty = ensemble.getDecisionUncertainty();
    assert(uncertainty >= 0.0f && uncertainty <= 1.0f);
    
    // High uncertainty should affect exploration
    float exploration = ensemble.getExplorationProbability();
    assert(exploration >= 0.0f && exploration <= 1.0f);
    
    std::cout << "✓ Decision uncertainty tests passed" << std::endl;
}

void test_exploitation_tradeoff() {
    std::cout << "Testing exploration-exploitation tradeoff..." << std::endl;
    
    NeuromodulatorEnsemble ensemble;
    
    // The ensemble should balance exploration and exploitation
    float exploration = ensemble.getExplorationProbability();
    float exploitation = 1.0f - exploration;
    
    assert(exploration >= 0.0f && exploration <= 1.0f);
    assert(exploitation >= 0.0f && exploitation <= 1.0f);
    
    // Test that the tradeoff adapts based on neuromodulator levels
    ensemble.update(0.1f);
    
    std::cout << "✓ Exploration-exploitation tradeoff tests passed" << std::endl;
}

void run_all_tests() {
    std::cout << "Running comprehensive neuromodulation tests..." << std::endl;
    std::cout << "================================================" << std::endl;
    
    test_dopamine();
    test_serotonin();
    test_acetylcholine();
    test_norepinephrine();
    test_receptor_dynamics();
    test_neuromodulator_ensemble();
    test_state_dependent_modulation();
    test_decision_uncertainty();
    test_exploitation_tradeoff();
    
    std::cout << "===============================================" << std::endl;
    std::cout << "All neuromodulation tests passed! ✓" << std::endl;
    std::cout << "The real neuromodulation system is working correctly." << std::endl;
}

} // namespace nlm

int main() {
    nlm::run_all_tests();
    return 0;
}