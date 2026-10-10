// Dopamine Tests
#include "neuromodulation/Dopamine.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 0.5f);
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineConfiguration() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 0.5f);
    
    std::cout << "    testDopamineConfiguration passed" << std::endl;
}

void testDopamineLevelSetting() {
    nlm::Dopamine dopamine;
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(1.0f);
    assert(dopamine.getLevel() == 1.0f);
    
    // Test clamping - levels > 1.0 should be clamped to 1.0
    dopamine.setLevel(1.5f);
    assert(dopamine.getLevel() == 1.0f);
    
    // Test clamping - levels < 0.0 should be clamped to 0.0
    dopamine.setLevel(-0.5f);
    assert(dopamine.getLevel() == 0.0f);
    
    std::cout << "    testDopamineLevelSetting passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // Test default plasticity factor
    assert(dopamine.getPlasticityFactor() == 0.5f);
    
    dopamine.setLevel(0.0f);
    assert(dopamine.getPlasticityFactor() == 0.5f); // 0.5 + 0.5 * 0.0 = 0.5
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getPlasticityFactor() == 0.75f); // 0.5 + 0.5 * 0.5 = 0.75
    
    dopamine.setLevel(1.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f); // 0.5 + 0.5 * 1.0 = 1.0
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void testDopamineUpdate() {
    nlm::Dopamine dopamine;
    
    // Test default decay (level should decrease over time)
    float initialLevel = dopamine.getLevel();
    dopamine.update(1.0f); // 1.0 second timestep
    float afterUpdate = dopamine.getLevel();
    
    // With default decay rate of 0.1, level should decrease
    assert(afterUpdate <= initialLevel);
    
    std::cout << "    testDopamineUpdate passed" << std::endl;
}

void testDopamineRewardSignaling() {
    nlm::Dopamine dopamine;
    
    // Test reward signaling
    dopamine.signalReward(0.5f);
    assert(dopamine.getLevel() > 0.0f); // Level should increase
    
    dopamine.signalReward(-0.5f); // Negative reward
    assert(dopamine.getLevel() >= 0.0f); // Level should not go negative
    
    std::cout << "    testDopamineRewardSignaling passed" << std::endl;
}

void testDopaminePredictionError() {
    nlm::Dopamine dopamine;
    
    // Test prediction error signaling
    dopamine.signalRewardPredictionError(0.3f);
    assert(dopamine.getLevel() > 0.0f);
    
    dopamine.signalRewardPredictionError(-0.3f); // Negative prediction error
    assert(dopamine.getLevel() >= 0.0f); // Level should not go negative
    
    std::cout << "    testDopaminePredictionError passed" << std::endl;
}

void testDopamineIntegration() {
    // Test integration with Brain (if available)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::Dopamine dopamine;
    
    // Test that dopamine can be initialized with brain (if method exists)
    // This depends on the Dopamine API
    // For now, just test that dopamine can exist independently
    assert(dopamine.getName() != nullptr);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Dopamine tests..." << std::endl;
    testDopamineCreation();
    testDopamineConfiguration();
    testDopamineLevelSetting();
    testDopaminePlasticityFactor();
    testDopamineUpdate();
    testDopamineRewardSignaling();
    testDopaminePredictionError();
    testDopamineIntegration();
}

} // namespace test_dopamine
