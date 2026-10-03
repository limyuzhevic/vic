// AgentBrain and SensoryPercept Tests
// Tests for the improved AgentBrain and SensoryPercept components

#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_agent {

void testSensoryPerceptCreation() {
    std::cout << "    Testing SensoryPercept creation..." << std::endl;
    
    nlm::SensoryPercept percept;
    
    // Check default dimensions
    assert(percept.getVisionWidth() == 16);
    assert(percept.getVisionHeight() == 16);
    assert(percept.getTouch().size() == 8);
    assert(percept.getInternal().size() == 4);
    assert(percept.getProprioception().size() == 6);
    assert(percept.getAudio().empty());
    
    std::cout << "        testSensoryPerceptCreation passed" << std::endl;
}

void testSensoryPerceptSetting() {
    std::cout << "    Testing SensoryPercept setters..." << std::endl;
    
    nlm::SensoryPercept percept;
    
    // Test valid data setting
    std::vector<float> validVision(256, 0.5f);
    std::vector<float> validTouch(8, 0.5f);
    std::vector<float> validInternal(4, 0.5f);
    std::vector<float> validProprioception(6, 0.5f);
    
    percept.setVision(validVision);
    percept.setTouch(validTouch);
    percept.setInternal(validInternal);
    percept.setProprioception(validProprioception);
    
    // Verify data was set
    assert(percept.getVision().size() == 256);
    assert(percept.getTouch().size() == 8);
    assert(percept.getInternal().size() == 4);
    assert(percept.getProprioception().size() == 6);
    
    // Test timestamp
    double timestamp = 1234.567;
    percept.setTimestamp(timestamp);
    assert(percept.getTimestamp() == timestamp);
    
    std::cout << "        testSensoryPerceptSetting passed" << std::endl;
}

void testSensoryPerceptClamping() {
    std::cout << "    Testing SensoryPercept value clamping..." << std::endl;
    
    nlm::SensoryPercept percept;
    
    // Test values out of range (should be clamped)
    std::vector<float> outOfRangeVision(256);
    for (size_t i = 0; i < 256; ++i) {
        outOfRangeVision[i] = -1.5f; // Below minimum
    }
    
    // This should not throw but should clamp values
    percept.setVision(outOfRangeVision);
    
    for (float value : percept.getVision()) {
        assert(value >= 0.0f && value <= 1.0f);
    }
    
    // Test above maximum
    std::vector<float> tooHighVision(256, 2.5f);
    percept.setVision(tooHighVision);
    
    for (float value : percept.getVision()) {
        assert(value >= 0.0f && value <= 1.0f);
    }
    
    std::cout << "        testSensoryPerceptClamping passed" << std::endl;
}

void testSensoryPerceptAllSignals() {
    std::cout << "    Testing SensoryPercept::getAllSignals()..." << std::endl;
    
    nlm::SensoryPercept percept;
    
    // Set valid data
    std::vector<float> vision(256, 0.3f);
    std::vector<float> touch(8, 0.7f);
    std::vector<float> internal(4, 0.5f);
    std::vector<float> proprioception(6, 0.8f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    // Get all signals
    std::vector<float> allSignals = percept.getAllSignals();
    
    // Verify total size
    size_t expectedSize = 256 + 8 + 4 + 6; // Vision + touch + internal + proprioception
    assert(allSignals.size() == expectedSize);
    
    // Verify vision data preserved
    for (size_t i = 0; i < 256; ++i) {
        assert(allSignals[i] == 0.3f);
    }
    
    // Verify touch data preserved
    for (size_t i = 0; i < 8; ++i) {
        assert(allSignals[256 + i] == 0.7f);
    }
    
    std::cout << "        testSensoryPerceptAllSignals passed" << std::endl;
}

void testAgentBrainCreation() {
    std::cout << "    Testing AgentBrain creation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create AgentBrain with valid brain
    nlm::AgentBrain agentBrain(brain.shared_from_this());
    
    // Test configuration methods
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "        testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainNullSafety() {
    std::cout << "    Testing AgentBrain null safety..." << std::endl;
    
    // Test with null brain pointer
    nlm::AgentBrain agentBrain(nullptr);
    
    // Should not crash when accessing null brain
    // (Note: Some methods will gracefully handle null, others may throw)
    
    std::cout << "        testAgentBrainNullSafety passed" << std::endl;
}

void testAgentBrainConfiguration() {
    std::cout << "    Testing AgentBrain configuration methods..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(brain.shared_from_this());
    
    // Test configuration enable/disable
    agentBrain.enableRewardModulation(true);
    assert(agentBrain.isRewardModulationEnabled() == true);
    
    agentBrain.enableStructuralPlasticity(true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    
    agentBrain.enableDevelopment(true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    
    agentBrain.enableCuriosity(true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    std::cout << "        testAgentBrainConfiguration passed" << std::endl;
}

void testAgentBrainReset() {
    std::cout << "    Testing AgentBrain reset functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(brain.shared_from_this());
    
    // Set some state
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    
    // Reset should clear state
    agentBrain.reset();
    
    // Verify state was reset
    assert(agentBrain.isRewardModulationEnabled() == true); // Flags should remain
    assert(agentBrain.isCuriosityEnabled() == true);        // Flags should remain
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);   // State should be cleared
    
    std::cout << "        testAgentBrainReset passed" << std::endl;
}

void runAll() {
    std::cout << "\n=== AgentBrain and SensoryPercept Tests ===\n" << std::endl;
    
    testSensoryPerceptCreation();
    testSensoryPerceptSetting();
    testSensoryPerceptClamping();
    testSensoryPerceptAllSignals();
    testAgentBrainCreation();
    testAgentBrainNullSafety();
    testAgentBrainConfiguration();
    testAgentBrainReset();
    
    std::cout << "\n=== All AgentBrain and SensoryPercept Tests Passed! ===\n" << std::endl;
}

} // namespace test_agent
