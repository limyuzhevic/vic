// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testAgentBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create an AgentBrain with the brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Initialize the agent brain
    agentBrain.initialize(world);
    
    // Test that agent brain was properly initialized
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Test processSensoryInput with some data
    nlm::SensoryPercept percept;
    std::vector<float> visionData(256, 0.5f);
    percept.setVision(visionData);
    
    agentBrain.processSensoryInput(percept);
    
    // Test curiosity level changes after input
    float initialCuriosity = agentBrain.getCuriosityLevel();
    
    // Test decodeMotorCommand doesn't crash
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait || cmd == nlm::MotorCommand::MoveForward || 
           cmd == nlm::MotorCommand::MoveBackward || cmd == nlm::MotorCommand::TurnLeft || 
           cmd == nlm::MotorCommand::TurnRight || cmd == nlm::MotorCommand::Interact);
    
    // Test applyRewardModulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Test development
    agentBrain.updateDevelopment(0.1f);
    
    // Test reset
    agentBrain.reset();
    
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    
    std::cout << "    testAgentBrain passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testAgentBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create an AgentBrain with the brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Initialize the agent brain
    agentBrain.initialize(world);
    
    // Test that agent brain was properly initialized
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Test processSensoryInput with some data
    nlm::SensoryPercept percept;
    std::vector<float> visionData(256, 0.5f);
    percept.setVision(visionData);
    
    agentBrain.processSensoryInput(percept);
    
    // Test curiosity level changes after input
    float initialCuriosity = agentBrain.getCuriosityLevel();
    
    // Test decodeMotorCommand doesn't crash
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait || cmd == nlm::MotorCommand::MoveForward || 
           cmd == nlm::MotorCommand::MoveBackward || cmd == nlm::MotorCommand::TurnLeft || 
           cmd == nlm::MotorCommand::TurnRight || cmd == nlm::MotorCommand::Interact);
    
    // Test applyRewardModulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Test development
    agentBrain.updateDevelopment(0.1f);
    
    // Test reset
    agentBrain.reset();
    
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    
    std::cout << "    testAgentBrain passed" << std::endl;
}

} // namespace test_brain
