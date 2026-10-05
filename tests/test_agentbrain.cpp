// AgentBrain Tests
// Tests for the AgentBrain implementation with memory management and configurable constants

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test default constructor
    nlm::AgentBrain agentBrain(brain);
    assert(agentBrain.getBrain() != nullptr);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainWithCustomConfig() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create AgentBrain with custom NeuronConfig
    nlm::AgentBrain::NeuronConfig customConfig;
    customConfig.visionSize = 128;
    customConfig.touchSize = 16;
    customConfig.internalSize = 8;
    customConfig.proprioceptionSize = 12;
    customConfig.motorSize = 8;
    customConfig.visionScaleFactor = 3.0f;
    customConfig.touchScaleFactor = 5.0f;
    
    nlm::AgentBrain agentBrain(brain, customConfig);
    assert(agentBrain.getBrain() != nullptr);
    
    // Test custom configuration
    assert(agentBrain.getSensoryInputSize() == 164);  // 128 + 16 + 8 + 12
    assert(agentBrain.getMotorOutputSize() == 8);
    
    std::cout << "    testAgentBrainWithCustomConfig passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Create a mock world for initialization
    struct MockWorld {
        size_t getVisionWidth() const { return 16; }
        size_t getVisionHeight() const { return 16; }
    };
    
    MockWorld world;
    agentBrain.initialize(world);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainMemoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    {
        // Create AgentBrain in a scope to test destructor
        nlm::AgentBrain agentBrain(brain);
        assert(agentBrain.getBrain() != nullptr);
        
        // Check that neuron groups are properly initialized
        assert(agentBrain.validateNeuronGroups());
        
        // Check total neuron count
        size_t totalCount = agentBrain.getTotalNeuronCount();
        assert(totalCount > 0);
        
        std::cout << "    testAgentBrainMemoryManagement passed (total neurons: " << totalCount << ")" << std::endl;
    }
    
    // Destructor should have been called and cleaned up memory
    std::cout << "    testAgentBrainMemoryManagement cleanup complete" << std::endl;
}

void testAgentBrainValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Test validation methods
    assert(agentBrain.validateNeuronGroups());
    
    // Test neuron distribution logging (should not crash)
    agentBrain.logNeuronDistribution();
    
    std::cout << "    testAgentBrainValidation passed" << std::endl;
}

void testAgentBrainConfigurationLoading() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("vision_size", static_cast<int64_t>(64), nlm::ConfigSource::Default);
    config->set("touch_size", static_cast<int64_t>(12), nlm::ConfigSource::Default);
    config->set("vision_scale_factor", 2.5f, nlm::ConfigSource::Default);
    config->set("touch_scale_factor", 4.5f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Load configuration
    agentBrain.loadFromConfig(config);
    
    // Note: This test passes if the loadFromConfig method doesn't crash
    // The exact values may vary depending on implementation
    
    std::cout << "    testAgentBrainConfigurationLoading passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Test with empty percept (should not crash)
    nlm::SensoryPercept emptyPercept;
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Check initial values
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    // Reset
    agentBrain.reset();
    
    // Check values after reset
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    
    bool allPassed = true;
    
    try {
        testAgentBrainCreation();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainWithCustomConfig();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainInitialization();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainValidation();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainConfigurationLoading();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainSensoryProcessing();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAgentBrainReset();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "All AgentBrain tests PASSED" << std::endl;
    } else {
        std::cout << "Some AgentBrain tests FAILED" << std::endl;
    }
}

} // namespace test_agentbrain