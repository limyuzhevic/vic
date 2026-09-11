// AgentBrain Tests
// Tests for the AgentBrain class that connects brain to world

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 6, 6);
    agentBrain.initialize(world);
    
    const auto& percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    auto command = agentBrain.decodeMotorCommand();
    assert(command != nullptr);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 6, 6);
    agentBrain.initialize(world);
    
    auto command = agentBrain.decodeMotorCommand();
    assert(command != nullptr);
    
    // Verify command has valid motor values
    assert(command->getForward() >= -1.0f && command->getForward() <= 1.0f);
    assert(command->getTurn() >= -1.0f && command->getTurn() <= 1.0f);
    assert(command->getInteract() >= 0.0f && command->getInteract() <= 1.0f);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(30.0f, 30.0f, 8, 8);
    agentBrain.initialize(world);
    
    // Apply different reward levels
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    float level1 = agentBrain.getNeuromodulationLevel();
    assert(std::abs(level1) <= 1.0f);
    
    agentBrain.applyRewardModulation(-0.5f, -0.2f);
    float level2 = agentBrain.getNeuromodulationLevel();
    assert(std::abs(level2) <= 1.0f);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(25.0f, 25.0f, 6, 6);
    agentBrain.initialize(world);
    
    // Enable development
    agentBrain.enableDevelopment(true);
    assert(agentBrain.isDevelopmentEnabled());
    
    // Update development
    double timestep = 0.001;
    agentBrain.updateDevelopment(timestep);
    
    DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
    assert(stage >= DevelopmentalStage::Juvenile && stage <= DevelopmentalStage::Adult);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainCuriosity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(35.0f, 35.0f, 8, 8);
    agentBrain.initialize(world);
    
    // Enable curiosity
    agentBrain.enableCuriosity(true);
    assert(agentBrain.isCuriosityEnabled());
    
    float curiosity = agentBrain.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    std::cout << "    testAgentBrainCuriosity passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain Tests..." << std::endl;
    std::cout << std::endl;
    
    testAgentBrainCreation();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainCuriosity();
    
    std::cout << std::endl;
    std::cout << "=== All AgentBrain Tests PASSED ===" << std::endl;
}

} // namespace test_agentbrain
