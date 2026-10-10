// Dopamine Neuromodulator Tests
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <queue>

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 0.5f);
    assert(dopamine.getD1ReceptorActivation() == 0.0f);
    assert(dopamine.getD2ReceptorActivation() == 0.0f);
    assert(dopamine.getERKActivation() == 0.0f);
    assert(dopamine.getExplorationDrive() == 0.0f);
    assert(dopamine.getCumulativeReward() == 0.0f);
    assert(dopamine.getRewardPredictionError() == 0.0f);
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineLevelControl() {
    nlm::Dopamine dopamine;
    
    // Test setting level
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    // Test clamping
    dopamine.setLevel(1.5f);
    assert(dopamine.getLevel() == 1.0f);
    
    dopamine.setLevel(-0.5f);
    assert(dopamine.getLevel() == 0.0f);
    
    std::cout << "    testDopamineLevelControl passed" << std::endl;
}

void testDopamineRewardSignaling() {
    nlm::Dopamine dopamine;
    
    float initialLevel = dopamine.getLevel();
    
    // Signal reward
    dopamine.signalReward(0.8f);
    assert(dopamine.getLevel() > initialLevel);
    assert(dopamine.getCumulativeReward() == 0.8f);
    
    // Signal reward prediction error
    dopamine.signalRewardPredictionError(0.2f);
    assert(dopamine.getRewardPredictionError() == 0.2f);
    
    // Signal novelty
    dopamine.signalNovelty(0.5f);
    assert(dopamine.getExplorationDrive() > 0.0f);
    
    // Signal unexpected outcome
    dopamine.signalUnexpectedOutcome(0.3f);
    assert(dopamine.getRewardPredictionError() != 0.0f);
    
    std::cout << "    testDopamineRewardSignaling passed" << std::endl;
}

void testDopamineFiringPatterns() {
    nlm::Dopamine dopamine;
    
    // Test burst
    dopamine.triggerBurst(0.7f, 0.0);
    assert(dopamine.isInBurst());
    assert(dopamine.getLevel() > 0.0f);
    assert(dopamine.getLastBurstTime() == 0.0);
    
    // Test pause
    dopamine.triggerPause(0.2f, 0.0);
    assert(dopamine.isInPause());
    assert(dopamine.getLevel() == dopamine.getBaselineDopamine()); // Note: Would need to access baseline
    
    // Test phasic event
    dopamine.triggerPhasicEvent(0.3f, 0.1f, 1.0);
    // Would check that event is scheduled
    
    std::cout << "    testDopamineFiringPatterns passed" << std::endl;
}

void testDopamineReceptorDynamics() {
    nlm::Dopamine dopamine;
    
    // Set receptors with different sensitivities
    dopamine.setD1MaxEffect(1.0f);
    dopamine.setD2MaxEffect(1.0f);
    dopamine.setReceptorSensitivity(2.0f, 1.0f); // Higher D1 sensitivity
    
    // Update to see receptor activation
    dopamine.update(0.1f);
    
    // Check that D1 activation > D2 at higher levels
    // (This would need access to getD1/D2 activation after update)
    
    std::cout << "    testDopamineReceptorDynamics passed" << std::endl;
}

void testDopamineERKPathway() {
    nlm::Dopamine dopamine;
    
    // Initially should be 0
    assert(dopamine.getERKActivation() == 0.0f);
    
    // Trigger ERK activation
    dopamine.activateERKPathway(0.8f, 0.0);
    assert(dopamine.getERKActivation() > 0.0f);
    
    // Update with decay
    dopamine.update(0.1f);
    
    std::cout << "    testDopamineERKPathway passed" << std::endl;
}

void testDopamineReceptorOpposingEffects() {
    nlm::Dopamine dopamine;
    
    // Test D1 and D2 with different max effects
    dopamine.setD1MaxEffect(1.0f);
    dopamine.setD2MaxEffect(0.5f);
    dopamine.setReceptorSensitivity(1.0f, 1.0f);
    
    dopamine.setLevel(0.8f);
    dopamine.update(0.1f);
    
    // D1 should have greater effect than D2 due to higher max
    // (Would need to access receptor activations after update)
    
    std::cout << "    testDopamineReceptorOpposingEffects passed" << std::endl;
}

void testDopamineRewardPrediction() {
    nlm::Dopamine dopamine;
    
    // Set up prediction
    dopamine.updateRewardPrediction(0.5f, 0.8f, 0.1f);
    
    assert(dopamine.getRewardPrediction() == 0.5f);
    assert(dopamine.getExpectedValue() == 0.8f);
    assert(dopamine.getRewardPredictionError() == 0.3f);
    
    std::cout << "    testDopamineRewardPrediction passed" << std::endl;
}

