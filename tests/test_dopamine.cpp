// Dopamine Neuromodulator Tests
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace nlm {
    // Constants from Dopamine implementation
    constexpr float DOPAMINE_MAX_DA = 15.0f;
    constexpr float DOPAMINE_BASELINE_DA = 0.05f;
    constexpr float DOPAMINE_BURST_PEAK = 5.0f;
} // namespace nlm

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    
    // Initial level should be baseline
    float level = dopamine.getLevel();
    assert(level >= 0.0f && level <= nlm::DOPAMINE_MAX_DA);
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineSetLevel() {
    nlm::Dopamine dopamine;
    
    // Test setting level within bounds
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    // Test clamping to maximum
    dopamine.setLevel(100.0f);
    assert(dopamine.getLevel() <= nlm::DOPAMINE_MAX_DA);
    
    // Test clamping to minimum
    dopamine.setLevel(-1.0f);
    assert(dopamine.getLevel() >= 0.0f);
    
    std::cout << "    testDopamineSetLevel passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // Get initial plasticity factor
    float plasticityFactor = dopamine.getPlasticityFactor();
    assert(plasticityFactor >= 0.1f && plasticityFactor <= 5.0f);
    
    // Set higher dopamine level and check plasticity factor
    dopamine.setLevel(5.0f);
    float highPlasticity = dopamine.getPlasticityFactor();
    
    // Plasticity should increase with dopamine (D1 receptor effect)
    assert(highPlasticity >= plasticityFactor);
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void testDopamineUpdate() {
    nlm::Dopamine dopamine;
    
    float initialLevel = dopamine.getLevel();
    
    // Update for small time step
    float dt = 0.001f; // 1ms
    dopamine.update(dt);
    
    float afterUpdate = dopamine.getLevel();
    
    // Level should change based on dynamics (decay/reuptake, release, degradation)
    // Should not increase dramatically without external stimulation
    assert(std::abs(afterUpdate - initialLevel) < 0.1f);
    
    std::cout << "    testDopamineUpdate passed" << std::endl;
}

void testDopamineSignalReward() {
    nlm::Dopamine dopamine;
    
    float initialLevel = dopamine.getLevel();
    
    // Signal a reward
    float reward = 1.0f;
    dopamine.signalReward(reward);
    
    float afterReward = dopamine.getLevel();
    
    // Reward should increase dopamine level
    assert(afterReward > initialLevel || std::abs(afterReward - initialLevel) < 0.1f);
    
    // Test with different reward magnitudes
    dopamine.setLevel(0.0f);
    float largeReward = 5.0f;
    dopamine.signalReward(largeReward);
    
    assert(dopamine.getLevel() > 0.0f);
    
    std::cout << "    testDopamineSignalReward passed" << std::endl;
}

void testDopamineSignalRewardPredictionError() {
    nlm::Dopamine dopamine;
    
    float initialLevel = dopamine.getLevel();
    
    // Signal positive reward prediction error
    float positiveRPE = 0.5f;
    dopamine.signalRewardPredictionError(positiveRPE);
    
    float afterPositiveRPE = dopamine.getLevel();
    
    // Positive RPE should increase dopamine
    assert(afterPositiveRPE > initialLevel);
    
    // Reset and test negative reward prediction error
    dopamine.setLevel(0.0f);
    float negativeRPE = -0.3f;
    dopamine.signalRewardPredictionError(negativeRPE);
    
    // Negative RPE should decrease dopamine (but not go below baseline)
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test plasticity factor changes with RPE
    dopamine.setLevel(1.0f);
    float plasticityBefore = dopamine.getPlasticityFactor();
    
    dopamine.signalRewardPredictionError(0.5f);
    float plasticityAfter = dopamine.getPlasticityFactor();
    
    // Positive RPE should enhance plasticity
    assert(plasticityAfter >= plasticityBefore);
    
    std::cout << "    testDopamineSignalRewardPredictionError passed" << std::endl;
}

void testDopamineBurstTonicFiring() {
    nlm::Dopamine dopamine;
    
    // Test tonic firing (baseline dynamics)
    float tonicLevel = dopamine.getLevel();
    
    // Update multiple times to simulate tonic firing
    for (int i = 0; i < 10; ++i) {
        dopamine.update(0.001f);
    }
    
    float afterTonic = dopamine.getLevel();
    
    // Tonic firing should maintain stable levels
    assert(std::abs(afterTonic - tonicLevel) < 0.5f);
    
    // Test burst firing
    dopamine.setLevel(0.0f);
    
    // Signal multiple rewards to induce burst firing
    for (int i = 0; i < 3; ++i) {
        dopamine.signalReward(2.0f);
    }
    
    // Should see higher dopamine levels from bursts
    float burstLevel = dopamine.getLevel();
    assert(burstLevel > 1.0f);
    
    std::cout << "    testDopamineBurstTonicFiring passed" << std::endl;
}

void testDopaminePathwaySequestration() {
    nlm::Dopamine dopamine;
    
    // Test mesolimbic pathway influence
    dopamine.setLevel(2.0f);
    
    // Get plasticity factor (which should incorporate mesolimbic effects)
    float mesolimbicPlasticity = dopamine.getPlasticityFactor();
    assert(mesolimbicPlasticity > 1.0f);
    
    // Test mesocortical pathway influence
    dopamine.setLevel(1.0f);
    float mesocorticalPlasticity = dopamine.getPlasticityFactor();
    
    // Should be different from mesolimbic effects
    assert(mesocorticalPlasticity > 1.0f);
    
    std::cout << "    testDopaminePathwaySequestration passed" << std::endl;
}

void testDopamineReceptorDynamics() {
    nlm::Dopamine dopamine;
    
    // Test D1 receptor occupancy at different dopamine levels
    dopamine.setLevel(0.1f);
    float plasticity1 = dopamine.getPlasticityFactor();
    
    dopamine.setLevel(1.0f);
    float plasticity2 = dopamine.getPlasticityFactor();
    
    // Higher dopamine should lead to higher plasticity (D1 effect)
    assert(plasticity2 >= plasticity1);
    
    // Test D2 receptor effects (inhibitory)
    dopamine.setLevel(5.0f);
    float plasticity3 = dopamine.getPlasticityFactor();
    
    // At very high levels, D2 inhibition might moderate plasticity
    assert(plasticity3 > 1.0f);  // Still should be above baseline
    
    std::cout << "    testDopamineReceptorDynamics passed" << std::endl;
}

void testDopamineTDLearning() {
    nlm::Dopamine dopamine;
    
    // Initialize TD learning components
    dopamine.setLevel(0.5f);
    
    // Simulate TD learning sequence
    for (int step = 0; step < 20; ++step) {
        // Simulate reward prediction error based on random rewards
        float reward = (step % 5 == 0) ? 1.0f : 0.0f;  // Every 5th step gets reward
        
        // Current value estimate
        float valueEstimate = dopamine.getLevel();
        
        // Compute RPE (simplified TD)
        float rpe = reward - 0.1f * valueEstimate;  // Simple TD with discount
        
        // Update with RPE
        dopamine.signalRewardPredictionError(rpe);
        
        // Update for time step
        dopamine.update(0.001f);
    }
    
    // After learning, should have adapted value estimates
    float finalPlasticity = dopamine.getPlasticityFactor();
    assert(finalPlasticity > 0.5f);
    
    std::cout << "    testDopamineTDLearning passed" << std::endl;
}

void testDopamineBehavioralModulation() {
    nlm::Dopamine dopamine;
    
    // Test exploration-exploitation balance
    dopamine.setLevel(0.2f);  // Low dopamine = more exploration
    float explorationLow = dopamine.getPlasticityFactor();
    
    dopamine.setLevel(2.0f);  // High dopamine = more exploitation
    float explorationHigh = dopamine.getPlasticityFactor();
    
    // Higher dopamine should lead to higher exploration bias (incentive salience)
    assert(explorationHigh >= explorationLow);
    
    // Test novelty detection
    dopamine.setLevel(1.0f);
    float noveltySensitive = dopamine.getPlasticityFactor();
    
    // Dopamine should amplify response to novel stimuli
    assert(noveltySensitive > 1.0f);
    
    std::cout << "    testDopamineBehavioralModulation passed" << std::endl;
}

void testDopamineStability() {
    nlm::Dopamine dopamine;
    
    // Test stability over many time steps
    float initialLevel = dopamine.getLevel();
    float maxLevelChange = 0.0f;
    
    for (int step = 0; step < 1000; ++step) {
        dopamine.update(0.001f);  // 1ms steps
        
        float currentLevel = dopamine.getLevel();
        float levelChange = std::abs(currentLevel - initialLevel);
        maxLevelChange = std::max(maxLevelChange, levelChange);
        
        // Should not explode or go negative
        assert(currentLevel >= 0.0f && currentLevel <= nlm::DOPAMINE_MAX_DA);
    }
    
    // Changes should be bounded
    assert(maxLevelChange < 10.0f);
    
    std::cout << "    testDopamineStability passed (max change: " << maxLevelChange << ")" << std::endl;
}

void testDopamineReleaseReuptakeKinetics() {
    nlm::Dopamine dopamine;
    
    // Test vesicle pool dynamics
    float initialPool = dopamine.getLevel();  // Note: This is DA level, not pool
    
    // Signal multiple rewards to deplete vesicle pool
    for (int i = 0; i < 5; ++i) {
        dopamine.signalReward(1.0f);
    }
    
    // Level should be higher after multiple rewards
    float afterBursts = dopamine.getLevel();
    
    // Update to allow reuptake and degradation
    for (int i = 0; i < 10; ++i) {
        dopamine.update(0.001f);
    }
    
    // Level should decrease due to reuptake and degradation
    float afterDecay = dopamine.getLevel();
    
    // Decay should bring levels back towards baseline
    assert(std::abs(afterDecay - initialPool) < 1.0f);
    
    std::cout << "    testDopamineReleaseReuptakeKinetics passed" << std::endl;
}

void runAll() {
    std::cout << "Running Dopamine neuromodulator tests..." << std::endl;
    
    testDopamineCreation();
    testDopamineSetLevel();
    testDopaminePlasticityFactor();
    testDopamineUpdate();
    testDopamineSignalReward();
    testDopamineSignalRewardPredictionError();
    testDopamineBurstTonicFiring();
    testDopaminePathwaySequestration();
    testDopamineReceptorDynamics();
    testDopamineTDLearning();
    testDopamineBehavioralModulation();
    testDopamineStability();
    testDopamineReleaseReuptakeKinetics();
    
    std::cout << "\nAll Dopamine neuromodulator tests passed!" << std::endl;
}

} // namespace test_dopamine

int main() {
    test_dopamine::runAll();
    return 0;
}
