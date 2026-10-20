// Dopamine (Reward) Tests
#include "neuromodulation/Reward.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Reward dopamine;
    
    assert(dopamine.getValue() == 0.0f);
    assert(dopamine.getHistory().empty());
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineValue() {
    nlm::Reward dopamine;
    
    // Test initial value
    assert(dopamine.getValue() == 0.0f);
    
    // Set value
    dopamine.setValue(0.5f);
    assert(dopamine.getValue() == 0.5f);
    
    // Add to value
    dopamine.add(0.3f);
    assert(dopamine.getValue() == 0.8f);
    
    // Add negative value
    dopamine.add(-0.2f);
    assert(dopamine.getValue() == 0.6f);
    
    // Reset
    dopamine.reset();
    assert(dopamine.getValue() == 0.0f);
    
    std::cout << "    testDopamineValue passed" << std::endl;
}

void testDopamineHistory() {
    nlm::Reward dopamine;
    
    assert(dopamine.getHistory().empty());
    
    // Set and track history
    dopamine.setValue(0.1f);
    assert(dopamine.getHistory().size() == 1);
    assert(dopamine.getHistory()[0] == 0.1f);
    
    dopamine.setValue(0.2f);
    assert(dopamine.getHistory().size() == 2);
    assert(dopamine.getHistory()[1] == 0.2f);
    
    // Clear history
    dopamine.clearHistory();
    assert(dopamine.getHistory().empty());
    
    std::cout << "    testDopamineHistory passed" << std::endl;
}

void testDopamineComputeReward() {
    nlm::Reward dopamine;
    
    // Test computeReward method (placeholder implementation)
    // Since it's marked as TODO PHASE 2, we'll just test that it compiles
    // and returns something (may be default 0.0)
    
    nlm::Observation observation;
    observation.intensity = 0.5f;
    observation.spatialX = 1.0f;
    observation.spatialY = 2.0f;
    
    float computed = dopamine.computeReward(observation);
    assert(computed >= 0.0f);  // Should be non-negative
    
    std::cout << "    testDopamineComputeReward passed" << std::endl;
}

void testDopamineAccumulation() {
    nlm::Reward dopamine;
    
    // Test multiple additions
    dopamine.add(1.0f);
    assert(dopamine.getValue() == 1.0f);
    
    dopamine.add(2.5f);
    assert(dopamine.getValue() == 3.5f);
    
    dopamine.add(-1.0f);
    assert(dopamine.getValue() == 2.5f);
    
    // Test setValue overrides additions
    dopamine.setValue(0.0f);
    assert(dopamine.getValue() == 0.0f);
    
    std::cout << "    testDopamineAccumulation passed" << std::endl;
}

void testDopamineReset() {
    nlm::Reward dopamine;
    
    // Set some history
    dopamine.setValue(0.5f);
    dopamine.add(0.3f);
    
    assert(dopamine.getValue() == 0.8f);
    assert(!dopamine.getHistory().empty());
    
    // Reset should clear both value and history
    dopamine.reset();
    assert(dopamine.getValue() == 0.0f);
    assert(dopamine.getHistory().empty());
    
    std::cout << "    testDopamineReset passed" << std::endl;
}

void testDopamineSystemIntegration() {
    nlm::Reward dopamine;
    
    // Test multiple operations to ensure system integrity
    std::vector<float> testValues = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
    
    for (size_t i = 0; i < testValues.size(); ++i) {
        dopamine.setValue(testValues[i]);
        assert(dopamine.getValue() == testValues[i]);
        assert(dopamine.getHistory().size() == i + 1);
        assert(dopamine.getHistory()[i] == testValues[i]);
    }
    
    // Reset and verify
    dopamine.reset();
    assert(dopamine.getValue() == 0.0f);
    assert(dopamine.getHistory().empty());
    
    std::cout << "    testDopamineSystemIntegration passed" << std::endl;
}

void runAll() {
    testDopamineCreation();
    testDopamineValue();
    testDopamineHistory();
    testDopamineComputeReward();
    testDopamineAccumulation();
    testDopamineReset();
    testDopamineSystemIntegration();
}

} // namespace test_dopamine
