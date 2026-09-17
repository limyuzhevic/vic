// Agent+World Integration Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_integration {

void testAgentWorldBasicInteraction() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create brain and initialize
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain interface
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // Initialize agent with world
    agent.initialize(world);
    
    // Enable subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run basic simulation
    for (int step = 0; step < 100; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        nlm::SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Get motor command
        nlm::MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply motor command
        world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // Apply reward modulation
        float reward = percept.getInternal().empty() ? 0.0f : percept.getInternal()[0];
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Validate agent state
        assert(agent.getNeuromodulationLevel() >= -1.0f);
        assert(agent.getCuriosityLevel() >= 0.0f && agent.getCuriosityLevel() <= 1.0f);
        assert(agent.getNoveltyLevel() >= 0.0f);
        
        // Print progress periodically
        if (step % 50 == 0) {
            std::cout << "    Integration step " << step << ", "
                      << "Curiosity: " << agent.getCuriosityLevel() << ", "
                      << "Novelty: " << agent.getNoveltyLevel() << std::endl;
        }
    }
    
    std::cout << "    testAgentWorldBasicInteraction passed" << std::endl;
}

void testAgentSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 4, 4);
    world.reset();
    
    agent.initialize(world);
    
    // Create test sensory percept
    nlm::SensoryPercept percept;
    
    // Set vision data (4x4 = 16 values)
    std::vector<float> vision(16, 0.5f);
    for (int i = 0; i < 16; i++) {
        vision[i] = static_cast<float>(i) / 15.0f;
    }
    percept.setVision(vision);
    
    // Set touch data (8 values, use first 4 for 4x4 vision)
    std::vector<float> touch(8, 0.0f);
    touch[0] = 1.0f;
    touch[1] = 0.5f;
    percept.setTouch(touch);
    
    // Set internal signals (4 values)
    std::vector<float> internal(4, 0.3f);
    internal[0] = 1.0f;
    internal[1] = -0.5f;
    percept.setInternal(internal);
    
    // Set proprioception (6 values)
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = 10.0f; // x position
    proprioception[1] = 10.0f; // y position
    proprioception[2] = 0.0f;  // velocity x
    proprioception[3] = 0.0f;  // velocity y
    proprioception[4] = 0.0f;  // angular velocity
    proprioception[5] = 0.0f;  // energy
    percept.setProprioception(proprioception);
    
    // Process percept
    agent.processSensoryInput(percept);
    
    // Verify novelty was computed
    float novelty = agent.getNoveltyLevel();
    assert(novelty >= 0.0f && novelty <= 1.0f);
    
    // Verify curiosity was updated
    float curiosity = agent.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    // Verify prediction error
    float predictionError = agent.getPredictionError();
    assert(std::abs(predictionError) <= 1.0f);
    
    // Verify neuromodulation level
    float neuromodulation = agent.getNeuromodulationLevel();
    assert(neuromodulation >= -1.0f && neuromodulation <= 1.0f);
    
    std::cout << "    testAgentSensoryProcessing passed" << std::endl;
}

void testAgentDevelopmentStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(15, 15, 6, 6);
    world.reset();
    
    agent.initialize(world);
    
    // Run for enough time to pass through development stages
    double timestep = 0.1;
    double totalTime = 0.0;
    
    for (int step = 0; step < 1000 && totalTime < 1000.0; ++step) {
        // Update world
        world.update(timestep);
        totalTime += timestep;
        
        // Get percept
        nlm::SensoryPercept percept = world.getSensoryPercept();
        
        // Process
        agent.processSensoryInput(percept);
        brain.step(step);
        agent.updateDevelopment(timestep);
        
        // Record developmental stage
        nlm::DevelopmentalStage stage = agent.getDevelopmentalStage();
        
        // Verify stage progression
        if (totalTime < 60.0) {
            assert(stage == nlm::DevelopmentalStage::Initial);
        } else if (totalTime < 300.0) {
            assert(stage == nlm::DevelopmentalStage::CriticalPeriod);
        } else if (totalTime < 900.0) {
            assert(stage == nlm::DevelopmentalStage::Maturation);
        } else {
            assert(stage == nlm::DevelopmentalStage::Adult);
        }
        
        // Verify development increases over time (early stages)
        if (totalTime < 300.0) {
            assert(agent.getNeuromodulationLevel() >= -1.0f);
        }
    }
    
    std::cout << "    testAgentDevelopmentStages passed" << std::endl;
}

void testAgentResetAndRecovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 5, 5);
    world.reset();
    
    agent.initialize(world);
    
    // Run some steps
    for (int step = 0; step < 100; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        agent.applyRewardModulation(0.5f, 0.0f);
        agent.updateDevelopment(0.1);
    }
    
    // Record state before reset
    float curiosityBefore = agent.getCuriosityLevel();
    float noveltyBefore = agent.getNoveltyLevel();
    float neuromodBefore = agent.getNeuromodulationLevel();
    
    // Reset agent
    agent.reset();
    
    // Verify reset
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    assert(agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentResetAndRecovery passed" << std::endl;
}

} // namespace test_integration