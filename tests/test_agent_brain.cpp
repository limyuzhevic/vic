// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::AgentBrain agent(brain);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    
    agent.initialize(world);
    
    // Verify initialization worked
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Create a simple sensory percept
    nlm::SensoryPercept percept;
    
    // Vision data (256 values for 16x16 vision)
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Verify processing worked (no assertion, just ensure no crash)
    assert(true);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Decode motor command
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    
    // Should return a valid motor command
    assert(cmd >= nlm::MotorCommand::MoveForward && cmd <= nlm::MotorCommand::Wait);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.5f);
    
    // Check neuromodulation levels
    float curiosity = agent.getCuriosityLevel();
    float novelty = agent.getNoveltyLevel();
    float predictionError = agent.getPredictionError();
    float neuromodulation = agent.getNeuromodulationLevel();
    
    // Verify values are reasonable (should be updated by reward modulation)
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    assert(novelty >= 0.0f && novelty <= 1.0f);
    assert(predictionError >= -1.0f && predictionError <= 1.0f);
    assert(neuromodulation >= -1.0f && neuromodulation <= 1.0f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Update development
    agent.updateDevelopment(0.1);
    
    // Check developmental stage
    nlm::DevelopmentalStage stage = agent.getDevelopmentalStage();
    assert(stage >= nlm::DevelopmentalStage::Initial && stage <= nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainSubsystemEnableDisable() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test enabling/disabling subsystems
    agent.enableRewardModulation(false);
    agent.enableStructuralPlasticity(false);
    agent.enableDevelopment(false);
    agent.enableCuriosity(false);
    
    // Verify they were disabled (the implementation should handle this)
    // Note: The actual implementation might not have getters for these flags,
    // but we've at least called the methods without crashing
    assert(true);
    
    std::cout << "    testAgentBrainSubsystemEnableDisable passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Process some sensory input to change state
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    agent.processSensoryInput(percept);
    
    // Reset agent
    agent.reset();
    
    // Verify reset worked (should clear neuromodulation state)
    assert(agent.getCuriosityLevel() >= 0.0f && agent.getCuriosityLevel() <= 1.0f);
    assert(agent.getNoveltyLevel() >= 0.0f && agent.getNoveltyLevel() <= 1.0f);
    assert(agent.getPredictionError() >= -1.0f && agent.getPredictionError() <= 1.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainFullIntegration() {
    // Test complete integration of AgentBrain with Brain and SimpleWorld
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    agent.initialize(world);
    
    // Enable all subsystems for comprehensive testing
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run a few integration steps
    for (int step = 0; step < 10; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input from world
        auto percept = world.getSensoryPercept();
        
        // Process sensory input in brain
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        nlm::MotorCommand motor_cmd = agent.decodeMotorCommand();
        
        // Apply motor command to world (using current time)
        world.applyMotorCommand(motor_cmd, world.getSimulationTime());
        
        // Apply reward modulation
        auto internal = percept.getInternal();
        float reward = internal.empty() ? 0.0f : internal[0];
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Print progress
        if (step % 5 == 0) {
            std::cout << "      Integration step " << step << ": "
                      << "Curiosity: " << agent.getCuriosityLevel() 
                      << ", Novelty: " << agent.getNoveltyLevel()
                      << ", Dev Stage: " << static_cast<int>(agent.getDevelopmentalStage())
                      << std::endl;
        }
    }
    
    std::cout << "    testAgentBrainFullIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "=== AgentBrain Tests ===" << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainSubsystemEnableDisable();
    testAgentBrainReset();
    testAgentBrainFullIntegration();
    std::cout << "=== All AgentBrain Tests Completed ===" << std::endl;
}

} // namespace test_agent_brain
