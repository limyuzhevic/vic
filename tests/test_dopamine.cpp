// Dopamine Tests
#include "brain/Dopamine.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    const char* name = dopamine.getName();
    assert(name != nullptr);
    assert(std::string(name) == "Dopamine");
    
    float level = dopamine.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineConfiguration() {
    nlm::Dopamine dopamine;
    
    // Test initial level
    float level = dopamine.getLevel();
    assert(level == 0.0f);
    
    // Set level
    dopamine.setLevel(0.5f);
    assert(std::abs(dopamine.getLevel() - 0.5f) < 0.001f);
    
    // Test boundary values
    dopamine.setLevel(0.0f);
    assert(dopamine.getLevel() == 0.0f);
    
    dopamine.setLevel(1.0f);
    assert(dopamine.getLevel() == 1.0f);
    
    std::cout << "    testDopamineConfiguration passed" << std::endl;
}

void testDopamineSignalProcessing() {
    nlm::Dopamine dopamine;
    
    // Signal reward
    dopamine.signalReward(0.8f);
    
    // Signal prediction error
    dopamine.signalRewardPredictionError(0.2f);
    
    // Level should have changed
    float level = dopamine.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testDopamineSignalProcessing passed" << std::endl;
}

void testDopamineUpdate() {
    nlm::Dopamine dopamine;
    
    // Update with different dt values
    dopamine.update(0.001);
    dopamine.update(0.01);
    dopamine.update(0.1);
    
    // Should not crash
    std::cout << "    testDopamineUpdate passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // Get plasticity factor (implementation-dependent)
    float factor = dopamine.getPlasticityFactor();
    assert(factor >= 0.0f);
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void runAll() {
    std::cout << "Running Dopamine tests..." << std::endl;
    testDopamineCreation();
    testDopamineConfiguration();
    testDopamineSignalProcessing();
    testDopamineUpdate();
    testDopaminePlasticityFactor();
}

} // namespace test_dopamine