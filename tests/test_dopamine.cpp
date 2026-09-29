// Dopamine Tests
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_dopamine {

void testDopamineBasic() {
    nlm::Dopamine dopamine;
    
    // Test initialization
    assert(dopamine.getName() == std::string("DA"));
    assert(dopamine.getLevel() == 0.0f); // Default level
    
    // Test setting level
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    // Test level clamping
    dopamine.setLevel(1.5f); // Should be clamped to 1.0
    assert(dopamine.getLevel() == 1.0f);
    
    dopamine.setLevel(-0.5f); // Should be clamped to 0.0
    assert(dopamine.getLevel() == 0.0f);
    
    // Test plasticity factor
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.0f);
    assert(std::abs(plasticity - (0.5f + 0.5f * 0.0f)) < 0.01f); // Based on level=0.0
    
    std::cout << "    testDopamineBasic passed" << std::endl;
}

void testDopamineRewardSignaling() {
    nlm::Dopamine dopamine;
    
    // Test reward signaling
    dopamine.signalReward(0.3f);
    assert(dopamine.getLevel() > 0.0f);
    
    // Test reward prediction error signaling
    dopamine.signalRewardPredictionError(0.2f);
    assert(dopamine.getLevel() > 0.0f);
    
    // Test high reward
    dopamine.setLevel(0.0f);
    dopamine.signalReward(1.0f);
    assert(dopamine.getLevel() <= 1.0f); // Should be clamped
    
    // Test prediction error with negative values
    dopamine.setLevel(0.0f);
    dopamine.signalRewardPredictionError(-0.5f);
    assert(dopamine.getLevel() == 0.0f); // Negative prediction error should give 0.0
    
    std::cout << "    testDopamineRewardSignaling passed" << std::endl;
}

void testDopamineUpdate() {
    nlm::Dopamine dopamine;
    
    // Set initial level
    dopamine.setLevel(0.8f);
    
    // Update with small time step
    float initialLevel = dopamine.getLevel();
    dopamine.update(0.001f);
    assert(dopamine.getLevel() <= initialLevel); // Should decay or stay same
    
    // Update with baseline
    dopamine.setLevel(0.3f);
    float baseline = 0.1f;
    // Note: The actual baseline is internal, we can't set it directly
    // This test just verifies the update doesn't crash
    
    std::cout << "    testDopamineUpdate passed" << std::endl;
}

void testDopamineIntegration() {
    nlm::Dopamine dopamine;
    
    // Test integration with Neuromodulator interface
    nlm::Neuromodulator* neuromod = &dopamine;
    
    assert(neuromod->getName() == std::string("DA"));
    assert(neuromod->getLevel() == 0.0f);
    
    neuromod->setLevel(0.6f);
    assert(neuromod->getLevel() == 0.6f);
    
    // Test polymorphism
    float plasticity = neuromod->getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.0f);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void runAll() {
    testDopamineBasic();
    testDopamineRewardSignaling();
    testDopamineUpdate();
    testDopamineIntegration();
}

} // namespace test_dopamine

int main() {
    std::cout << "=== Dopamine Neuromodulator Tests ===" << std::endl;
    
    test_dopamine::runAll();
    
    std::cout << "\n=== All Dopamine Tests Passed! ===" << std::endl;
    return 0;
}
