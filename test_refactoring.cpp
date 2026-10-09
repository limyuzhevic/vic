#include "SpikeHistoryManager.hpp"
#include "RefractoryState.hpp"
#include "SynapseBaseClasses.hpp"
#include "NeuralUtils.hpp"

namespace nlm {

// Comprehensive test for refactoring backward compatibility
test_refactoring_backward_compatibility() {
    // Test SpikeHistoryManager
    SpikeHistoryManager spikeHistory(50);
    
    // Test basic spike history operations
    spikeHistory.recordSpike(1.0);
    spikeHistory.recordSpike(2.0);
    spikeHistory.recordSpike(3.0);
    
    ASSERT_EQ(spikeHistory.getHistorySize(), 3);
    ASSERT_EQ(spikeHistory.getSpikeHistory().size(), 3);
    ASSERT_FALSE(spikeHistory.isEmpty());
    
    // Test history overflow
    for (int i = 4; i < 60; ++i) {
        spikeHistory.recordSpike(static_cast<Timestamp>(i));
    }
    
    ASSERT_EQ(spikeHistory.getHistorySize(), 50); // Max size
    
    // Test RefractoryState
    RefractoryState refractory(10);
    ASSERT_TRUE(refractory.isActive());
    ASSERT_EQ(refractory.getRemainingSteps(), 10);
    
    ASSERT_TRUE(refractory.decrement());
    ASSERT_EQ(refractory.getRemainingSteps(), 9);
    
    for (int i = 0; i < 8; ++i) {
        ASSERT_TRUE(refractory.decrement());
    }
    
    ASSERT_TRUE(refractory.decrement());
    ASSERT_EQ(refractory.getRemainingSteps(), 0);
    ASSERT_FALSE(refractory.isActive());
    
    // Test setRefractoryPeriod
    refractory.setRefractoryPeriod(20);
    ASSERT_EQ(refractory.getRefractoryPeriod(), 20);
    
    // Test NeuralUtils
    float clamped = NeuralUtils::clamp(15.0f, 10.0f, 20.0f);
    ASSERT_EQ(clamped, 15.0f);
    
    float lerped = NeuralUtils::lerp(10.0f, 20.0f, 0.5f);
    ASSERT_EQ(lerped, 15.0f);
    
    // Test NeuronSpikeHistoryManager
    NeuronSpikeHistoryManager neuronHistory;
    neuronHistory.recordPreSpike(1.0);
    neuronHistory.recordPostSpike(2.0);
    
    ASSERT_EQ(neuronHistory.getPreSpikeHistorySize(), 1);
    ASSERT_EQ(neuronHistory.getPostSpikeHistorySize(), 1);
    
    // Test SynapseEfficacyManager
    SynapseEfficacyManager efficacy;
    ASSERT_EQ(efficacy.getEfficacy(), 1.0f);
    
    efficacy.setEfficacy(0.5f);
    ASSERT_EQ(efficacy.getEfficacy(), 0.5f);
    
    // Test SynapseEligibilityTraceManager
    SynapseEligibilityTraceManager trace;
    ASSERT_EQ(trace.getEligibilityTrace(), 0.0f);
    
    trace.setEligibilityTrace(0.8f);
    trace.decayEligibilityTrace(0.1f);
    ASSERT_NEAR(trace.getEligibilityTrace(), 0.72f, 0.01f);
}

} // namespace nlm
