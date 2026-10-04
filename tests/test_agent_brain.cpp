// Agent Brain Tests
// Tests AgentBrain interface and brain-world integration

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    assert(agent.getBrain() == brain.get());
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialize() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create simple world
    nlm::SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    // Initialize agent
    agent.initialize(world);
    
    // Should have sensory input size
    assert(agent.getSensoryInputSize() > 0);
    
    // Should have motor output size
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialize passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Create sensory percept
    auto percept = world.getSensoryPercept();
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Should not crash and should process input
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Decode motor command
    auto cmd = agent.decodeMotorCommand();
    
    // Should return a motor command
    assert(cmd != nullptr);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Enable subsystems
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.5f);
    
    // Update development
    agent.updateDevelopment(0.001);
    
    // Get neuromodulation levels
    float neuromodLevel = agent.getNeuromodulationLevel();
    float curiosityLevel = agent.getCuriosityLevel();
    float noveltyLevel = agent.getNoveltyLevel();
    
    // Should return valid values (possibly 0.0 if no activity)
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Reset agent
    agent.reset();
    
    // Should work without crashing
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialize();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainReset();
}

} // namespace test_agent_brain