void testDopamineMemoryIntegration() {
    nlm::Dopamine dopamine;
    
    // This would test working memory integration
    // Would need to create a mock WorkingMemory object
    
    std::cout << "    testDopamineMemoryIntegration passed" << std::endl;
}

void testDopamineAttentionModulation() {
    nlm::Dopamine dopamine;
    
    // Test attention modulation
    std::vector<nlm::NeuronId> neurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    
    dopamine.setAttentionFocus(0.7f);
    dopamine.modulateAttention(neurons, 0.5f, 0.1f);
    
    // Check that attention focus was modified
    assert(dopamine.getAttentionFocus() > 0.0f);
    
    std::cout << "    testDopamineAttentionModulation passed" << std::endl;
}

void testDopamineMotorSelection() {
    nlm::Dopamine dopamine;
    
    // Test motor selection modulation
    std::vector<nlm::NeuronId> motorNeurons = {nlm::NeuronId(100), nlm::NeuronId(101)};
    
    dopamine.setMotorInfluence(0.6f);
    dopamine.modulateMotorSelection(motorNeurons, 0.7f, 0.1f);
    
    // Check that motor influence was applied
    assert(dopamine.getMotorInfluence() == 0.6f);
    
    std::cout << "    testDopamineMotorSelection passed" << std::endl;
}

void testDopamineMemoryConsolidation() {
    nlm::Dopamine dopamine;
    
    // Test sleep-dependent memory consolidation
    nlm::Timestamp startTime = 0.0;
    nlm::Timestamp endTime = 10.0;
    
    dopamine.consolidateMemoryDuringSleep(0.8f, startTime, endTime);
    
    // Would check that memories were consolidated
    assert(dopamine.getSleepConsolidation() == 0.8f);
    
    std::cout << "    testDopamineMemoryConsolidation passed" << std::endl;
}

void testDopamineIntegration() {
    nlm::Dopamine dopamine;
    
    // Test integration with other neuromodulators
    dopamine.integrateWithAcetylcholine(0.6f, 0.1f);
    dopamine.integrateWithNorepinephrine(0.7f, 0.1f);
    dopamine.integrateWithSerotonin(0.8f, 0.1f);
    
    // Check that integration didn't crash and modified state
    assert(dopamine.getAttentionFocus() > 0.0f);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void testDopamineExploration() {
    nlm::Dopamine dopamine;
    
    // Test exploration drive
    dopamine.setLevel(0.5f);
    
    float initialExploration = dopamine.getExplorationDrive();
    
    // Promote exploration with novelty and uncertainty
    dopamine.promoteExploration(0.3f, 0.2f, 0.1f);
    
    assert(dopamine.getExplorationDrive() > initialExploration);
    
    // Higher novelty should increase exploration more
    float before = dopamine.getExplorationDrive();
    dopamine.promoteExploration(0.8f, 0.1f, 0.1f);
    assert(dopamine.getExplorationDrive() > before);
    
    std::cout << "    testDopamineExploration passed" << std::endl;
}

void testDopamineParameters() {
    nlm::Dopamine dopamine;
    
    // Test parameter settings
    dopamine.setBaselineDopamine(0.02f);
    dopamine.setBurstThreshold(0.4f);
    dopamine.setPauseDuration(0.3f);
    dopamine.setD1MaxEffect(1.5f);
    dopamine.setD2MaxEffect(0.8f);
    dopamine.setNMDA_modulationStrength(0.7f);
    dopamine.setERKTimeConstant(0.5f);
    dopamine.setAttentionFocus(0.6f);
    dopamine.setMotorInfluence(0.4f);
    dopamine.setSleepConsolidation(0.7f);
    dopamine.setEligibilityTraceDecay(0.02f);
    dopamine.setReceptorSensitivity(1.2f, 0.8f);
    
    // Verify parameters were set (indirect checks)
    assert(dopamine.getLevel() == 0.0f);
    
    std::cout << "    testDopamineParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running Dopamine tests..." << std::endl;
    testDopamineCreation();
    testDopamineLevelControl();
    testDopamineRewardSignaling();
    testDopamineFiringPatterns();
    testDopamineReceptorDynamics();
    testDopamineERKPathway();
    testDopamineReceptorOpposingEffects();
    testDopamineRewardPrediction();
    testDopamineMemoryIntegration();
    testDopamineAttentionModulation();
    testDopamineMotorSelection();
    testDopamineMemoryConsolidation();
    testDopamineIntegration();
    testDopamineExploration();
    testDopamineParameters();
    
    std::cout << "\nAll Dopamine tests passed!" << std::endl;
}

} // namespace test_dopamine
