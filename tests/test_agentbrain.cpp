// AgentBrain Tests
// Phase 3: World Interaction Tests

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::AgentBrain agentBrain(brain);
    
    // Verify basic properties
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize with world
    agentBrain.initialize(*world);
    
    // Verify initialization worked
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Create a simple percept
    nlm::SensoryPercept percept;
    percept.setVision(std::vector<float>(256, 0.5f)); // 16x16 vision
    percept.setTouch(std::vector<float>(8, 0.0f));
    percept.setInternal(std::vector<float>(4, 0.0f));
    
    // Process the percept
    agentBrain.processSensoryInput(percept);
    
    // Should not crash
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Get a motor command
    auto motorCmd = agentBrain.decodeMotorCommand();
    
    // Should return a valid command
    assert(motorCmd == nlm::MotorCommand::Wait || 
           motorCmd == nlm::MotorCommand::MoveForward ||
           motorCmd == nlm::MotorCommand::MoveBackward ||
           motorCmd == nlm::MotorCommand::TurnLeft ||
           motorCmd == nlm::MotorCommand::TurnRight ||
           motorCmd == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Test reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Check that neuromodulation state is updated
    assert(agentBrain.getPredictionError() == 0.5f);
    assert(agentBrain.getNeuromodulationLevel() > -1.0f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Update development
    agentBrain.updateDevelopment(10.0);
    
    // Check developmental stage
    auto stage = agentBrain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial ||
           stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation ||
           stage == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainSubsystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Test enabling/disabling subsystems
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    // Now disable them
    agentBrain.enableRewardModulation(false);
    agentBrain.enableStructuralPlasticity(false);
    agentBrain.enableDevelopment(false);
    agentBrain.enableCuriosity(false);
    
    assert(agentBrain.isRewardModulationEnabled() == false);
    assert(agentBrain.isStructuralPlasticityEnabled() == false);
    assert(agentBrain.isDevelopmentEnabled() == false);
    assert(agentBrain.isCuriosityEnabled() == false);
    
    std::cout << "    testAgentBrainSubsystems passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Modify some state
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Reset
    agentBrain.reset();
    
    // Check that state was reset
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainCompleteWorkflow() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    
    // Create a simple world setup
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Run a few steps of a complete workflow
    for (int step = 0; step < 10; ++step) {
        // Update world
        world->update(0.1);
        
        // Get percept from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agentBrain.processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Get motor command
        auto motorCmd = agentBrain.decodeMotorCommand();
        
        // Apply motor command (in this case to world)
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply some reward based on position
        float reward = world->getAgentBody().getEnergy() / 100.0f;
        agentBrain.applyRewardModulation(reward, agentBrain.getPredictionError());
        
        // Update development
        agentBrain.updateDevelopment(0.1);
    }
    
    // Verify everything still works
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCompleteWorkflow passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainSubsystems();
    testAgentBrainReset();
    testAgentBrainCompleteWorkflow();
}

} // namespace test_agentbrain